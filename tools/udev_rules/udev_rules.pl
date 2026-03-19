# Made by Edoardo Mantovani, 2026
# Simple Perl script which permits to add udev rules for using usb devices without root permissions

use strict;
use warnings;
use Getopt::Long;

my $pid = undef;
my $vid = undef;
my $default = undef;
my $current_udev_line = undef;

GetOptions(
	"pid=s"		=> \$pid,
	"vid=s"		=> \$vid,
	"default"	=> \$default,
);

# the "demo" will be focused on rtw89 driver, so the 'default' option will add to udev every rtw89 dongle known to the drivers
my %udev_rtw89_devices = {
	# rtw8852bu
	# pid		 vid
	# --------------------------
	"0x0000b832" => "0x00000bda",
	"0x0000b83a" => "0x00000bda",
	"0x0000b852" => "0x00000bda",
	"0x0000b85a" => "0x00000bda",
	"0x0000a85b" => "0x00000bda",
	"0x00003428" => "0x00000586",
	"0x00001a62" => "0x00000b05",
	"0x00006931" => "0x00000db0",
	"0x00003327" => "0x00002001",
	"0x00006121" => "0x00003574",
	"0x00000100" => "0x000035bc",
	"0x00000108" => "0x000035bc",
	"0x00006822" => "0x00007392",
	# rtw8852cu
	# pid		 vid
	# ---------------------------
	"0x0000c832" => "0x00000bda",
	"0x0000c85a" => "0x00000bda",
	"0x0000c85d" => "0x00000bda",
	"0x0000991d" => "0x00000db0",
	"0x00000502" => "0x000035b2",
	"0x00000101" => "0x000035bc",
	"0x00000102" => "0x000035bc",

};

if($default && ($vid || $pid)){
	printf("[!] error\n");
}

$current_udev_line = "SUBSYSTEM==\"usb\", ATTR{idVendor}==\"$vid\", ATTR{idProduct}==\"$pid\", MODE=\"0660\", GROUP=\"plugdev\"";
open my $udev, '>', '/etc/udev/rules.d/99-userspace-drv.rules' or die "Cannot open udev rules file: $!";


close($udev);
