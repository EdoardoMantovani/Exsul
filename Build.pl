#!/usr/bin/perl

# Made by Edoardo Mantovani, 2026

use strict;
use warnings;
use Term::ANSIColor qw(color);
use Getopt::Long;

my $ls = `which ls`;
my $rm = `which rm`;
my $sed = `which sed`;
my $grep = `which grep`;
my $find = `which find`;

my $arch   = "./arch";
my $source = "./src";
my $include = "./inc";
my $stubs = "./stubs";
my $build = "./build";
my $bin   = "./bin";

my $final_program = "translator";

my $main      = "main.c";
my $win_layer = "layer.h";
my $proto_gen = "prototypes.h";
my $cb_table  = "cb_table.h";

my $functions = "functions";
my $variables = "variables";

my $function_proto_regex = q{^\h*[_A-Za-z]\w*(?:[\w\h\*]*\h+)+[_A-Za-z]\w*\h*\([^;{}]*\)\h*(?=\{)};

my $final_obj_line = undef;
my $proto = undef;
my $current_stub = undef;
my $final_stub = undef;
my $stub_name = undef;
my $cb_header = undef;
my $cb_counter = 0;
my $proto_header = undef;
my $iterator = undef;

my $compiler = undef;
my $pahole_kernel = undef;
my $clean = undef;
my $help = undef;

my @gcc_params = ("-O2", "-w", "-I$include", "-I$arch", "-march=native", "-mtune=native");
my @gcc_final_params = ("-fno-pie", "-no-pie", "-rdynamic");
my @gcc_final_linkkings = ("-lusb-1.0");

my %struct_whitelist = (
	"skb" => "sk_buff",
	"newsk" => "sk_buff",
	"dev" => "device",
	"mod" => "module",
	"wiphy" => "wiphy",
	"udev" => "usb_device",
	"hw" => "ieee80211_hw",
	"rate" => "rate_info"
);

chomp($ls);
chomp($rm);
chomp($sed);
chomp($grep);
chomp($find);

GetOptions(
	"compiler=s"		=> \$compiler,
	"pahole_kernel=s"	=> \$pahole_kernel,
	"clean"			=> \$clean,
	"help"			=> \$help,
);

if($help){
	print("helper\n");
	exit(0);
}

if($clean){
	#delete every compiled file and quit
	`$rm $build/*.o`;
	`$rm $bin/$final_program`;
	exit(0);
}

if(!$compiler){
	$compiler = "gcc";
}

foreach(keys %struct_whitelist){
        #iterate for every struct function
        $iterator = $_;
        chomp($iterator);
#        print `$grep -Ri "$iterator->"`;
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

print FH "};\n\n";
close(FH);
close(PROTO);

sleep(0.5);

foreach(`$ls $stubs/$functions`){
        $current_stub = $_;
        chomp($current_stub);
        $final_stub = $current_stub;
        $final_stub =~ s/\..*$/.o/;
        $stub_name = $final_stub;
	system("$compiler @gcc_params -include $include/$win_layer -c $stubs/$functions/$current_stub -o $build/$final_stub");
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

system("$compiler @gcc_params @gcc_final_params $source/$main -o $bin/$final_program $final_obj_line @gcc_final_linkkings");
print(color("reset"));

undef(@gcc_params);
undef(@gcc_final_params);
undef(@gcc_final_linkkings);
