# Made by Edoardo Mantovani, 2026
# Simple tool which takes a config generated from Build.pl, downloads every kernel with debugging informations from
# Ubuntu repositories and extract the offset files

# requirements:
# - curl
# - zstd
# - pahole
# - dpkg-deb (for Ubuntu)
# - JSON
# - WWW::Mechanize

use strict;
use warnings;
use File::Spec;
use JSON qw(decode_json);
use WWW::Mechanize;

my $rm = undef;
my $zstd = undef;
my $curl = undef;
my $dpkg = undef;
my $find = undef;
my $pahole = undef;
my $current_arch = undef;
my $fallback_path = undef;
my $mechanize_object = undef;
my $required_structs = undef;
my $ubuntu_current_link = undef;
my $required_offsets_json = undef;
my $generated_header_path = "distro_struct_offsets.h";
my $generated_header = undef;
my $stdout_fh = undef;
my $header_prefix = "";
my $header_buffer = undef;
my %kernel_record_by_package = ();
my $mac80211_mod_fallback = "mac80211.ko";
my $bluetooth_mod_fallback = "bluetooth.ko";
my $ubuntu_kernel_pkg_folder = "extracted_kernel/";
my $ubuntu_kernel_dbgsym_string = "-generic-dbgsym_";
my $ubuntu_kernel_dbgsym_local_path = "usr/lib/debug/boot/";
my $ubuntu_kernel_repo_string = "http://ddebs.ubuntu.com/pool/main/l/linux/";

my $status = 0;
my $output = 0;
my @ubuntu_min_kernel_version = (5, 15, 0);
my @kernel_offset_records = ();

if(@ARGV > 2){
	print("[*] usage: $0 <required_offsets.json> [distro_struct_offsets.h]\n");
	exit(0);
}

$required_offsets_json = shift || die "pass the offset json file!\n";
$generated_header_path = shift || $generated_header_path;
$required_structs = load_required_offsets($required_offsets_json);

$rm = `which rm`;
if(!$rm){
	print("[!] failed to find the rm command!\n");
	exit(0);
}else{
	chomp($rm);
}

$zstd = `which zstd`;
if(!$zstd){
	print("[!] failed to detect the zstd command!\n");
	exit(0);
}else{
	chomp($zstd);
}

$find = `which find`;
if(!$find){
	print("[!] failed to detect the find command!\n");
	exit(0);
}else{
	chomp($find);
}

$curl = `which curl`;
if(!$curl){
	print("[!] failed to detect a curl installation!\n");
	exit(0);
}else{
	chomp($curl);
}

# for Ubuntu, will need to add the pkg manager also for Alpine, Fedora ecc..
$dpkg = `which dpkg-deb`;
if(!$dpkg){
	print("[!] failed to detect the unzip package!\n");
	exit(0);
}else{
	chomp($dpkg);
}

$pahole = `which pahole`;
if(!$pahole){
	print("[!] failed to detect a pahole installation!\n");
	exit(0);
}else{
	chomp($pahole);
}

# detect the current architecture
$current_arch = `uname -p`;
chomp($current_arch);

if($current_arch eq "x86_64"){
	$current_arch = "amd64";
}

$mechanize_object = WWW::Mechanize->new();
$mechanize_object->get($ubuntu_kernel_repo_string);

# open the final header file
open($header_buffer, ">", \$header_prefix) or die "open header buffer: $!\n";
$stdout_fh = select();
select($header_buffer);
print("/* header automatically created by the 'distro_offset_generator.pl' script */\n");

# first stage: map every kernel version and create the header structure without holding the offsets
print("/* header automatically created by the \'distro_offset_generator.pl\' script */\n\n");
print("enum supported_kernel_versions {\n");
foreach($mechanize_object->find_all_links()){
	if($_->url =~ $ubuntu_kernel_dbgsym_string && $_->url =~ $current_arch && ubuntu_kernel_version_supported($_->url)){
		my $pkg_name = $_->url;
		my @version = ();
		$pkg_name =~ /(\d+)\.(\d+)\.(\d+)-(\d+)-generic-dbgsym_/;
		@version = (0 + $1, 0 + $2, 0 + $3, 0 + $4);
		my $kernel_enum = "LINUX_KERNEL_" . $version[0] . "_" . $version[1] . "_" . $version[2] . "_" . $version[3];
			print("    " . $kernel_enum . "," . "\n");
			push(@kernel_offset_records, {
				kernel_enum => $kernel_enum,
				package => $pkg_name,
				structs => {},
			});
			$kernel_record_by_package{$pkg_name} = $kernel_offset_records[-1];
	}
}
print("};\n\n");
print("/* get_current_kernel_version: automatically created function which returns the enum value correspondent to the actual kernel version detected */\n");
print("static enum supported_kernel_versionsenum supported_kernel_versions get_current_kernel_version(void){\n");
foreach($mechanize_object->find_all_links()){
        if($_->url =~ $ubuntu_kernel_dbgsym_string && $_->url =~ $current_arch && ubuntu_kernel_version_supported($_->url)){
                my $pkg_name = $_->url;
                my @version = ();
                $pkg_name =~ /(\d+)\.(\d+)\.(\d+)-(\d+)-generic-dbgsym_/;
                @version = (0 + $1, 0 + $2, 0 + $3, 0 + $4);
                print("    if(director.kernel_major_version == $version[0] && director.kernel_minor_version == $version[1] && director.kernel_patch == $version[3]){\n");
		print("        return LINUX_KERNEL_" . $version[0] . "_" . $version[1] . "_" . $version[2] . "_" . $version[3] . ";" . "\n");
		print("    }\n");
        }
}
print("    /* no valid kernel version detected! */\n");
print("    return 0xff;\n");
print("}\n\n");
select($stdout_fh);

# second stage: download everything
foreach($mechanize_object->find_all_links()){
	if($_->url =~ $ubuntu_kernel_dbgsym_string && $_->url =~ $current_arch && ubuntu_kernel_version_supported($_->url)){
		my $pkg_name = $_->url;
		my $vmlinux_path = undef;

		print("[*] downloading " . $pkg_name . "\n");
		$ubuntu_current_link = $ubuntu_kernel_repo_string . $pkg_name;
		`$curl --progress-bar --remote-name --location $ubuntu_current_link`;
		print("[*] extracting " . $pkg_name . "\n");
		system($dpkg, "-x", $pkg_name, $ubuntu_kernel_pkg_folder);
		$vmlinux_path = find_extracted_vmlinux($ubuntu_kernel_pkg_folder);
		if(!defined($vmlinux_path)){
			print("[!] failed to find vmlinux inside " . $pkg_name . "\n");
		}else{
			my $kernel_offsets = extract_offsets_from_vmlinux($pkg_name, $vmlinux_path, $required_structs);
				if(defined($kernel_offsets)){
					if(exists($kernel_record_by_package{$pkg_name})){
						$kernel_record_by_package{$pkg_name}->{structs} = $kernel_offsets->{structs};
					}else{
						push(@kernel_offset_records, $kernel_offsets);
						$kernel_record_by_package{$pkg_name} = $kernel_offsets;
					}
				}
		}
		write_generated_header();

		# delete every downloaded '.ddeb' package
		`$rm -rf $ubuntu_kernel_pkg_folder`;
		`$rm -f *.ddeb`;
		$ubuntu_current_link = undef;	 
	}
}

write_generated_header();

sub write_generated_header {
	open($generated_header, ">", $generated_header_path) or die "open $generated_header_path: $!\n";
	print $generated_header $header_prefix;
	select($generated_header);
	emit_kernel_offset_header($required_structs, \@kernel_offset_records);
	select($stdout_fh);
	close($generated_header) or die "close $generated_header_path: $!\n";
	print("[*] wrote " . $generated_header_path . "\n");
}

sub ubuntu_kernel_enum_name {
	my ($pkg_name) = @_;

	return undef unless defined($pkg_name);
	return undef unless $pkg_name =~ /(\d+)\.(\d+)\.(\d+)-(\d+)-generic-dbgsym_/;
	return "LINUX_KERNEL_" . (0 + $1) . "_" . (0 + $2) . "_" . (0 + $3) . "_" . (0 + $4);
}

sub c_identifier {
	my ($name) = @_;

	$name =~ s/[^A-Za-z0-9_]/_/g;
	return $name;
}

sub emit_c_offset_array {
	my ($name, $type, $values) = @_;

	print("static const " . $type . " " . $name . "[] = {\n");
	foreach my $entry (@$values){
		print("    [" . $entry->{kernel_enum} . "] = " . $entry->{value} . ",\n");
	}
	print("};\n\n");
}

sub emit_supported_kernel_arrays {
	my ($records) = @_;

	print("#define DISTRO_SUPPORTED_KERNEL_VERSION_COUNT " . scalar(@$records) . "\n\n");
	print("static const enum supported_kernel_versions distro_supported_kernel_versions[DISTRO_SUPPORTED_KERNEL_VERSION_COUNT] = {\n");
	foreach my $record (@$records){
		print("    " . $record->{kernel_enum} . ",\n");
	}
	print("};\n\n");
	print("static const unsigned char distro_supported_kernel_version_valid[] = {\n");
	foreach my $record (@$records){
		print("    [" . $record->{kernel_enum} . "] = 1,\n");
	}
	print("};\n\n");
}

sub emit_kernel_offset_header {
	my ($structs, $records) = @_;
	my @bindings = ();

	print("\n/* kernel offset arrays generated by 'distro_offset_generator.pl' */\n\n");
	emit_supported_kernel_arrays($records);
	foreach my $struct_idx (0 .. $#$structs){
		my $struct = $structs->[$struct_idx];
		my $struct_name = $struct->{name};
		my $struct_id = c_identifier($struct_name);
		my @size_values = ();
		my @size_valid_values = ();
		my $size_array = $struct_id . "_struct_size_offsets";
		my $size_valid_array = $struct_id . "_struct_size_valid";

		foreach my $record (@$records){
			my $layout = $record->{structs}{$struct_name};
			my $has_size = defined($layout) && defined($layout->{struct_size});

			push(@size_values, {
				kernel_enum => $record->{kernel_enum},
				value => $has_size ? $layout->{struct_size} : 0,
			});
			push(@size_valid_values, {
				kernel_enum => $record->{kernel_enum},
				value => $has_size ? 1 : 0,
			});
		}

		emit_c_offset_array($size_array, "unsigned short", \@size_values);
		emit_c_offset_array($size_valid_array, "unsigned char", \@size_valid_values);
		push(@bindings, "\truntime_structs->structs[" . $struct_idx . "].kernel_sizes = " . $size_array . ";\n");
		push(@bindings, "\truntime_structs->structs[" . $struct_idx . "].kernel_size_valid = " . $size_valid_array . ";\n");

		foreach my $field_idx (0 .. $#{ $struct->{fields} }){
			my $field = $struct->{fields}[$field_idx];
			my $field_id = c_identifier($field);
			my $offset_array = $struct_id . "_" . $field_id . "_offsets";
			my $offset_valid_array = $struct_id . "_" . $field_id . "_offset_valid";
			my @offset_values = ();
			my @offset_valid_values = ();

			foreach my $record (@$records){
				my $layout = $record->{structs}{$struct_name};
				my $has_offset = defined($layout) && exists($layout->{fields}{$field});

				push(@offset_values, {
					kernel_enum => $record->{kernel_enum},
					value => $has_offset ? $layout->{fields}{$field} : 0,
				});
				push(@offset_valid_values, {
					kernel_enum => $record->{kernel_enum},
					value => $has_offset ? 1 : 0,
				});
			}

			emit_c_offset_array($offset_array, "unsigned short", \@offset_values);
			emit_c_offset_array($offset_valid_array, "unsigned char", \@offset_valid_values);
			push(@bindings, "\truntime_structs->structs[" . $struct_idx . "].fields[" . $field_idx . "].kernel_offsets = " . $offset_array . ";\n");
			push(@bindings, "\truntime_structs->structs[" . $struct_idx . "].fields[" . $field_idx . "].kernel_offset_valid = " . $offset_valid_array . ";\n");
		}
	}

	print("static void associate_runtime_generated_struct_offsets(struct runtime_struct *runtime_structs){\n");
	print("\tif(runtime_structs == NULL){\n");
	print("\t\treturn;\n");
	print("\t}\n\n");
	foreach my $binding (@bindings){
		print($binding);
	}
	print("}\n");
}

sub ubuntu_kernel_version_supported {
	my ($pkg_name) = @_;
	my @version = ();

	return 0 unless defined($pkg_name);
	# search for the dbgsym kernel package
	return 0 unless $pkg_name =~ /(\d+)\.(\d+)\.(\d+)-(\d+)-generic-dbgsym_/;

	# parse the kernel version, for Exsul, all versions from kernel 5.15 are supported
	@version = (0 + $1, 0 + $2, 0 + $3, 0 + $4);
	for(my $i = 0; $i < @ubuntu_min_kernel_version; $i++){
		if($version[$i] > $ubuntu_min_kernel_version[$i]){
			return 1;
		}
		if($version[$i] < $ubuntu_min_kernel_version[$i]){
			return 0;
		}
	}
	return 1;
}

sub load_required_offsets {
	my ($path) = @_;
	my $data = undef;
	my $json_text = undef;
	my @structs = ();

	open(my $fh, "<", $path) or die "open $path: $!\n";
	local $/;
	$json_text = <$fh>;
	close($fh) or die "close $path: $!\n";

	$data = decode_json($json_text);
	die "$path does not contain a top-level structs array\n" unless ref($data->{structs}) eq "ARRAY";

	foreach my $entry (@{ $data->{structs} }){
		die "$path contains an invalid struct entry\n" unless ref($entry) eq "HASH";
		die "$path contains a struct entry without a name\n" unless defined($entry->{name}) && $entry->{name} =~ /\A[A-Za-z_][A-Za-z0-9_]*\z/;
		die "$path struct " . $entry->{name} . " has an invalid fields array\n" unless ref($entry->{fields}) eq "ARRAY";

		foreach my $field (@{ $entry->{fields} }){
			die "$path struct " . $entry->{name} . " contains an invalid field name\n" unless defined($field) && $field =~ /\A[A-Za-z_][A-Za-z0-9_]*\z/;
		}

		push @structs, {
			name => $entry->{name},
			fields => [ @{ $entry->{fields} } ],
		};
	}

	die "$path does not request any structs\n" unless @structs;
	return \@structs;
}

sub find_extracted_vmlinux {
	my ($root) = @_;
	my @candidates = grep { -f $_ } glob(File::Spec->catfile($root, $ubuntu_kernel_dbgsym_local_path, "vmlinux-*"));

	return undef unless @candidates;
	return $candidates[0];
}

sub find_fallback_module_path {
	my ($module_name) = @_;
	my $path = undef;
	my $paths = undef;

	$paths = `$find $ubuntu_kernel_pkg_folder -type f -name $module_name`;
	($path) = split(/\n/, $paths);
	if(!defined($path) || $path eq ""){
		$paths = `$find $ubuntu_kernel_pkg_folder -type f -name $module_name.zst`;
		($path) = split(/\n/, $paths);
	}
	return undef unless defined($path) && $path ne "";

	if($path =~ /\.zst\z/){
		my $decompressed_path = $path;
		$decompressed_path =~ s/\.zst\z//;
		system($zstd, "-df", "-q", $path, "-o", $decompressed_path);
		if($? != 0 || ! -f $decompressed_path){
			print("[!] failed to decompress fallback module " . $path . "\n");
			return undef;
		}
		return $decompressed_path;
	}

	return $path;
}

sub capture_fallback_pahole_struct {
	my ($struct_name, $module_name) = @_;

	$fallback_path = find_fallback_module_path($module_name);
	return 0 unless defined($fallback_path);

	($status, $output) = capture_pahole_struct($struct_name, $fallback_path);
	return $output =~ /\S/;
}

sub extract_offsets_from_vmlinux {
	my ($pkg_name, $vmlinux_path, $structs) = @_;

	print("[*] extracting offsets from " . $vmlinux_path . "\n");
	print("[*] package: " . $pkg_name . "\n");
	my $kernel_enum = ubuntu_kernel_enum_name($pkg_name);
	my %record = (
		kernel_enum => $kernel_enum,
		package => $pkg_name,
		structs => {},
	);

	return undef unless defined($kernel_enum);

	foreach my $struct (@$structs){
		($status, $output) = capture_pahole_struct($struct->{name}, $vmlinux_path);

		if($output !~ /\S/){
			# usually pahole fails for hci_xxx and ieee80211_xxx structs in vmlinux;
			# in this case, search the matching module, including compressed .ko.zst files.
			if($struct->{name} =~ "hci_" && capture_fallback_pahole_struct($struct->{name}, $bluetooth_mod_fallback)){
				goto fallback;
			}
			if($struct->{name} =~ "ieee80211_" && capture_fallback_pahole_struct($struct->{name}, $mac80211_mod_fallback)){
				goto fallback;
			}
			print("[!] struct " . $struct->{name} . " not found by pahole\n");
			next;
		}
		fallback:
		my ($struct_size, $offsets) = parse_pahole_output($output, $struct->{fields});
		$record{structs}{$struct->{name}} = {
			struct_size => $struct_size,
			fields => {},
		};
		print("struct " . $struct->{name} . " size=" . $struct_size . "\n");

		foreach my $field (@{ $struct->{fields} }){
			if(exists($offsets->{$field})){
				$record{structs}{$struct->{name}}->{fields}{$field} = $offsets->{$field};
				print("\t" . $field . "=" . $offsets->{$field} . "\n");
			}else{
				print("\t" . $field . "=missing\n");
			}
		}
	}

	return \%record;
}

sub capture_pahole_struct {
	my ($struct_name, $vmlinux_path) = @_;
	my $output = "";
	my $status = 0;
	my $pid = open(my $fh, "-|");

	die "failed to fork pahole: $!\n" unless defined($pid);
	if($pid == 0){
		open(STDERR, ">", File::Spec->devnull()) or exit(127);
		exec($pahole, "-C", $struct_name, $vmlinux_path) or exit(127);
	}

	{
		local $/;
		$output = <$fh>;
	}
	$output = "" unless defined($output);
	close($fh);
	$status = $? >> 8;
	return ($status, $output);
}

sub parse_pahole_output {
	my ($output, $fields) = @_;
	my %requested = map { $_ => 1 } @$fields;
	my %offsets = ();
	my $struct_size = 0;

	foreach my $line (split(/\n/, $output)){
		if($line =~ /\/\*\s*size:\s*(\d+),/){
			$struct_size = 0 + $1;
			next;
		}

		next unless $line =~ /\/\*\s*(\d+)(?::\s*\d+)?\s+\d+\s*\*\//;
		my $offset = 0 + $1;
		my ($decl) = split(/\/\*/, $line, 2);
		my $field_name = declared_field_name($decl);

		next unless defined($field_name);
		next unless $requested{$field_name};
		$offsets{$field_name} = $offset unless exists($offsets{$field_name});
	}

	return ($struct_size, \%offsets);
}

sub declared_field_name {
	my ($decl) = @_;

	return undef unless defined($decl);
	$decl =~ s/\A\s+|\s+\z//g;
	return undef if $decl eq "";
	return undef if $decl =~ /\{\s*\z/;

	if($decl =~ /\(\s*\*\s*([A-Za-z_][A-Za-z0-9_]*)\s*\)/){
		return $1;
	}

	$decl =~ s/\s+__attribute__\s*\(\(.*\)\)//g;
	$decl =~ s/\s+__aligned(?:__)?\s*\([^)]*\)//g;
	$decl =~ s/\s+__packed(?:__)?\b//g;
	$decl =~ s/\[[^\]]*\]//g;
	$decl =~ s/:\s*\d+\s*//g;
	$decl =~ s/[;=].*\z//;
	$decl =~ s/\A\s+|\s+\z//g;

	return $1 if $decl =~ /([A-Za-z_][A-Za-z0-9_]*)\s*\z/;
	return undef;
}
