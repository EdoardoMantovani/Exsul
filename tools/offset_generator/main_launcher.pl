#!/usr/bin/env perl

# Made by Edoardo Mantovani 2026
#
# Download Linux v6+ releases, build an x86_64 defconfig with debug info,
# extract requested struct field offsets with pahole, and emit a C header.

use strict;
use warnings;

use Cwd qw(abs_path getcwd);
use Digest::SHA ();
use File::Basename qw(basename dirname);
use File::Path qw(make_path remove_tree);
use File::Spec;
use Getopt::Long qw(GetOptions);
use IO::Handle ();
use JSON qw(decode_json);
use List::Util qw(max);

$| = 1;

my $ARCH                 = "x86";
my $DEFCONFIG            = "x86_64_defconfig";
my $DEFCONFIG_PATH       = File::Spec->catfile("arch", "x86", "configs", "x86_64_defconfig");
my $OBJTOOL_FIX_PATH     = File::Spec->catfile("tools", "objtool", "elf.c");
my $DEFAULT_LINUX_REPO   = "https://github.com/torvalds/linux.git";
my $DEFAULT_ARCHIVE_BASE = "https://github.com/torvalds/linux/archive/refs/tags";

my $CONFIG_ADDITION = <<'CONFIG';
CONFIG_DEBUG_INFO=y
CONFIG_AS_HAS_NON_CONST_LEB128=y
CONFIG_DEBUG_INFO_DWARF5=y
CONFIG_DEBUG_INFO_REDUCED=n
CONFIG_DEBUG_INFO_COMPRESSED=n
CONFIG_DEBUG_INFO_SPLIT=n
CONFIG_PAHOLE_HAS_SPLIT_BTF=n
CONFIG_GDB_SCRIPTS=n
CONFIG

my %opt = (
	json          => "required_offsets.json",
	output        => "offset_backup_solution.h",
	state         => ".offset_generator_state.json",
	workdir       => ".offset_generator_work",
	from_tag      => "v6.0",
	to_tag        => undef,
	tags          => undef,
	include_rc    => 0,
	jobs          => $ENV{JOBS} || 16,
	make_target   => "vmlinux",
	pahole_input  => "vmlinux",
	keep_builds   => 0,
	force         => 0,
	stop_on_error => 0,
	objtool_fix   => 1,
	linux_repo    => $DEFAULT_LINUX_REPO,
	archive_base  => $DEFAULT_ARCHIVE_BASE,
	help          => 0,
);

GetOptions(
	"json=s"           => \$opt{json},
	"output=s"         => \$opt{output},
	"state=s"          => \$opt{state},
	"workdir=s"        => \$opt{workdir},
	"from-tag|from=s"  => \$opt{from_tag},
	"to-tag|to=s"      => \$opt{to_tag},
	"tags=s"           => \$opt{tags},
	"include-rc!"      => \$opt{include_rc},
	"jobs=i"           => \$opt{jobs},
	"make-target=s"    => \$opt{make_target},
	"pahole-input=s"   => \$opt{pahole_input},
	"keep-builds!"     => \$opt{keep_builds},
	"force!"           => \$opt{force},
	"stop-on-error!"   => \$opt{stop_on_error},
	"objtool-fix!"     => \$opt{objtool_fix},
	"linux-repo=s"     => \$opt{linux_repo},
	"archive-base=s"   => \$opt{archive_base},
	"help|h"           => \$opt{help},
) or usage(1);

usage(0) if $opt{help};

if (@ARGV == 1) {
	$opt{json} = shift @ARGV;
} elsif (@ARGV > 1) {
	usage(1);
}

main();

sub usage {
	my ($exit_code) = @_;
	print <<"USAGE";
Usage: $0 [options] [required_offsets.json]

Options:
  --json FILE             Offset JSON input (default: required_offsets.json)
  --output FILE           Generated C header (default: offset_backup_solution.h)
  --workdir DIR           Download/build/log directory (default: .offset_generator_work)
  --state FILE            Resume state file (default: .offset_generator_state.json)
  --from-tag TAG          First Linux tag to include (default: v6.0)
  --to-tag TAG            Last Linux tag to include, inclusive
  --tags LIST             Comma-separated explicit tags; skips remote tag filtering
  --include-rc            Include release candidate tags
  --jobs N                make -j value (default: JOBS env or 16)
  --force                 Rebuild selected tags already present in the state file
  --keep-builds           Keep extracted Linux source trees
  --stop-on-error         Stop on the first failed kernel
  --no-objtool-fix        Do not apply the legacy tools/objtool/elf.c patch
  --linux-repo URL        Git repo used for tag discovery
  --archive-base URL      Base URL for tag zip downloads

The tool writes progress to the state file after each completed kernel, so it
can be stopped and restarted without losing completed offsets.
USAGE
	exit($exit_code);
}

sub main {
	my $root        = getcwd();
	my $json_path   = File::Spec->rel2abs($opt{json}, $root);
	my $output_path = File::Spec->rel2abs($opt{output}, $root);
	my $state_path  = File::Spec->rel2abs($opt{state}, $root);
	my $workdir     = File::Spec->rel2abs($opt{workdir}, $root);

	require_tools(qw(git make pahole wget unzip));
	make_path($workdir);
	make_path(File::Spec->catdir($workdir, "downloads"));
	make_path(File::Spec->catdir($workdir, "sources"));
	make_path(File::Spec->catdir($workdir, "logs"));

	my $structs = load_struct_map($json_path);
	my $input_sha256 = sha256_file($json_path);
	my $state = load_state($state_path, $input_sha256, $opt{force});
	my @tags = discover_linux_tags($root);

	if ($opt{force}) {
		my %selected = map { $_->{tag} => 1 } @tags;
		$state->{records} = [
			grep { !$selected{$_->{tag}} } @{ $state->{records} || [] }
		];
		$state->{errors} = [
			grep { !$selected{$_->{tag}} } @{ $state->{errors} || [] }
		];
	}

	my %completed = map { $_->{tag} => 1 } @{ $state->{records} || [] };

	print "[*] selected " . scalar(@tags) . " Linux tag(s)\n";
	write_header($output_path, $state, $structs);

	foreach my $tag (@tags) {
		if ($completed{$tag->{tag}}) {
			print "[*] skipping $tag->{tag}; already present in " . basename($state_path) . "\n";
			next;
		}

		my $record = eval { process_tag($tag, $structs, $workdir) };
		if ($@) {
			my $err = $@;
			chomp($err);
			push @{ $state->{errors} }, {
				tag   => $tag->{tag},
				error => $err,
			};
			save_state($state_path, $state);
			print STDERR "[!] failed $tag->{tag}: $err\n";
			print STDERR "[!] see logs in " . File::Spec->catdir($workdir, "logs") . "\n";
			exit(1) if $opt{stop_on_error};
			next;
		}

		push @{ $state->{records} }, $record;
		save_state($state_path, $state);
		write_header($output_path, $state, $structs);
		print "[*] wrote " . basename($output_path) . " after $tag->{tag}\n";
	}

	write_header($output_path, $state, $structs);
	print "[*] done: $output_path\n";
}

sub load_struct_map {
	my ($path) = @_;
	my $json_text = read_file($path);
	my $data = decode_json($json_text);

	die "$path does not contain a top-level structs array\n"
		unless ref($data->{structs}) eq "ARRAY";

	my @structs;
	foreach my $entry (@{ $data->{structs} }) {
		die "$path contains a non-object struct entry\n"
			unless ref($entry) eq "HASH";
		last if defined($entry->{name}) && $entry->{name} eq "NULL";
		die "$path contains a struct entry without a valid name\n"
			unless defined($entry->{name}) && length($entry->{name});
		die "$path struct '$entry->{name}' has an invalid fields array\n"
			unless ref($entry->{fields}) eq "ARRAY";
		foreach my $field (@{ $entry->{fields} }) {
			die "$path struct '$entry->{name}' contains an invalid field name\n"
				unless defined($field) && !ref($field) && length($field);
		}
		push @structs, {
			name   => $entry->{name},
			fields => [ @{ $entry->{fields} } ],
		};
	}

	die "$path does not request any structs\n" unless @structs;
	return \@structs;
}

sub discover_linux_tags {
	my ($cwd) = @_;
	my $from = parse_linux_tag($opt{from_tag})
		or die "--from-tag must look like v6, v6.0, v6.1.2, or v6.9-rc1\n";
	my $to = undef;
	if (defined($opt{to_tag})) {
		$to = parse_linux_tag($opt{to_tag})
			or die "--to-tag must look like v6, v6.0, v6.1.2, or v6.9-rc1\n";
	}

	my @tags;
	if (defined($opt{tags}) && length($opt{tags})) {
		foreach my $raw_tag (split(/\s*,\s*/, $opt{tags})) {
			my $tag = parse_linux_tag($raw_tag)
				or die "invalid tag in --tags: $raw_tag\n";
			push @tags, $tag;
		}
	} else {
		print "[*] discovering tags from $opt{linux_repo}\n";
		my ($status, $output) = capture_cmd(
			$cwd, "git", "ls-remote", "--tags", $opt{linux_repo}, "refs/tags/v*"
		);
		die "git tag discovery failed:\n$output\n" if $status != 0;

		my %seen;
		foreach my $line (split(/\n/, $output)) {
			my @fields = split(/\s+/, $line);
			next unless @fields >= 2;
			my $tag = parse_linux_tag($fields[1]);
			next unless $tag;
			$seen{ $tag->{tag} } = $tag;
		}
		@tags = values %seen;
	}

	@tags = grep {
		($opt{include_rc} || !defined($_->{rc}))
			&& version_cmp($_, $from) >= 0
			&& (!defined($to) || version_cmp($_, $to) <= 0)
	} @tags;
	@tags = sort { tag_cmp($a, $b) } @tags;

	die "no matching Linux tags found\n" unless @tags;
	return @tags;
}

sub parse_linux_tag {
	my ($raw) = @_;
	return undef unless defined($raw);
	my $tag = $raw;
	chomp($tag);
	$tag =~ s/\^\{\}\z//;
	$tag =~ s{\Arefs/tags/}{};
	return undef unless $tag =~ /\Av(\d+)(?:\.(\d+))?(?:\.(\d+))?(?:-rc(\d+))?\z/;

	return {
		tag        => $tag,
		major      => 0 + $1,
		patchlevel => 0 + ($2 // 0),
		sublevel   => 0 + ($3 // 0),
		rc         => defined($4) ? 0 + $4 : undef,
	};
}

sub version_cmp {
	my ($a, $b) = @_;
	return $a->{major}      <=> $b->{major}
	    || $a->{patchlevel} <=> $b->{patchlevel}
	    || $a->{sublevel}   <=> $b->{sublevel};
}

sub tag_cmp {
	my ($a, $b) = @_;
	my $arc = defined($a->{rc}) ? $a->{rc} : 9999;
	my $brc = defined($b->{rc}) ? $b->{rc} : 9999;
	return version_cmp($a, $b) || $arc <=> $brc;
}

sub process_tag {
	my ($tag, $structs, $workdir) = @_;
	my $downloads_dir = File::Spec->catdir($workdir, "downloads");
	my $sources_dir   = File::Spec->catdir($workdir, "sources");
	my $logs_dir      = File::Spec->catdir($workdir, "logs");
	my $log_file      = File::Spec->catfile($logs_dir, "$tag->{tag}.log");
	unlink $log_file if -e $log_file;

	my $tree;
	my $record = eval {
		my $zip = download_archive($tag, $downloads_dir, $logs_dir);
		$tree = extract_archive($tag, $zip, $sources_dir, $log_file, $workdir);

		print "[*] configuring $tag->{tag}\n";
		append_debug_config($tree);
		patch_objtool($tree) if $opt{objtool_fix};

		my $version = read_kernel_version($tree, $tag);
		printf(
			"[*] building %s (%d.%d.%d%s)\n",
			$tag->{tag},
			$version->{major},
			$version->{patchlevel},
			$version->{sublevel},
			$version->{extraversion},
		);
		build_kernel($tree, $log_file);

		print "[*] extracting offsets for $tag->{tag}\n";
		my ($layouts, $warnings) = extract_offsets($tree, $structs, $log_file);
		foreach my $warning (@$warnings) {
			print "[!] $tag->{tag}: $warning\n";
		}

		{
			tag      => $tag->{tag},
			version  => $version,
			layouts  => $layouts,
			warnings => $warnings,
		};
	};
	my $err = $@;

	if ($tree && !$opt{keep_builds}) {
		safe_remove_tree($tree, $workdir);
	}

	die $err if $err;
	return $record;
}

sub download_archive {
	my ($tag, $downloads_dir, $logs_dir) = @_;
	my $zip = File::Spec->catfile($downloads_dir, "$tag->{tag}.zip");
	return $zip if -s $zip;

	my $url = "$opt{archive_base}/$tag->{tag}.zip";
	my $log = File::Spec->catfile($logs_dir, "downloads.log");
	print "[*] downloading $tag->{tag} from $url\n";

	eval {
		run_cmd(getcwd(), $log, "wget", "-q", "--show-progress", "-O", $zip, $url);
	};
	if ($@) {
		unlink $zip if -e $zip;
		die $@;
	}

	return $zip;
}

sub extract_archive {
	my ($tag, $zip, $sources_dir, $log_file, $workdir) = @_;
	my $extract_root = File::Spec->catdir($sources_dir, $tag->{tag});
	safe_remove_tree($extract_root, $workdir) if -d $extract_root;
	make_path($extract_root);

	print "[*] extracting $tag->{tag}\n";
	run_cmd(getcwd(), $log_file, "unzip", "-q", $zip, "-d", $extract_root);

	opendir(my $dh, $extract_root) or die "opendir $extract_root: $!\n";
	my @dirs = sort grep {
		$_ !~ /\A\./ && -d File::Spec->catdir($extract_root, $_)
	} readdir($dh);
	closedir($dh);

	die "could not find extracted Linux tree under $extract_root\n" unless @dirs;
	return File::Spec->catdir($extract_root, $dirs[0]);
}

sub append_debug_config {
	my ($tree) = @_;
	my $defconfig = File::Spec->catfile($tree, $DEFCONFIG_PATH);
	open(my $fh, ">>", $defconfig) or die "open $defconfig: $!\n";
	print $fh "\n# Added by tools/offset_generator/main_launcher.pl\n";
	print $fh $CONFIG_ADDITION;
	close($fh) or die "close $defconfig: $!\n";
}

sub patch_objtool {
	my ($tree) = @_;
	my $objtool = File::Spec->catfile($tree, $OBJTOOL_FIX_PATH);
	return unless -e $objtool;

	my $content = read_file($objtool);
	my $patched = $content;
	$patched =~ s/return -1/return 0/g;
	return if $patched eq $content;

	open(my $fh, ">", $objtool) or die "open $objtool: $!\n";
	print $fh $patched;
	close($fh) or die "close $objtool: $!\n";
}

sub read_kernel_version {
	my ($tree, $tag) = @_;
	my $makefile = File::Spec->catfile($tree, "Makefile");
	open(my $fh, "<", $makefile) or die "open $makefile: $!\n";
	my %values;
	while (my $line = <$fh>) {
		if ($line =~ /\A([A-Z]+)\s*=\s*(.*?)\s*\z/) {
			$values{$1} = $2;
		}
	}
	close($fh);

	my $extraversion = $values{EXTRAVERSION} // "";
	my $extra = 0;
	$extra = 0 + $1 if $extraversion =~ /rc(\d+)/;

	return {
		major        => numeric_make_value($values{VERSION},    $tag->{major}),
		patchlevel   => numeric_make_value($values{PATCHLEVEL}, $tag->{patchlevel}),
		sublevel     => numeric_make_value($values{SUBLEVEL},   $tag->{sublevel}),
		extra        => $extra,
		extraversion => $extraversion,
	};
}

sub numeric_make_value {
	my ($value, $fallback) = @_;
	return $fallback unless defined($value) && $value =~ /\A\d+\z/;
	return 0 + $value;
}

sub build_kernel {
	my ($tree, $log_file) = @_;
	run_cmd($tree, $log_file, "make", "ARCH=$ARCH", $DEFCONFIG);
	run_cmd(
		$tree,
		$log_file,
		"make",
		"ARCH=$ARCH",
		"KCFLAGS=-Wno-error -w",
		"HOSTCFLAGS=-Wno-error -w",
		"-j$opt{jobs}",
		$opt{make_target},
	);
}

sub extract_offsets {
	my ($tree, $structs, $log_file) = @_;
	my $pahole_target = File::Spec->catfile($tree, $opt{pahole_input});
	die "$pahole_target was not built\n" unless -e $pahole_target;

	my @layouts;
	my @warnings;

	foreach my $struct (@$structs) {
		my ($status, $output) = capture_cmd(
			$tree,
			"pahole",
			"-C",
			$struct->{name},
			$pahole_target,
		);

		if ($status != 0 || $output !~ /\S/) {
			push @warnings, "struct $struct->{name} not found by pahole";
			append_log($log_file, "\n[pahole missing] $struct->{name}\n$output\n");
			next;
		}

		my ($struct_size, $offsets) = parse_pahole_output($output, $struct->{fields});
		my @found_fields;
		my @missing_fields;

		foreach my $field (@{ $struct->{fields} }) {
			if (exists $offsets->{$field}) {
				push @found_fields, {
					name   => $field,
					offset => $offsets->{$field},
				};
			} else {
				push @missing_fields, $field;
			}
		}

		if (@missing_fields) {
			push @warnings,
				"struct $struct->{name}: "
				. scalar(@missing_fields)
				. " missing field(s): "
				. join(", ", @missing_fields);
		}

		push @layouts, {
			struct_name => $struct->{name},
			struct_size => $struct_size,
			fields      => \@found_fields,
		};
	}

	return (\@layouts, \@warnings);
}

sub parse_pahole_output {
	my ($output, $fields) = @_;
	my %requested = map { $_ => 1 } @$fields;
	my %offsets;
	my $struct_size = 0;

	foreach my $line (split(/\n/, $output)) {
		if ($line =~ /\/\*\s*size:\s*(\d+),/) {
			$struct_size = 0 + $1;
			next;
		}

		next unless $line =~ /\/\*\s*(\d+)(?::\s*\d+)?\s+\d+\s*\*\//;
		my $offset = 0 + $1;
		my ($decl) = split(/\/\*/, $line, 2);
		my $field_name = declared_field_name($decl);
		next unless defined($field_name);
		next unless $requested{$field_name};
		$offsets{$field_name} = $offset unless exists $offsets{$field_name};
	}

	return ($struct_size, \%offsets);
}

sub declared_field_name {
	my ($decl) = @_;

	if ($decl =~ /\(\s*\*\s*([A-Za-z_][A-Za-z0-9_]*)\s*\)/) {
		return $1;
	}

	$decl =~ s/\[[^\]]*\]/ /g;
	$decl =~ s/:\s*\d+//g;
	$decl =~ s/\b__attribute__\s*\(\(.*?\)\)//g;
	$decl =~ s/\b__(?:aligned|counted_by|counted_by_or_null)\s*\([^)]*\)//g;
	$decl =~ s/\s+/ /g;

	return $1 if $decl =~ /([A-Za-z_][A-Za-z0-9_]*)\s*;\s*\z/;
	return undef;
}

sub write_header {
	my ($output_path, $state, $structs) = @_;

	my @records = sort {
		version_record_cmp($a, $b) || $a->{tag} cmp $b->{tag}
	} @{ $state->{records} || [] };

	my @layouts;
	foreach my $record (@records) {
		foreach my $layout (@{ $record->{layouts} || [] }) {
			push @layouts, [ $record, $layout ];
		}
	}

	my $max_fields = max(1, map { scalar(@{ $_->{fields} }) } @$structs);
	my $max_struct_name = max(32, map { length($_->{name}) + 1 } @$structs);
	my $max_field_name = max(
		32,
		map { length($_) + 1 } map { @{ $_->{fields} } } @$structs
	);
	my $layout_count = scalar(@layouts);
	my $guard = uc(basename($output_path));
	$guard =~ s/[^A-Z0-9_]/_/g;

	my $tmp_path = "$output_path.tmp";
	open(my $out, ">", $tmp_path) or die "open $tmp_path: $!\n";

	print $out "/* Auto-generated by tools/offset_generator/main_launcher.pl. */\n";
	print $out "/* Do not edit by hand; rerun the generator instead. */\n\n";
	print $out "#ifndef $guard\n";
	print $out "#define $guard\n\n";
	print $out "#define OFFSET_BACKUP_FIELD_NAME_MAX $max_field_name\n";
	print $out "#define OFFSET_BACKUP_STRUCT_NAME_MAX $max_struct_name\n";
	print $out "#define OFFSET_BACKUP_FIELDS_MAX $max_fields\n";
	print $out "#define OFFSET_BACKUP_LAYOUTS_MAX $layout_count\n\n";

	print $out "struct offset_backup_field {\n";
	print $out "\tunsigned char field_name[OFFSET_BACKUP_FIELD_NAME_MAX];\n";
	print $out "\tunsigned short field_offset;\n";
	print $out "};\n\n";

	print $out "struct offset_backup_layout {\n";
	print $out "\tunsigned char kernel_version;\n";
	print $out "\tunsigned char kernel_patch_level;\n";
	print $out "\tunsigned char kernel_sublevel;\n";
	print $out "\tunsigned char kernel_extra_version;\n";
	print $out "\tunsigned short struct_size;\n";
	print $out "\tunsigned char struct_name[OFFSET_BACKUP_STRUCT_NAME_MAX];\n";
	print $out "\tunsigned short struct_field_num;\n";
	print $out "\tstruct offset_backup_field fields[OFFSET_BACKUP_FIELDS_MAX];\n";
	print $out "};\n\n";

	print $out "struct offset_backup_archive {\n";
	print $out "\tunsigned int layout_count;\n";
	print $out "\tstruct offset_backup_layout layouts[OFFSET_BACKUP_LAYOUTS_MAX + 1];\n";
	print $out "};\n\n";

	print $out "static const struct offset_backup_archive kernel_backup_archive = {\n";
	print $out "\t.layout_count = $layout_count,\n";
	print $out "\t.layouts = {\n";

	for (my $i = 0; $i < @layouts; $i++) {
		my ($record, $layout) = @{ $layouts[$i] };
		my $version = $record->{version};
		my $fields = $layout->{fields} || [];

		print $out "\t\t[$i] = {\n";
		print $out "\t\t\t.kernel_version = $version->{major},\n";
		print $out "\t\t\t.kernel_patch_level = $version->{patchlevel},\n";
		print $out "\t\t\t.kernel_sublevel = $version->{sublevel},\n";
		print $out "\t\t\t.kernel_extra_version = $version->{extra},\n";
		print $out "\t\t\t.struct_size = " . ($layout->{struct_size} || 0) . ",\n";
		print $out "\t\t\t.struct_name = \"" . c_escape($layout->{struct_name}) . "\",\n";
		print $out "\t\t\t.struct_field_num = " . scalar(@$fields) . ",\n";

		if (@$fields) {
			print $out "\t\t\t.fields = {\n";
			for (my $j = 0; $j < @$fields; $j++) {
				my $field = $fields->[$j];
				print $out "\t\t\t\t[$j] = {\n";
				print $out "\t\t\t\t\t.field_name = \"" . c_escape($field->{name}) . "\",\n";
				print $out "\t\t\t\t\t.field_offset = $field->{offset},\n";
				print $out "\t\t\t\t},\n";
			}
			print $out "\t\t\t},\n";
		}

		print $out "\t\t},\n";
	}

	print $out "\t},\n";
	print $out "};\n\n";
	print $out "#endif /* $guard */\n";
	close($out) or die "close $tmp_path: $!\n";
	rename($tmp_path, $output_path) or die "rename $tmp_path -> $output_path: $!\n";
}

sub version_record_cmp {
	my ($a, $b) = @_;
	foreach my $key (qw(major patchlevel sublevel extra)) {
		my $cmp = ($a->{version}{$key} || 0) <=> ($b->{version}{$key} || 0);
		return $cmp if $cmp;
	}
	return 0;
}

sub c_escape {
	my ($value) = @_;
	$value =~ s/\\/\\\\/g;
	$value =~ s/"/\\"/g;
	$value =~ s/\n/\\n/g;
	$value =~ s/\r/\\r/g;
	return $value;
}

sub load_state {
	my ($path, $input_sha256, $force) = @_;
	my $state = {
		input_sha256 => $input_sha256,
		records      => [],
		errors       => [],
	};

	return $state unless -e $path;

	my $loaded = decode_json(read_file($path));
	if (($loaded->{input_sha256} // "") ne $input_sha256) {
		die "$path was created for a different JSON input; use --force to reset it\n"
			unless $force;
		return $state;
	}

	$loaded->{records} ||= [];
	$loaded->{errors}  ||= [];
	return $loaded;
}

sub save_state {
	my ($path, $state) = @_;
	$state->{input_sha256} ||= "";
	$state->{records} ||= [];
	$state->{errors}  ||= [];

	my $json = JSON->new->canonical->pretty->encode($state);
	my $tmp = "$path.tmp";
	open(my $fh, ">", $tmp) or die "open $tmp: $!\n";
	print $fh $json;
	close($fh) or die "close $tmp: $!\n";
	rename($tmp, $path) or die "rename $tmp -> $path: $!\n";
}

sub sha256_file {
	my ($path) = @_;
	open(my $fh, "<", $path) or die "open $path: $!\n";
	binmode($fh);
	my $sha = Digest::SHA->new(256);
	$sha->addfile($fh);
	close($fh);
	return $sha->hexdigest;
}

sub read_file {
	my ($path) = @_;
	open(my $fh, "<", $path) or die "open $path: $!\n";
	local $/;
	my $content = <$fh>;
	close($fh);
	return $content;
}

sub append_log {
	my ($path, $content) = @_;
	make_path(dirname($path));
	open(my $fh, ">>", $path) or die "open $path: $!\n";
	print $fh $content;
	close($fh);
}

sub require_tools {
	my @tools = @_;
	my @missing;
	foreach my $tool (@tools) {
		push @missing, $tool unless find_tool($tool);
	}
	die "missing required tool(s): " . join(", ", @missing) . "\n" if @missing;
}

sub find_tool {
	my ($tool) = @_;
	return $tool if File::Spec->file_name_is_absolute($tool) && -x $tool;
	foreach my $dir (split(/:/, $ENV{PATH} || "")) {
		my $candidate = File::Spec->catfile($dir, $tool);
		return $candidate if -x $candidate && !-d $candidate;
	}
	return undef;
}

sub run_cmd {
	my ($cwd, $log_file, @cmd) = @_;
	make_path(dirname($log_file));
	open(my $log, ">>", $log_file) or die "open $log_file: $!\n";
	print $log "\n\$ " . join(" ", map { shell_quote($_) } @cmd) . "\n";
	$log->flush();

	my $pid = fork();
	die "fork failed: $!\n" unless defined($pid);

	if ($pid == 0) {
		chdir($cwd) or die "chdir $cwd: $!\n";
		open(STDOUT, ">&", $log) or die "redirect stdout: $!\n";
		open(STDERR, ">&", $log) or die "redirect stderr: $!\n";
		exec @cmd or do {
			print STDERR "exec failed for @cmd: $!\n";
			exit(127);
		};
	}

	waitpid($pid, 0);
	my $status = $?;
	close($log);

	if ($status == -1) {
		die "waitpid failed for @cmd: $!\n";
	}
	if ($status & 127) {
		die "command died with signal " . ($status & 127) . ": @cmd\n";
	}
	my $exit = $status >> 8;
	die "command failed ($exit): @cmd; see $log_file\n" if $exit != 0;
}

sub capture_cmd {
	my ($cwd, @cmd) = @_;
	my $pid = open(my $fh, "-|");
	die "fork failed for @cmd: $!\n" unless defined($pid);

	if ($pid == 0) {
		chdir($cwd) or die "chdir $cwd: $!\n";
		open(STDERR, ">&", STDOUT) or die "redirect stderr: $!\n";
		exec @cmd or do {
			print STDERR "exec failed for @cmd: $!\n";
			exit(127);
		};
	}

	local $/;
	my $output = <$fh> // "";
	my $ok = close($fh);
	my $status = $ok ? 0 : (($? >> 8) || 1);
	return ($status, $output);
}

sub shell_quote {
	my ($value) = @_;
	return "''" if !defined($value) || $value eq "";
	return $value if $value =~ /\A[A-Za-z0-9_+=:,.\/@-]+\z/;
	$value =~ s/'/'"'"'/g;
	return "'$value'";
}

sub safe_remove_tree {
	my ($path, $allowed_root) = @_;
	return unless -e $path;

	my $real_path = abs_path($path);
	my $real_root = abs_path($allowed_root);
	die "refusing to remove $path outside $allowed_root\n"
		unless defined($real_path)
		&& defined($real_root)
		&& ($real_path eq $real_root || index($real_path, "$real_root/") == 0);

	remove_tree($real_path);
}
