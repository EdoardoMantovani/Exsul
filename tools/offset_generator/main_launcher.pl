#/usr/bin/perl

# Made by Edoardo Mantovani 2026

use strict;
use warnings;
use JSON;
use List::Util qw(uniq);
use Data::Dumper qw(Dumper);

my $json_file = shift || "struct_mapping.json";

my $ls = `which ls`;
my $cd = `which cd`;
my $rm = `which rm`;
my $awk = `which awk`;
my $sed = `which sed`;
my $git = `which git`;
my $grep = `which grep`;
my $tail = `which tail`;
my $make = `which make`;
my $wget = `which wget`;
my $head = `which head`;
my $unzip = `which unzip`;
my $strings = `which strings`;
my $pahole  = `which pahole`;

my $arch = "x86";
my $defconfig = "x86_64_defconfig";
my $pahole_btf_file = "vmlinux";
my $runtime_struct_created = 0;
my $kernel_version = 0;
my $patch_level = 0;
my $sub_level = 0;
my $extra_version = 0;
my $tail_args = "-n 320";
my $linux_git_args = "ls-remote --tags";
my $objtool_fix_path = "tools/objtool/elf.c";
my $defconfig_path = "arch/x86/configs/x86_64_defconfig";
my $linux_repo = "https://www.github.com/torvalds/linux/";
my $linux_refs = "archive/refs/tags/";
my $backup_header = "offset_backup_solution.h";

my $awk_args = q{'{print $2}'};
my $cleanup_args = q{'s/\^\{\}//'};
my $branch_extract = q{sed 's/\// /g' | awk '{print $3}'};
my $sed_objtool_fix = q{-i 's/return -1/return 0/g'};
my $version_extract = q{'{print $3}'};

my @linux_tags = ();

my $iterator = undef;
my $current_struct = undef;
my $pahole_output = undef;
my $current_offset = 0;

chomp($ls);
chomp($cd);
chomp($rm);
chomp($awk);
chomp($sed);
chomp($git);
chomp($grep);
chomp($make);
chomp($tail);
chomp($wget);
chomp($head);
chomp($unzip);
chomp($strings);
chomp($pahole);

my $config_addition = "
CONFIG_DEBUG_INFO=y
CONFIG_AS_HAS_NON_CONST_LEB128=y
CONFIG_DEBUG_INFO_DWARF5=y
CONFIG_DEBUG_INFO_REDUCED=n
CONFIG_DEBUG_INFO_COMPRESSED=n
CONFIG_DEBUG_INFO_SPLIT=n
CONFIG_PAHOLE_HAS_SPLIT_BTF=n
CONFIG_GDB_SCRIPTS=n";

my $json_text = do { open my $fh, '<', $json_file; local $/; <$fh> };
if(!($json_text)){
	print("[!] invalid json file!\n");
}

my $perl_data = decode_json $json_text;

for (@{$perl_data->{structs}}) {
	if($_->{name} eq "NULL"){
		last;
	}
	$runtime_struct_created++;
}

# get every tags
foreach(`$git $linux_git_args $linux_repo 2>/dev/null | $tail $tail_args | $awk $awk_args | $sed $cleanup_args | $branch_extract`){
	$iterator = $_;
	chomp($iterator);
	push(@linux_tags, $iterator);
}

$linux_repo .= $linux_refs;

open(OFFSET_BACKUP_SOL, '>', $backup_header) or die $!;

print OFFSET_BACKUP_SOL "struct struct_field {\n";
print OFFSET_BACKUP_SOL "	unsigned char  field_name[24];\n";
print OFFSET_BACKUP_SOL "	unsigned short field_offset;\n";
print OFFSET_BACKUP_SOL "};\n";
print OFFSET_BACKUP_SOL "struct runtime_struct_layout {\n";
print OFFSET_BACKUP_SOL "	unsigned char kernel_version;\n";
print OFFSET_BACKUP_SOL	"	unsigned char kernel_patch_level;\n";
print OFFSET_BACKUP_SOL	"	unsigned char kernel_sublevel;\n";
print OFFSET_BACKUP_SOL "	unsigned char kernel_extra_version;\n";
print OFFSET_BACKUP_SOL "	unsigned char *struct_name;\n";
print OFFSET_BACKUP_SOL "	unsigned short struct_field_num;\n";
print OFFSET_BACKUP_SOL "	struct struct_field fields[64];\n";
print OFFSET_BACKUP_SOL "};\n";
print OFFSET_BACKUP_SOL "struct runtime_struct {\n";
print OFFSET_BACKUP_SOL "	int compilation_time_generated_structs;\n";
print OFFSET_BACKUP_SOL "	struct runtime_struct_layout structs[$runtime_struct_created];\n";
print OFFSET_BACKUP_SOL "};\n";

print OFFSET_BACKUP_SOL "struct runtime_struct kernel_backup_archive = {\n";

foreach(uniq(@linux_tags)){
	print("[*] downloading linux $_\n");
	`$wget $linux_repo/$_.zip  -q --show-progress`;
	`$unzip *.zip`;
	`$rm *.zip`;
	$iterator = "linux-$_";
	$iterator =~ s/v//;
	chdir($iterator) or die $!;

	# get the kernel version, patch level and sub level
	$kernel_version = `$strings Makefile | $grep "^VERSION =" | $awk $version_extract`;
	chomp($kernel_version);
	$patch_level = `$strings Makefile | $grep "^PATCHLEVEL =" | $awk $version_extract`;
	chomp($patch_level);
	$sub_level = `$strings Makefile | $grep "^SUBLEVEL =" | $awk $version_extract`;
	chomp($sub_level);
	$extra_version = `$strings Makefile | $grep "^EXTRAVERSION =" | $awk $version_extract`;
	chomp($extra_version);
	print("[*] current kernel version: $kernel_version, current kernel patch level: $patch_level, current sub_level: $sub_level extra_version: $extra_version\n");

	open(my $defconfig, '>>', $defconfig_path) or die $!;
	print $defconfig "$config_addition\n";
	close($defconfig) or die $!;
	`$sed $sed_objtool_fix $objtool_fix_path`;
	`$make ARCH=$arch x86_64_defconfig`;
	# add the suppression for every warnings
	`$make "KCFLAGS=-Wno-error -w" "HOSTCFLAGS=-Wno-error -w" -j16 2>/dev/null`;
	# start to iterate the struct XX with field YY for pahole
	for (@{$perl_data->{structs}}) {
		if($_->{name} eq "NULL"){
			last;
		}
		$current_struct = $_->{name};
		print "current struct: $_->{name}\n";
		for(my $x = 0; length($_->{fields}[$x]); $x++){
			$pahole_output = `$pahole -C $current_struct $pahole_btf_file | $grep "$_->{fields}[$x]"`;
			chomp($pahole_output);
			#$pahole_output =~ /\b$_->{fields}[$x]\b(?:\s*;|\s*\[)/;
			$pahole_output =~ /\/\*+\s*(\d+)/;
			print("[*] field $_->{fields}[$x] has offset $1\n");
		}
	}

	chdir("../") or die $!;
	`$rm -rf $iterator`;
}
print OFFSET_BACKUP_SOL "};\n";
close(OFFSET_BACKUP_SOL);
