/* Made by Edoardo Mantovani, C code still not fully tested, it should permit to create a p2p network and allow the communication between 2 clients */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#ifndef EINVAL
#define EINVAL 22
#endif

#ifndef EAGAIN
#define EAGAIN 11
#endif

#define P2P_PEERS 32
#define P2P_MESSAGE_MAX 248
#define P2P_ACTION_SUBTYPE 7
#define P2P_ATTRIBUTE_ID 0xdd
#define WLAN_EID_VENDOR_SPECIFIC 221

struct p2p_peer {
	uint8_t address[6];
	uint8_t device_address[6];
	uint8_t device_capability;
	uint8_t group_capability;
	unsigned int frequency;
	uint64_t last_seen_ms;
	char name[33];
};

int wifi_p2p_stop(void);
int wifi_scan_stop(void);
int elf_initialize_driver(void);
int wifi_p2p_find(unsigned int duration_ms);
int wifi_p2p_get_peer(unsigned int index, struct p2p_peer *peer);
int wifi_p2p_start(const char *device_name, unsigned int frequency);
int wifi_p2p_listen(unsigned int frequency, unsigned int duration_ms);
int wifi_p2p_receive(void *buffer, int capacity, unsigned int timeout_ms);
int wifi_p2p_send_action(const uint8_t *destination, uint8_t subtype, uint8_t dialog_token, const void *attributes, int length);

/* read_line: read the input cmdline from the user */
static int read_line(char *buffer, size_t capacity){
	int ret = 0;
	int character = 0;
	size_t length = 0;

	if(buffer == NULL || capacity < 2){
		ret = -EINVAL;
		return ret;
	}
	if(fgets(buffer, capacity, stdin) == NULL){
		ret = -EINVAL;
		return ret;
	}
	length = strcspn(buffer, "\r\n");
	if(buffer[length] == '\0'){
		character = getchar();
		if(character != '\n' && character != EOF){
			while(character != '\n' && character != EOF){
				character = getchar();
			}
			buffer[0] = '\0';
			ret = -EINVAL;
			return ret;
		}
	}else{
		buffer[length] = '\0';
	}
	if(length == 0){
		ret = -EINVAL;
	}
	return ret;
}

/* parse_frequency: parse the frequency string and transform it into a number */
static int parse_frequency(const char *string, unsigned int *frequency){
	int ret = 0;

	if(string == NULL || frequency == NULL){
		ret = -EINVAL;
		return ret;
	}
	if(strcmp(string, "2412") == 0){
		*frequency = 2412;
	}else if(strcmp(string, "2437") == 0){
		*frequency = 2437;
	}else if(strcmp(string, "2462") == 0){
		*frequency = 2462;
	}else{
		ret = -EINVAL;
	}
	return ret;
}

/* wait_for_p2p_peer: wait until a new p2p peer connect to the network */
static int wait_for_p2p_peer(struct p2p_peer *peer){
	int ret = 0;

	if(peer == NULL){
		ret = -EINVAL;
		return ret;
	}
	for(int attempt = 0; attempt < 200; attempt++){
		for(int index = 0; index < P2P_PEERS; index++){
			ret = wifi_p2p_get_peer(index, peer);
			if(ret == 0){
				return ret;
			}
		}
		usleep(100000);
	}
	ret = -EINVAL;
	return ret;
}

/* print_p2p_payload: dump the p2p payload to the screen */
static int print_p2p_payload(const uint8_t *frame, int frame_length){
	int ret = 0;
	int offset = 0;
	int attribute_offset = 0;
	int attributes_length = 0;
	int ie_length = 0;
	int attribute_length = 0;
	char message[P2P_MESSAGE_MAX + 1] = {};

	if(frame == NULL || frame_length < 32 ||
	   memcmp(frame + 24, "\x04\x09\x50\x6f\x9a\x09", 6)){
		ret = -EINVAL;
		return ret;
	}
	offset = 32;
	while(offset + 2 <= frame_length){
		ie_length = frame[offset + 1];
		if(offset + 2 + ie_length > frame_length){
			ret = -EINVAL;
			return ret;
		}
		if(frame[offset] == WLAN_EID_VENDOR_SPECIFIC && ie_length >= 7 && !memcmp(frame + offset + 2, "\x50\x6f\x9a\x09", 4)){
			attribute_offset = offset + 6;
			attributes_length = ie_length - 4;
			while(attributes_length >= 3){
				attribute_length = frame[attribute_offset + 1] | (frame[attribute_offset + 2] << 8);
				if(attribute_length > attributes_length - 3){
					ret = -EINVAL;
					return ret;
				}
				if(frame[attribute_offset] == P2P_ATTRIBUTE_ID){
					memcpy(message, frame + attribute_offset + 3, attribute_length);
					message[attribute_length] = '\0';
					printf("[*] P2P payload: %s\n", message);
					return ret;
				}
				attribute_offset += attribute_length + 3;
				attributes_length -= attribute_length + 3;
			}
		}
		offset += ie_length + 2;
	}
	ret = -EINVAL;
	return ret;
}

int main(void){
	int ret = 0;
	int message_length = 0;
	unsigned int p2p_frequency = 0;
	bool sender = false;
	bool p2p_started = false;
	char role[4] = {};
	uint8_t frame[2400] = {};
	struct p2p_peer peer = {};
	char p2p_device_name[33] = {};
	char p2p_frequency_string[16] = {};
	char message[P2P_MESSAGE_MAX + 1] = {};
	uint8_t p2p_attributes[P2P_MESSAGE_MAX + 3] = {};

	ret = elf_initialize_driver();
	if(ret != 0){
		printf("[!] driver initialization failed: %d\n", ret);
		return ret;
	}
	ret = wifi_scan_stop();
	if(ret != 0){
		printf("[!] failed to stop the Wi-Fi scan: %d\n", ret);
		return ret;
	}

	printf("[*] insert the P2P frequency [2412/2437/2462]: ");
	ret = read_line(p2p_frequency_string, sizeof(p2p_frequency_string));
	if(ret != 0 || parse_frequency(p2p_frequency_string, &p2p_frequency) != 0){
		printf("[!] invalid P2P frequency, aborting!\n");
		ret = -EINVAL;
		return ret;
	}

	printf("[*] insert the P2P device name: ");
	ret = read_line(p2p_device_name, sizeof(p2p_device_name));
	if(ret != 0){
		printf("[!] invalid P2P device name, aborting!\n");
		return ret;
	}

	printf("[*] select the P2P role [s=sender/r=receiver]: ");
	ret = read_line(role, sizeof(role));
	if(ret != 0 || role[1] != '\0' ||
	   (role[0] != 's' && role[0] != 'S' && role[0] != 'r' && role[0] != 'R')){
		printf("[!] invalid P2P role, aborting!\n");
		ret = -EINVAL;
		return ret;
	}
	sender = role[0] == 's' || role[0] == 'S';

	if(sender == true){
		printf("[*] write the P2P message: ");
		ret = read_line(message, sizeof(message));
		if(ret != 0){
			printf("[!] invalid P2P message, aborting!\n");
			return ret;
		}
		message_length = strlen(message);
		p2p_attributes[0] = P2P_ATTRIBUTE_ID;
		p2p_attributes[1] = message_length & 0xff;
		p2p_attributes[2] = message_length >> 8;
		memcpy(p2p_attributes + 3, message, message_length);
	}

	ret = wifi_p2p_start(p2p_device_name, p2p_frequency);
	if(ret != 0){
		printf("[!] P2P start failed: %d\n", ret);
		return ret;
	}
	p2p_started = true;

	if(sender == true){
		ret = wifi_p2p_find(15000);
		if(ret != 0){
			printf("[!] P2P discovery failed: %d\n", ret);
			goto out;
		}

		ret = wait_for_p2p_peer(&peer);
		if(ret != 0){
			printf("[!] no P2P peer discovered\n");
			goto out;
		}

		printf("[*] discovered %s at %02x:%02x:%02x:%02x:%02x:%02x, %u MHz\n",
			peer.name,
			peer.address[0], peer.address[1], peer.address[2],
			peer.address[3], peer.address[4], peer.address[5],
			peer.frequency);

		ret = wifi_p2p_listen(peer.frequency, 5000);
		if(ret != 0){
			printf("[!] remain-on-channel failed: %d\n", ret);
			goto out;
		}
		usleep(200000);

		for(int attempt = 0; attempt < 20; attempt++){
			ret = wifi_p2p_send_action(peer.address, P2P_ACTION_SUBTYPE, 1, p2p_attributes, message_length + 3);
			if(ret == 0){
				printf("[*] P2P action frame sent\n");
				break;
			}
			usleep(100000);
		}
		if(ret != 0){
			printf("[!] failed to send the P2P action frame: %d\n", ret);
		}
	}else{
		printf("[*] waiting for a P2P action frame on %u MHz\n", p2p_frequency);
		for(;;){
			ret = wifi_p2p_listen(p2p_frequency, 5000);
			if(ret != 0){
				printf("[!] P2P listen failed: %d\n", ret);
				break;
			}
			ret = wifi_p2p_receive(frame, sizeof(frame), 5000);
			if(ret > 0){
				printf("[*] received %d-byte P2P management frame\n", ret);
				if(print_p2p_payload(frame, ret) == 0){
					ret = 0;
					break;
				}
			}else if(ret != -EAGAIN){
				printf("[!] P2P receive failed: %d\n", ret);
				break;
			}
		}
	}
	out:
	if(p2p_started == true){
		wifi_p2p_stop();
	}
	return ret < 0 ? ret : 0;
}
