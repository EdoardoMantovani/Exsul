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
my %udev_rtw89_devices = (
	# rtw8851bu
	# pid		 vid
	# --------------------------
	"b851" => "0bda",
	"332a" => "2001",
	"010b" => "3625",
	"e611" => "7392",
	# rtw8852bu
	# pid		 vid
	# --------------------------
	"b832" => "0bda",
	"b83a" => "0bda",
	"b852" => "0bda",
	"b85a" => "0bda",
	"a85b" => "0bda",
	"3428" => "0586",
	"1a62" => "0b05",
	"6931" => "0db0",
	"3327" => "2001",
	"6121" => "3574",
	"0100" => "35bc",
	"0108" => "35bc",
	"6822" => "7392",
	# rtw8852cu
	# pid		 vid
	# --------------------------
	"c832" => "0bda",
	"c85a" => "0bda",
	"c85d" => "0bda",
	"991d" => "0db0",
	"0502" => "35b2",
	"0101" => "35bc",
	"0102" => "35bc",
	# rtw8922au
	# pid		 vid
	# --------------------------
	"76d7" => "0502",
	"1bcf" => "0b05",
	"1bd2" => "0b05",
	"8912" => "0bda",
	"da0e" => "0db0",
	"010a" => "3625",
	"3822" => "7392",
);

if($default && ($vid || $pid)){
	printf("[!] error\n");
}

open my $udev, '>', '/etc/udev/rules.d/99-userspace-executor.rules' or die $!;
while(my ($rtw89_pid, $rtw89_vid) = each %udev_rtw89_devices){
        $current_udev_line = "SUBSYSTEM==\"usb\", ATTR{idVendor}==\"$rtw89_vid\", ATTR{idProduct}==\"$rtw89_pid\", MODE=\"0660\", GROUP=\"plugdev\"";
        print $udev "$current_udev_line\n";
}
close($udev) or die $!;

# - reload rules
system("sudo udevadm control --reload-rules");
system("sudo udevadm trigger");
