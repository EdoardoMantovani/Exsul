#!/usr/bin/perl

# Simple tool which merge every stubs and header into a big .c file which can be used for llm based analysis and/or debugging and/or portability

use strict;
use warnings;
no strict 'subs';

my $cat = `which cat`;
my $find = `which find`;

my $stubs = "./stubs";
my $func  = "/functions/";

my $layer = "./inc/layer.h";
my $utils = "./inc/utils.h";

my $merged = "./merged_api.h";

my $cur_file = undef;
my $saved_cat = undef;

chomp($cat);
chomp($find);

open(FH, '>', $merged) or die $!;
print FH "/* Copyright (C) Edoardo Mantovani - All Rights Reserved\n";
print FH "* Unauthorized copying of this file, via any medium is strictly prohibited\n";
print FH "* Proprietary and confidential\n";
print FH "*/\n\n";

$saved_cat = `$cat $layer`;
chomp($saved_cat);
print FH $saved_cat;
print FH "\n\n";
$saved_cat = `$cat $utils`;
chomp($saved_cat);
print FH $saved_cat;
print FH "\n\n";

foreach(`$find $stubs/$func`){
	$cur_file = $_;
	chomp($cur_file);
	$saved_cat = `$cat $cur_file`;
	chomp($saved_cat);
	print FH $saved_cat;
	print FH "\n\n";
}

close(FH);
