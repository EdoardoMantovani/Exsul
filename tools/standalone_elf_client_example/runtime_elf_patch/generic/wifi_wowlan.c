/* Made by Edoardo Mantovani
 * simple C "script" which permits to put the Wi-Fi card in WoWlan mode
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int wifi_suspend(void);
int elf_initialize_driver(void);
int wifi_connect(unsigned char *network_name, unsigned char *network_passkey);

int main(int argc, char *argv[]){
        int ret = 0;
        unsigned char network_ssid[32] = {};
        unsigned char network_passkey[32] = {};

	ret = elf_initialize_driver();
	if(ret != 0){
		printf("[!] failed to initialize the driver: %d\n", ret);
		return ret;
	}
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
	if(ret != 0){
		printf("[!] failed to start the Wi-Fi connection: %d\n", ret);
		return ret;
	}
	printf("[*] waiting for the Wi-Fi connection before entering WoWLAN mode...\n");
	ret = wifi_suspend();
	if(ret != 0){
		printf("[!] failed to enter WoWLAN mode: %d\n", ret);
		return ret;
	}
	printf("[*] entered WoWLAN mode\n");
	return ret;
}
