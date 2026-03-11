#!/usr/bin/perl

# Made by Edoardo Mantovani, 2026
# Rewritten with safer local alias inference and guarded field replacement.
# version 1.0.2 (11 March 2026)

use strict;
use warnings;
use Getopt::Long;
use Cwd qw(getcwd);
use File::Copy qw(move);
use List::Util qw(uniq);
use Term::ANSIColor qw(color);
use File::Temp qw(tempfile tempdir);
use File::Basename qw(dirname basename);

my $ls   = `which ls`;
my $cd   = `which cd`;
my $cp   = `which cp`;
my $rm   = `which rm`;
my $sed  = `which sed`;
my $grep = `which grep`;
my $find = `which find`;
my $make = `which make`;

my $arch    = "./arch";
my $source  = "src";
my $include = "inc";
my $stubs   = "stubs";
my $build   = "build";
my $bin     = "./bin";
my $example = "./example";

my $final_program = "translator";

my $main      = "main.c";
my $win_layer = "layer.h";
my $proto_gen = "prototypes.h";
my $cb_table  = "cb_table.h";

my $functions = "functions";
my $variables = "variables";

my $function_proto_regex = q{^\h*[_A-Za-z]\w*(?:[\w\h\*]*\h+)+[_A-Za-z]\w*\h*\([^;{}]*\)\h*(?=\{)};
my $find_command_arg = q{-type f -not -path "out_*" -name "*.c" -or -name "*.h"};

my $final_obj_line = undef;
my $proto = undef;
my $current_stub = undef;
my $final_stub = undef;
my $stub_name = undef;
my $cb_header = undef;
my $offsets = undef;
my $cb_counter = 0;
my $proto_header = undef;
my $iterator = undef;
my $word_processor = undef;
my $base_folder = undef;
my $offset_generation_file = undef;
my $virtual_compilation_folder = undef;

my $compiler = undef;
my $generate_offset_file = undef;
my $pahole_kernel = undef;
my $windows_support = undef;
my $make_examples = undef;
my $verbose = undef;
my $clean = undef;
my $help = undef;

my $modified = 0;
my $process_step = 0;
my $struct_field_counter = 0;
my $struct_name_counter = 0;
my $total_struct_number = 0;
my $current_struct_field_num = 0;

my @sources = ();
my @crafted_sources = ();
my @crafted_source_lines = ();
my @struct_fields = ();
my @runtime_structs = ();
my %struct_idx_by_name = ();
my %field_idx_by_name = ();
my %struct_set = ();
my %used_fields_by_struct = ();

my @gcc_params = (
	"-DENABLE_DBG_PRINT",
	"-DCONFIG_PM",
	"-O2",
	"-w",
	"-I$arch",
	"-march=native",
	"-mtune=native",
	"-mstackrealign",
	"-mincoming-stack-boundary=3",
);
my @gcc_stub_params = (
	"-mno-sse",
	"-mno-mmx",
	"-mno-sse2",
	"-mno-3dnow",
	"-mno-avx",
	"-mno-sse4a",
	"-mno-red-zone",
	" -mpreferred-stack-boundary=3",
	"-fno-omit-frame-pointer"
);
my @gcc_final_params = (
	"-fno-pie",
	"-no-pie",
	"-rdynamic"
);
my @gcc_final_linkkings = (
	"-lusb-1.0",
	"-lpthread",
	"-ldwarf",
);

my %struct_whitelist = (
	"skb"		=> "sk_buff",
	"mod"		=> "module",
	"wiphy"		=> "wiphy",
	"sock"		=> "sock",
	"dev"		=> "device",
	"udev"		=> "usb_device",
        "rate_info"     => "rate_info",
	"hw"		=> "ieee80211_hw",
	"urb"		=> "urb",
	"fops"		=> "file_operations",
	"netdev"	=> "net_device",
);

my %field_ctype = (
	module => {
		"mem"			=> "struct module_memory",
		"crcs"			=> "u32 *",
		"gpl_crcs"		=> "u32 *",
		"num_gpl_syms"		=> "unsigned int",
		"num_exentries"		=> "unsigned int",
		"args"			=> "char *",
		"ctors"			=> "void *",
		"exit"			=> "void *",
		"name"			=> "char",
		"build_id"		=> "char",
		"sig_ok"		=> "bool",
		"status"		=> "unsigned int",
		"aborted"		=> "bool",
		"kp"			=> "struct kernel_param *",
		"num_kp"		=> "unsigned int",
		"syms"			=> "struct kernel_symbol *",
		"kallsyms"		=> "struct mod_kallsyms *",
		"core_kallsyms"		=> "struct mod_kallsyms",
	},
	sk_buff => {
		"len"                   => "unsigned int",
		"data_len"              => "unsigned int",
		"mac_len"               => "__u16",
		"hdr_len"               => "__u16",
		"next"                  => "struct sk_buff *",
		"prev"                  => "struct sk_buff *",
		"tail"                  => "sk_buff_data_t",
		"end"                   => "sk_buff_data_t",
		"head"                  => "unsigned char *",
		"data"                  => "unsigned char *",
		"cb"			=> "char",
		"queue_mapping"		=> "u16",
		"pkt_type"		=> "u8",
		"unreadable"            => "u8",
		"csum"			=> "__wsum",
		"hash"			=> "u32",
		"protocol"		=> "be16",
		"inner_protocol"        => "be16",
		"inner_protocol_type"   => "u8",
		"inner_ipproto"         => "u8",
		"inner_mac_header"      => "u16",
		"inner_network_header"  => "u16",
		"inner_transport_header"=> "u16",
		"transport_header"      => "u16",
		"network_header"        => "u16",
		"mac_header"            => "u16",
		"dev"                   => "void *",
		"queue_mapping"         => "unsigned int",
		"reserved_tailroom"     => "unsigned int",
		"destructor"            => "void *",
	},
	sk_buff_head => {
		"next"			=> "struct sk_buff *",
		"prev"			=> "struct sk_buff *",
		"qlen"			=> "unsigned int",
	},
	skb_shared_info => {
		"flags"			=> "unsigned char",
		"frag_list"		=> "struct sk_buff *",
		"destructor_arg"	=> "void *",
	},
	urb => {
		"kref"			=> "struct kref",
		"urb_list"		=> "struct list_head",
		"anchor_list"		=> "struct list_head",
		"ep"			=> "struct usb_host_endpoint *",
		"pipe"			=> "unsigned int",
		"transfer_flags"	=> "unsigned int",
		"transfer_buffer"	=> "void *",
		"anchor"		=> "struct usb_anchor *",
		"context"		=> "void *",
	},
	usb_device => {
		"ep0"			=> "struct usb_host_endpoint",
		"descriptor"		=> "struct usb_device_descriptor",
		"bos"			=> "struct usb_host_bos *",
		"config"		=> "struct usb_host_config *",
		"actconfig"		=> "struct usb_host_config *",
		"dev"			=> "void *",
		"kobj"			=> "void *",
	},
	wiphy => {
		"flags"			=> "u32",
		"regulatory_flags"	=> "u32",
		"features"		=> "u32",
		"ext_features"		=> "u8",
		"priv"			=> "char",
	},
	net_device => {
		"name"			=> "char",
		"mem_end"		=> "unsigned long",
		"mem_start"		=> "unsigned long",
		"base_addr"		=> "unsigned long",
		"irq"			=> "int",
		"dev_list"		=> "struct list_head",
		"napi_list"		=> "struct list_head",
		"unreg_list"		=> "struct list_head",
		"ptype_all"		=> "struct list_head",
		"ptype_specific"	=> "struct list_head",
	},
);

chomp($ls);
chomp($cd);
chomp($cp);
chomp($rm);
chomp($sed);
chomp($grep);
chomp($find);
chomp($make);

GetOptions(
	"compiler=s"      => \$compiler,
	"generate_offset" => \$generate_offset_file,
	"pahole_kernel=s" => \$pahole_kernel,
	"windows"         => \$windows_support,
	"examples"        => \$make_examples,
	"verbose"	  => \$verbose,
	"clean"           => \$clean,
	"help"            => \$help,
);

if($help){
	print("Build script for ...\n");
	print("Made by Edoardo Mantovani, 2026\n");
	print("options:\n");
	print("compiler=[gcc|clang|tcc] (only working for now: gcc)\n");
	print("generate_offset -> generate a special json file which can be used for creating the offset backup plan\n");
	print("windows -> enable windows compilation\n");
	print("examples -> compile every kernel object example under the ./example folder\n");
	print("verbose -> show compilation command line\n");
	print("clean -> clear every compilation file\n");
	print("help -> display this help text\n");
	exit(0);
}

if($generate_offset_file){
	$offset_generation_file = "required_offsets.json";
}

if($clean){
	`$rm $build/*.o 2>/dev/null`;
	`$rm $bin/$final_program 2>/dev/null`;
	`$rm -rf out_* 2>/dev/null`;

	if($offset_generation_file){
		`$rm $offset_generation_file 2>/dev/null`;
	}else{
		`$rm required_offsets.json 2>/dev/null`;
	}
	chdir($example);
	`for d in */; do (cd "\$d" && make clean); done`;
	exit(0);
}

if($make_examples){
	chdir($example);
	`for d in */; do (cd "\$d" && make); done`;
	exit(0);
}

if($windows_support){
	# reserved for future work
}

if(!$compiler){
	$compiler = "gcc";
}

sub get_struct_index {
	my ($struct_name) = @_;
	return $struct_idx_by_name{$struct_name};
}

sub get_field_index {
	my ($struct_name, $field_name) = @_;
	return undef if !exists $field_idx_by_name{$struct_name};
	return $field_idx_by_name{$struct_name}{$field_name};
}

sub get_extended_ptr_variables {
	my ($line, $struct) = @_;
	my @vars = ();

	while ($line =~ /\b(?:const\s+)?struct\s+\Q$struct\E\b[^;{]*?\*+\s*([A-Za-z_]\w*)\s*(?=[,;\)=])/g) {
		push(@vars, $1);
	}

	if ($line =~ /\b(?:const\s+)?struct\s+\Q$struct\E\b([^;{]*)/) {
		my $rest = $1;
		while ($rest =~ /,\s*\*+\s*([A-Za-z_]\w*)\s*(?=[,;\)=])/g) {
			push(@vars, $1);
		}
	}

	return uniq(@vars);
}

sub get_type_from_struct_field {
	my ($struct, $field) = @_;
	return $field_ctype{$struct}{$field} if exists $field_ctype{$struct} && exists $field_ctype{$struct}{$field};
	return "unsigned int";
}

sub build_fixed_alias_map {
	my %fixed_alias_to_struct = ();
	while (my ($var, $st) = each %struct_whitelist) {
		$fixed_alias_to_struct{$var} = $st;
	}
	return %fixed_alias_to_struct;
}

sub harvest_declared_ptr_aliases {
	my ($line) = @_;
	my %found = ();

	foreach my $struct (keys %struct_set) {
		my @vars = get_extended_ptr_variables($line, $struct);
		foreach my $v (@vars) {
			$found{$v} = $struct;
		}
	}

	return %found;
}

sub collect_used_fields {
	my (@srcs) = @_;
	my %fixed_alias = build_fixed_alias_map();
	my %used = ();

	foreach my $src (@srcs) {
		open(my $fh, '<', $src) or die $!;
		my %local_alias = %fixed_alias;

		while (my $line = <$fh>) {
			my %new_aliases = harvest_declared_ptr_aliases($line);
			@local_alias{keys %new_aliases} = values %new_aliases if %new_aliases;

			while ($line =~ /\b([A-Za-z_]\w*)->([A-Za-z_]\w*)/g) {
				my ($var, $field) = ($1, $2);
				my $st = $local_alias{$var};
				next if !defined $st;
				$used{$st}{$field} = 1;
			}
		}

		close($fh) or die $!;
	}

	return %used;
}

sub emit_runtime_struct_header {
	my ($offsets_path, $offset_json_path) = @_;

	open(OFFSETS, '>', $offsets_path) or die $!;
	print OFFSETS "struct struct_field {\n";
	print OFFSETS "	unsigned char  field_name[24];\n";
	print OFFSETS "	unsigned short field_offset;\n";
	print OFFSETS "};\n";
	print OFFSETS "struct runtime_struct_layout {\n";
	print OFFSETS "	short struct_size;\n";
	print OFFSETS "	unsigned char *struct_name;\n";
	print OFFSETS "	unsigned short struct_field_num;\n";
	print OFFSETS "struct struct_field fields[64];\n";
	print OFFSETS "};\n";
	print OFFSETS "struct runtime_struct {\n";
	print OFFSETS "	int compilation_time_generated_structs;\n";
	print OFFSETS "struct runtime_struct_layout structs[$total_struct_number];\n";
	print OFFSETS "};\n";
	print OFFSETS "OFFSET_TABLE_SECTION static struct runtime_struct runtime_generated_structs = {\n";
	print OFFSETS "	.structs = {\n";

	if($generate_offset_file){
		open(OFFSET_GEN, '>', $offset_json_path) or die $!;
		print OFFSET_GEN "{\n";
		print OFFSET_GEN "\"structs\": [\n";
	}

	my @tracked_structs = sort keys %used_fields_by_struct;
	$struct_name_counter = 0;
	%struct_idx_by_name = ();
	%field_idx_by_name = ();
	@runtime_structs = ();

	for (my $i = 0; $i < scalar(@tracked_structs); $i++) {
		my $struct_name = $tracked_structs[$i];
		my @fields = sort keys %{ $used_fields_by_struct{$struct_name} };
		$current_struct_field_num = scalar(@fields);
		$struct_idx_by_name{$struct_name} = $struct_name_counter;

		my $current_struct = {
			name    => $struct_name,
			fields  => [],
			by_name => {},
		};

		print OFFSETS "\t\t\t[$struct_name_counter] = {\n";
		print OFFSETS "\t\t\t\t.struct_size = 0,\n";
		print OFFSETS "\t\t\t\t.struct_name = \"$struct_name\",\n";
		print OFFSETS "\t\t\t\t.struct_field_num = $current_struct_field_num,\n";

		if (@fields) {
			print OFFSETS "\t\t\t\t.fields = {\n";
			$struct_field_counter = 0;

			foreach my $field (@fields) {
				$field_idx_by_name{$struct_name}{$field} = $struct_field_counter;
				$current_struct->{by_name}{$field} = $struct_field_counter;
				push(@{ $current_struct->{fields} }, { offset => 0, field_name => $field });

				print OFFSETS "\t\t\t\t\t[$struct_field_counter] = {\n";
				print OFFSETS "\t\t\t\t\t\t.field_name = \"$field\",\n";
				print OFFSETS "\t\t\t\t\t\t.field_offset = 0,\n";
				print OFFSETS "\t\t\t\t\t},\n";
				$struct_field_counter++;
			}

			print OFFSETS "\t\t\t\t},\n";
		}

		print OFFSETS "\t\t\t},\n";
		push(@runtime_structs, $current_struct);

		if($generate_offset_file){
			print OFFSET_GEN "\t\t{\n";
			print OFFSET_GEN "\t\t\t\"name\":\"$struct_name\",\n";
			print OFFSET_GEN "\t\t\t\"fields\": [";
			for (my $j = 0; $j < scalar(@fields); $j++) {
				print OFFSET_GEN "\"$fields[$j]\"";
				print OFFSET_GEN ", " if $j < $#fields;
			}
			print OFFSET_GEN "]\n";
			print OFFSET_GEN "\t\t}";
			print OFFSET_GEN "," if $i < $#tracked_structs;
			print OFFSET_GEN "\n";
		}

		$struct_name_counter++;
	}

	print OFFSETS "\t\t},\n";
	print OFFSETS "};\n";
	close(OFFSETS) or die $!;

	if($generate_offset_file){
		print OFFSET_GEN "\t]\n";
		print OFFSET_GEN "}\n";
		close(OFFSET_GEN) or die $!;
	}
}

sub rewrite_source_file {
	my ($in_path) = @_;
	my %fixed_alias = build_fixed_alias_map();

	open(my $in, '<', $in_path) or die $!;

	my $dir = dirname($in_path);
	my ($out, $tmp_path) = tempfile(".tmp_XXXX", DIR => $dir, UNLINK => 0);
	my %local_alias = %fixed_alias;

	while(my $line = <$in>){
		my %new_aliases = harvest_declared_ptr_aliases($line);
		@local_alias{keys %new_aliases} = values %new_aliases if %new_aliases;

#		$line =~ s{
#			\b([A-Za-z_]\w*)->([A-Za-z_]\w*)
#		}{
#			print("$line\n");
#			my ($var, $field) = ($1, $2);
#			my $st = $local_alias{$var};
#
#			if (!defined $st) {
#				"$var->$field";
#			}
#			else {
#				my $current_struct_idx = get_struct_index($st);
#				my $current_field_idx  = get_field_index($st, $field);
#
#				if (!defined $current_struct_idx || !defined $current_field_idx) {
#					print("[!] skipping unresolved access $var->$field (struct $st)\n");
#					"$var->$field";
#				}
#				else {
#					print("[*] $st $field\n");
#					my $current_field_type = get_type_from_struct_field($st, $field);
#					"(*($current_field_type*)((char *)$var + director.runtime_generated_structs->structs[$current_struct_idx].fields[$current_field_idx].field_offset))";
#				}
#			}
#		}gex;

		if (%local_alias) {
        		my $var_re = join '|', map { quotemeta($_) } sort { length($b) <=> length($a) } keys %local_alias;

			$line =~ s{\b($var_re)->([A-Za-z_]\w*)}{
				my ($var, $field) = ($1, $2);
				my $st = $local_alias{$var};

				if (!(defined $st)) {
					"$var->$field";
				}else{
					my $current_struct_idx = get_struct_index($st);
					my $current_field_idx  = get_field_index($st, $field);

					if (!defined $current_struct_idx || !defined $current_field_idx) {
						print("[!] skipping unresolved access $var->$field (struct $st)\n");
						"$var->$field";
					}else{
						my $current_field_type = get_type_from_struct_field($st, $field);
						"(*($current_field_type*)((char *)$var + director.runtime_generated_structs->structs[$current_struct_idx].fields[$current_field_idx].field_offset))";
					}
				}
			}gex;
		}
		if($line =~ /\]\.base\b/){
			$line =~ s/\]\.base\b/]+ 0/g;
		}
		if($line =~ /\]\.size\b/){
			$line =~ s/\]\.size\b/] + director.runtime_single_fields.module_mem_size_offset/g;
		}
		if($line =~ /\[type\]/){
			$line =~ s/\[type\]/ + \(type * director.runtime_generated_structs->structs[0].struct_size\)/g;
		}

		print $out $line;
	}

	close($out) or die $!;
	close($in)  or die $!;
	move($tmp_path, $in_path) or die $!;
}

# fill the '\@sources' array
foreach(`$find $find_command_arg | $grep -iv out_`){
	$iterator = $_;
	chomp($iterator);
	push(@sources, $iterator);
}

# set of structs recognised from declarations
%struct_set = map { $_ => 1 } values %struct_whitelist;
$struct_set{"sk_buff_head"}    = 1;
$struct_set{"skb_shared_info"} = 1;
$struct_set{"ieee80211_ops"}   = 1;

# first pass: gather field usage using local alias inference
%used_fields_by_struct = collect_used_fields(@sources);
$total_struct_number = scalar(keys %used_fields_by_struct);

$offsets = "$include/struct_offset.h";
emit_runtime_struct_header($offsets, $offset_generation_file);

# second iteration:
# 1) process header files under "./inc/"
# 2) process stubs
# 3) process "src/main.c"
$process_step++;

$virtual_compilation_folder = tempdir("out_XXXX", CLEANUP => 0);

foreach(qw(inc stubs src)){
	`$cp -R $_ $virtual_compilation_folder/$_`;
}

@sources = ();
foreach(`$find $virtual_compilation_folder`){
	$iterator = $_;
	chomp($iterator);
	if($iterator =~ /\.(c|h)\z/){
		push(@sources, $iterator);
	}
}

foreach my $src (uniq(@sources)){
	rewrite_source_file($src);
}

$cb_header = "$include/$cb_table";
open(FH, '>', $cb_header) or die $!;
print FH "/* Copyright (C) Edoardo Mantovani - All Rights Reserved\n";
print FH "* Unauthorized copying of this file, via any medium is strictly prohibited\n";
print FH "* Proprietary and confidential\n";
print FH "*/\n\n";
print FH "/** automatically generated file, don't edit! **/\n\n";
print FH "struct internal_cb_table sym_table[] = {\n";

$proto_header = "$include/$proto_gen";
open(PROTO, '>', $proto_header) or die $!;
print PROTO "/* Copyright (C) Edoardo Mantovani - All Rights Reserved\n";
print PROTO "* Unauthorized copying of this file, via any medium is strictly prohibited\n";
print PROTO "* Proprietary and confidential\n";
print PROTO "*/\n\n";
print PROTO "/** automatically generated file, don't edit! **/\n\n";

$find_command_arg = q{-name "*.c"};
foreach(`$find $virtual_compilation_folder/$stubs/$functions/ $find_command_arg`){
	$current_stub = $_;
	chomp($current_stub);
	$final_stub = $current_stub;
	$final_stub =~ s/\..*$/.o/;
	$stub_name = $final_stub;
	$stub_name =~ s/\..*$//;
	$iterator = (((split("/", $stub_name))[-1]));
	print FH "    [$cb_counter] = { .function_name = \"$iterator\", .function_ptr = $iterator },\n";
	$cb_counter++;
	$proto = `grep -oP '$function_proto_regex' $current_stub`;
	chomp($proto);
	if(length($proto) == 0){
		$proto = `cat $current_stub | head -n 1`;
		chomp($proto);
		$proto =~ s/{//;
	}
	print PROTO "$proto;\n";
}

foreach(`$find $virtual_compilation_folder/$stubs/$variables/ $find_command_arg`){
	$current_stub = $_;
	chomp($current_stub);
	$final_stub = $current_stub;
	$final_stub =~ s/\..*$/.o/;
	$stub_name = $final_stub;
	$stub_name =~ s/\..*$//;
	$iterator = (((split("/", $stub_name))[-1]));
	print FH "    [$cb_counter] = { .function_name = \"$iterator\", .function_ptr = &$iterator },\n";
	$cb_counter++;
	$proto = `cat $current_stub | head -n 1`;
	chomp($proto);
	$proto =~ s/=//;
	$proto =~ s/{//;
	print PROTO "extern $proto;\n";
}

print FH "    [$cb_counter] = { .function_name = NULL, .function_ptr = NULL },\n";
print FH "};\n\n";
close(FH) or die $!;
close(PROTO) or die $!;

`$cp $proto_header $virtual_compilation_folder/$include`;
`$cp $cb_header $virtual_compilation_folder/$include`;

foreach(`$ls $virtual_compilation_folder/$stubs/$functions`){
	$current_stub = $_;
	chomp($current_stub);
	$final_stub = $current_stub;
	$final_stub =~ s/\..*$/.o/;
	if($verbose){
		print("$compiler @gcc_params -I./$virtual_compilation_folder/$include @gcc_stub_params -include $virtual_compilation_folder/$include/$win_layer -c $virtual_compilation_folder/$stubs/$functions/$current_stub -o $build/$final_stub\n");
	}
	system("$compiler @gcc_params -I./$virtual_compilation_folder/$include @gcc_stub_params -include $virtual_compilation_folder/$include/$win_layer -c $virtual_compilation_folder/$stubs/$functions/$current_stub -o $build/$final_stub");
	if(!$verbose){
		print("[*] compiling $final_stub\n");
	}
}

foreach(`$ls $virtual_compilation_folder/$stubs/$variables`){
	$current_stub = $_;
	chomp($current_stub);
	$final_stub = $current_stub;
	$final_stub =~ s/\..*$/.o/;
	$stub_name = $final_stub;
	$stub_name =~ s/\..*$//;
	if($verbose){
		print("$compiler @gcc_params -I./$virtual_compilation_folder/$include -include $virtual_compilation_folder/$include/$win_layer -c $virtual_compilation_folder/$stubs/$variables/$current_stub -o $build/$final_stub\n");
	}
	system("$compiler @gcc_params -I./$virtual_compilation_folder/$include -include $virtual_compilation_folder/$include/$win_layer -c $virtual_compilation_folder/$stubs/$variables/$current_stub -o $build/$final_stub");
	if(!$verbose){
		print("[*] compiling $final_stub\n");
	}
}

foreach(`$ls $build`){
	$current_stub = $_;
	chomp($current_stub);
	$final_obj_line .= "$build/$current_stub ";
}

print(color("red"));
print("[*] compiling $final_program\n");
print(color("reset"));
if($verbose){
	print("$compiler @gcc_params @gcc_final_params -I./$virtual_compilation_folder/$include $virtual_compilation_folder/$source/$main -o $bin/$final_program $final_obj_line @gcc_final_linkkings");
	print("\n\n");
}
system("$compiler @gcc_params @gcc_final_params -I./$virtual_compilation_folder/$include $source/$main -o $bin/$final_program $final_obj_line @gcc_final_linkkings");

undef(@struct_fields);
undef(@gcc_params);
undef(@gcc_final_params);
undef(@gcc_final_linkkings);
