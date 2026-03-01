#!/usr/bin/perl

# Made by Edoardo Mantovani, 2026

use strict;
use warnings;
use Getopt::Long;
use Cwd qw(getcwd);
use List::Util qw(uniq);
use File::Temp qw(tempfile tempdir);
use Term::ANSIColor qw(color);

my $ls = `which ls`;
my $cd = `which cd`;
my $rm = `which rm`;
my $sed = `which sed`;
my $grep = `which grep`;
my $find = `which find`;
my $make = `which make`;

my $arch   = "./arch";
my $source = "./src";
my $include = "./inc";
my $stubs = "./stubs";
my $build = "./build";
my $bin   = "./bin";
my $example = "./example";

my $final_program = "translator";

my $main      = "main1.c";
my $win_layer = "layer.h";
my $proto_gen = "prototypes.h";
my $cb_table  = "cb_table.h";

my $functions = "functions";
my $variables = "variables";

my $function_proto_regex = q{^\h*[_A-Za-z]\w*(?:[\w\h\*]*\h+)+[_A-Za-z]\w*\h*\([^;{}]*\)\h*(?=\{)};
my $find_command_arg = q{\( -path "*/inc*" -name "*.h" \)};

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
my @gcc_params = (
	"-DENABLE_DBG_PRINT",
	"-O2",
	"-w",
	"-I$include",
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
	"skb" => "sk_buff",
	"newsk" => "sk_buff",
	"mod" => "module",
	"wiphy" => "wiphy",
	"udev" => "usb_device",
	"hw" => "ieee80211_hw",
	"urb" => "urb",
	"rate" => "rate_info"
);

chomp($ls);
chomp($cd);
chomp($rm);
chomp($sed);
chomp($grep);
chomp($find);
chomp($make);

GetOptions(
	"compiler=s"		=> \$compiler,
	"generate_offset"	=> \$generate_offset_file,
	"pahole_kernel=s"	=> \$pahole_kernel,
	"windows"		=> \$windows_support,
	"examples"		=> \$make_examples,
	"clean"			=> \$clean,
	"help"			=> \$help,
);

if($help){
	print("Build script for ...\n");
	print("Made by Edoardo Mantovani, 2026\n");
	print("options:\n");
	print("	compiler=[gcc|clang|tcc] (only working for now: gcc)\n");
	print("	generate_offset -> generate a special json file which can be used for creating the offset backup plan\n");
	print("	windows -> enable windows compilation\n");
	print("	examples -> compile every kernel object example under the ./example folder\n");
	print("	clean -> clear every compilation file\n");
	print("	help -> display this help text\n");
	exit(0);
}

if($clean){
	# delete every compiled file and quit
	`$rm $build/*.o`;
	`$rm $bin/$final_program`;

	chdir($example);
	# taken from 'https://askubuntu.com/questions/700933/make-clean-in-several-subdirectories'
	`for d in */; do (cd "\$d" && make clean); done`;

	exit(0);
}

if($generate_offset_file){
	$offset_generation_file = "required_offsets.json";
}

if($make_examples){
	# compile every example
	chdir($example);
	# taken from stackoverflow
	`for d in */; do (cd "\$d" && make); done`;
	exit(0);
}

if($windows_support){

}

if(!$compiler){
	# set "gcc" as the default compiler
	$compiler = "gcc";
}

# "get_struct_index" helper function: given a struct name, returns it's index inside the generated header file
sub get_struct_index {
	my ($struct_name) = @_;
	return $struct_idx_by_name{$struct_name};
}
# "get_field_index" helper function: given a field name, returns it's index inside the generated header file
sub get_field_index {
	my ($struct_name, $field_name) = @_;
	return $field_idx_by_name{$struct_name}{$field_name};
}

# fill the '@sources' array
foreach(`$find $find_command_arg`){
	$iterator = $_;
	chomp($iterator);
	push(@sources, $iterator);
}

$total_struct_number = keys %struct_whitelist;

$offsets = "$include/struct_offset.h";

open(OFFSETS, '>', $offsets) or die $!;
print OFFSETS "struct struct_field {\n";
print OFFSETS "	unsigned char  field_name[24];\n";
print OFFSETS "	unsigned short field_offset;\n";
print OFFSETS "};\n";

print OFFSETS "struct runtime_struct_layout {\n";
print OFFSETS "	short struct_size;\n";
print OFFSETS "	unsigned char *struct_name;\n";
print OFFSETS "	unsigned short struct_field_num;\n";
print OFFSETS"	struct struct_field fields[64];\n";
print OFFSETS"};\n";

print OFFSETS "struct runtime_struct {\n";
print OFFSETS "	int compilation_time_generated_structs;\n";
print OFFSETS "	struct runtime_struct_layout structs[$total_struct_number];\n";
print OFFSETS"};\n";

print OFFSETS "OFFSET_TABLE_SECTION static struct runtime_struct runtime_generated_structs = {\n";
print OFFSETS "	.structs = {\n";

if($generate_offset_file){
	open(OFFSET_GEN, '>', $offset_generation_file) or die $!;
        print OFFSET_GEN "{\n";
        print OFFSET_GEN "	\"structs\": [\n";
}

# first iteration: create the header
foreach(keys %struct_whitelist){
	$iterator = $_;
	foreach(@sources){
		open my $source, $_ or die $!;
		while(my $line = <$source>){
			if($line =~ "$iterator->"){
				$word_processor = $line;
				#$line =~ s/$iterator->/(char *)$iterator + director.$struct_whitelist{$iterator}./;
				$line =~ s/\b\Q$iterator\E->/(char *)$iterator + director.$struct_whitelist{$iterator}./;
				if($word_processor =~ /$iterator->(\w+)/){
					push(@struct_fields, $1);
				}
			}
			last if eof($source);
		}
		close($source) or die $!;
	}
	$current_struct_field_num = uniq(@struct_fields);
	my $struct_name = $struct_whitelist{$iterator};
	$struct_idx_by_name{$struct_name} = $struct_name_counter;
	# very dirty solution: create the struct on the fly while generating the header
	my $current_struct = {
		name    => $struct_name,
		fields  => [],
		by_name => {},
	};
	print OFFSETS "			[$struct_name_counter] = {\n";
	print OFFSETS "				.struct_size = 0,\n";
	print OFFSETS "				.struct_name =  \"$struct_whitelist{$iterator}\",\n";
	print OFFSETS "				.struct_field_num = $current_struct_field_num,\n";
	if($#struct_fields != -1){
		print OFFSETS "				.fields = {\n";
		foreach(uniq(@struct_fields)){
			$field_idx_by_name{$struct_name}{$_} = $struct_field_counter;
			$current_struct->{by_name}{$_} = $struct_field_counter;
			push(@{ $current_struct->{fields} }, { offset => 0, field_name => $_ });
			print OFFSETS "					[$struct_field_counter] = {\n";
			print OFFSETS "						.field_name = \"$_\",\n";
			print OFFSETS "						.field_offset = 0,\n";
			print OFFSETS "					},\n";
			$struct_field_counter++;
		}
		# update: add also a custom "offset_to_search.json" file for the "offset_generator"
		if($generate_offset_file){
			print OFFSET_GEN "		{\n";
			print OFFSET_GEN "			\"name\":\"$struct_whitelist{$iterator}\",\n";
			print OFFSET_GEN "			\"fields\": [";
			my @uniques = uniq(@struct_fields);
			foreach(@uniques){
				print OFFSET_GEN "\"$_\"";
				if(!($_ eq $uniques[-1])){
					print OFFSET_GEN ", ";
				} 
			}
			print OFFSET_GEN "]\n";
			print OFFSET_GEN "		},\n";
		}
		$field_idx_by_name{$struct_name}{"NULL"} = $struct_field_counter;
		$current_struct->{by_name}{"NULL"} = $struct_field_counter;
		push(@{ $current_struct->{fields} }, { offset => 0, field_name => "NULL" });

		# add a last NULL entry for the parsing phase
		$struct_field_counter++;
		print OFFSETS "					[$struct_field_counter] = {\n";
		print OFFSETS "						.field_name = \"NULL\",\n";
		print OFFSETS "						.field_offset = 0,\n";
		print OFFSETS "					},\n";
		print OFFSETS"				},\n";
	}
	print OFFSETS "        		},\n";

	push(@runtime_structs, $current_struct);

	$struct_name_counter++;
	$struct_field_counter = 0;
	@struct_fields = ()
}
print OFFSETS "		},\n";
print OFFSETS "};\n";

if($generate_offset_file){
	# add a last NULL entry
	print OFFSET_GEN "		{\n";
	print OFFSET_GEN "			\"name\":\"NULL\",\n";
	print OFFSET_GEN "			\"fields\": []\n";
	print OFFSET_GEN "		}\n";
	print OFFSET_GEN "	]\n";
	print OFFSET_GEN "}\n";
	close(OFFSET_GEN) or die $!;
}

close(OFFSETS) or die $!;

# second iteration:
# 1) process header files under "./inc/"
# 2) process stubs
# 3) process "src/main.c"

$process_step++;

# create a temp directory where we will store every file

$virtual_compilation_folder = tempdir("out_XXXX", CLEANUP => 0);

foreach(uniq(@sources)){
	open my $source, $_ or die $!;
	while(my $line = <$source>){
		foreach(keys %struct_whitelist){
			$iterator = $_;
			if($line =~ "$iterator->"){
				my $current_struct_idx = 0;
				my $current_field_idx = 0;
				$current_struct_idx = get_struct_index($struct_whitelist{$iterator});
				if($line =~ /$iterator->(\w+)/){
					$current_field_idx = get_field_index($struct_whitelist{$iterator}, $1);
				}
				$line =~ s/$iterator->(\w+)/((char *)$iterator + director.runtime_generated_structs->structs[$current_struct_idx].fields[$current_field_idx].field_offset)/g;
				$modified = 1;
				print($line);
			}
			if($line =~ "].base"){
				$line =~ s/].base/]+ 0/;
				$modified = 1;
			}
			if($line =~ "].size"){
				$line =~ s/].size/] + director.runtime_single_fields.module_mem_size_offset/;
				$modified = 1;
			}
			if($line =~ "[type]"){
				$line =~ s/\[type\]/ + (type * director.runtime_generated_structs->structs[0].struct_size)/;
				$modified = 1;
			}
		}
		if($modified == 1){
			$modified = 0;
		}
		last if eof($source);
	}

	@crafted_source_lines = ();
	close($source) or die $!;
}

sleep(10);

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

foreach(`$ls $stubs/$functions`){
	$current_stub = $_;
	chomp($current_stub);
	$final_stub = $current_stub;
	$final_stub =~ s/\..*$/.o/;
	$stub_name = $final_stub;
	$stub_name =~ s/\..*$//;
	print FH "    [$cb_counter] = { .function_name = \"$stub_name\", .function_ptr = $stub_name },\n";
	$cb_counter++;
	$proto = `grep -oP '$function_proto_regex' $stubs/$functions/$current_stub`;
	chomp($proto);
	if(length($proto) == 0){
		$proto = `cat $stubs/$functions/$current_stub | head -n 1`;
		chomp($proto);
		$proto =~ s/{//;
	}
	print PROTO "$proto;\n";
}

foreach(`$ls $stubs/$variables`){
        $current_stub = $_;
        chomp($current_stub);
        $final_stub = $current_stub;
        $final_stub =~ s/\..*$/.o/;
        $stub_name = $final_stub;
        $stub_name =~ s/\..*$//;
        print FH "    [$cb_counter] = { .function_name = \"$stub_name\", .function_ptr = &$stub_name },\n";
        $cb_counter++;
	$proto = `cat $stubs/$variables/$current_stub | head -n 1`;
	chomp($proto);
	$proto =~ s/=//;
	$proto =~ s/{//;
	print PROTO "extern $proto;\n";
}

# print a last entry with NULL fields
print FH "    [$cb_counter] = { .function_name = NULL, .function_ptr = NULL },\n";

print FH "};\n\n";
close(FH) or die $!;
close(PROTO) or die $!;

foreach(`$ls $stubs/$functions`){
        $current_stub = $_;
        chomp($current_stub);
        $final_stub = $current_stub;
        $final_stub =~ s/\..*$/.o/;
        $stub_name = $final_stub;
	system("$compiler @gcc_params @gcc_stub_params -include $include/$win_layer -c $stubs/$functions/$current_stub -o $build/$final_stub");
	print("[*] compiling $final_stub\n");
}

foreach(`$ls $stubs/$variables`){
	$current_stub = $_;
	chomp($current_stub);
	$final_stub = $current_stub;
	$final_stub =~ s/\..*$/.o/;
	$stub_name = $final_stub;
	$stub_name =~ s/\..*$//;
	system("$compiler @gcc_params -include $include/$win_layer -c $stubs/$variables/$current_stub -o $build/$final_stub");
	print("[*] compiling $final_stub\n");
}

# last step: compile the main application

foreach(`$ls $build`){
	$current_stub = $_;
	chomp($current_stub);
	$final_obj_line .= "$build/$current_stub ";
}

print(color("red"));
print("[*] compiling $final_program\n");
print(color("reset"));
system("$compiler @gcc_params @gcc_final_params $source/$main -o $bin/$final_program $final_obj_line @gcc_final_linkkings");

undef(@struct_fields);
undef(@gcc_params);
undef(@gcc_final_params);
undef(@gcc_final_linkkings);
