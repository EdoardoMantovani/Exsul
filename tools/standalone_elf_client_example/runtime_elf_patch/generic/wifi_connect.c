/* Made by Edoardo Mantovani
 * simple C "script" which permits to connect to a Wi-Fi network given its name and its password
 */
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
	int ret = 0;
	unsigned char network_ssid[32] = {};
	unsigned char network_passkey[32] = {};

	elf_initialize_driver();
	printf("[*] insert the network name: ");
	fgets(network_ssid, sizeof(network_ssid), stdin);
	network_ssid[strlen(network_ssid) - 1] = '\0';
	printf("[*] insert the network passkey: ");
	fgets(network_passkey, sizeof(network_passkey), stdin);
	network_passkey[strlen(network_passkey) - 1] = '\0';
	printf("--------\n");
	printf("[*] network name: %s, network passkey: %s\n", network_ssid, network_passkey);
	printf("--------\n");
	/* start the authentication connection process */
	ret = wifi_connect(network_ssid, network_passkey);
	return ret;
}
