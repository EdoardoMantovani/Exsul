/* Made by Edoardo Mantovani
 * simple C "script" which permits to enable mesh networking
 */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>
#include <sys/select.h>

#ifndef EINVAL
#define EINVAL 22
#endif

#ifndef EAGAIN
#define EAGAIN 11
#endif

#ifndef ETIMEDOUT
#define ETIMEDOUT 110
#endif

#define MESH_PEERS		32
#define MESH_ETHERTYPE		0x88b5
#define MESH_MESSAGE_MAX	127
#define MESH_NODE_NAME_MAX	32
#define MESH_PROTOCOL_HEADER	12
#define MESH_PROTOCOL_VERSION	1
#define MESH_MESSAGE_HELLO	1
#define MESH_MESSAGE_TEXT	2
#define MESH_MESSAGE_BYE	3

/*
 * enum nl80211_plink_state - state of a mesh peer link finite state machine
 *
 * @NL80211_PLINK_LISTEN: initial state, considered the implicit
 *	state of non-existent mesh peer links
 * @NL80211_PLINK_OPN_SNT: mesh plink open frame has been sent to
 *	this mesh peer
 * @NL80211_PLINK_OPN_RCVD: mesh plink open frame has been received
 *	from this mesh peer
 * @NL80211_PLINK_CNF_RCVD: mesh plink confirm frame has been
 *	received from this mesh peer
 * @NL80211_PLINK_ESTAB: mesh peer link is established
 * @NL80211_PLINK_HOLDING: mesh peer link is being closed or cancelled
 * @NL80211_PLINK_BLOCKED: all frames transmitted from this mesh
 *	plink are discarded, except for authentication frames
 * @NUM_NL80211_PLINK_STATES: number of peer link states
 * @MAX_NL80211_PLINK_STATES: highest numerical value of plink states
 */
#ifndef NL80211_PLINK_LISTEN
#define NL80211_PLINK_LISTEN	0
#endif
#ifndef NL80211_PLINK_OPN_SNT
#define NL80211_PLINK_OPN_SNT	1
#endif
#ifndef NL80211_PLINK_OPN_RCVD
#define NL80211_PLINK_OPN_RCVD	2
#endif
#ifndef NL80211_PLINK_CNF_RCVD
#define NL80211_PLINK_CNF_RCVD	3
#endif
#ifndef NL80211_PLINK_ESTAB
#define NL80211_PLINK_ESTAB	4
#endif
#ifndef NL80211_PLINK_HOLDING
#define NL80211_PLINK_HOLDING	5
#endif
#ifndef NL80211_PLINK_BLOCKED
#define NL80211_PLINK_BLOCKED	6
#endif

int wifi_scan_stop(void);
int wifi_mesh_leave(void);
int elf_initialize_driver(void);
int wifi_mesh_join(const char *mesh_id, unsigned int frequency);
int wifi_mesh_receive(void *buffer, int capacity, unsigned int timeout_ms);
int wifi_mesh_get_peer(unsigned int index, unsigned char *address, unsigned int *plink_state);
int wifi_mesh_send(const unsigned char *destination, unsigned short proto, const void *payload, int length);

/* read_line: read the input from the user */
static int read_line(char *buffer, int capacity){
	int ret = 0;
	int length = 0;
	int character = 0;

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

/* parse_frequency: convert the frequency string into its number representation and verify it covers the correct ranges */
static int parse_frequency(const char *string, unsigned int *frequency){
	int ret = 0;
	unsigned int value = 0;

	if(string == NULL || frequency == NULL || string[0] == '\0'){
		ret = -EINVAL;
		return ret;
	}
	for(int index = 0; string[index] != '\0'; index++){
		if(string[index] < '0' || string[index] > '9' || value > 100000){
			ret = -EINVAL;
			return ret;
		}
		value = value * 10 + (string[index] - '0');
	}
	if(value < 2000 || value > 8000){
		ret = -EINVAL;
		return ret;
	}
	*frequency = value;
	return ret;
}

/* state_to_string: convert the current state of the mesh node into a string, used for debugging */
static const char *state_to_string(int state){
	int ret = 0;

	switch(state){
		case NL80211_PLINK_LISTEN:
			return "listen";
		case NL80211_PLINK_OPN_SNT:
			return "open frame sent";
		case NL80211_PLINK_OPN_RCVD:
			return "open frame received";
		case NL80211_PLINK_CNF_RCVD:
			return "frame received from peer";
		case NL80211_PLINK_ESTAB:
			return "link established";
		case NL80211_PLINK_HOLDING:
			return "link is closed";
		case NL80211_PLINK_BLOCKED:
			return "link is open only for authentication frames!";
		default:
			return "unknown state";
	}
}

/* utils for putting a value in u16/u32 into a buffer */
static void wire_put_u16(unsigned char *buffer, unsigned int value){
	buffer[0] = value >> 8;
	buffer[1] = value;
}

static void wire_put_u32(unsigned char *buffer, uint32_t value){
	buffer[0] = value >> 24;
	buffer[1] = value >> 16;
	buffer[2] = value >> 8;
	buffer[3] = value;
}

/* utils for getting u16/u32 values from a buffer */
static unsigned int wire_get_u16(const unsigned char *buffer){
	int ret = 0;

	ret = ((unsigned int)buffer[0] << 8) | buffer[1];
	return ret;
}

static uint32_t wire_get_u32(const unsigned char *buffer){
	int ret = 0;
	uint32_t value = 0;

	value = ((uint32_t)buffer[0] << 24) | ((uint32_t)buffer[1] << 16) | ((uint32_t)buffer[2] << 8) | buffer[3];
	return value;
}

static bool mesh_text_valid(const unsigned char *text, unsigned int length){
	int ret = 0;

	if(text == NULL || length == 0){
		ret = false;
		return ret;
	}
	for(int index = 0; index < length; index++){
		if(text[index] < 0x20 || text[index] == 0x7f){
			ret = false;
			return ret;
		}
	}
	ret = true;
	return ret;
}

/* mesh_message_send: send a mesh message, high level API */
static int mesh_message_send(const unsigned char *destination, unsigned int type, uint32_t sequence, const void *payload, unsigned int payload_length){
	int ret = 0;
	unsigned char packet[MESH_PROTOCOL_HEADER + MESH_MESSAGE_MAX] = {};

	if(destination == NULL || type < MESH_MESSAGE_HELLO || type > MESH_MESSAGE_BYE || payload_length > MESH_MESSAGE_MAX || (payload_length != 0 && payload == NULL)){
		ret = -EINVAL;
		return ret;
	}
	if((type == MESH_MESSAGE_BYE && payload_length != 0) || (type == MESH_MESSAGE_HELLO && (payload_length == 0 || payload_length > MESH_NODE_NAME_MAX)) ||
	   (type == MESH_MESSAGE_TEXT && payload_length == 0) || (type != MESH_MESSAGE_BYE && mesh_text_valid(payload, payload_length) == false)){
		ret = -EINVAL;
		return ret;
	}
	/* use 'EXSM' for the mesh message header */
	memcpy(packet, "EXSM", 4);
	packet[4] = MESH_PROTOCOL_VERSION;
	packet[5] = type;
	wire_put_u16(packet + 6, payload_length);
	wire_put_u32(packet + 8, sequence);
	if(payload_length != 0){
		memcpy(packet + MESH_PROTOCOL_HEADER, payload, payload_length);
	}
	ret = wifi_mesh_send(destination, MESH_ETHERTYPE, packet, MESH_PROTOCOL_HEADER + payload_length);
	return ret;
}

/* mesh_message_parse: parse an incoming mesh message from another node */
static int mesh_message_parse(const unsigned char *frame, int frame_length, unsigned char *source, unsigned int *type, uint32_t *sequence, const unsigned char **payload, unsigned int *payload_length){
	int ret = 0;
	unsigned int length = 0;
	const unsigned char *packet = NULL;

	if(frame == NULL || source == NULL || type == NULL || sequence == NULL || payload == NULL || payload_length == NULL || frame_length < 14 + MESH_PROTOCOL_HEADER){
		ret = -EINVAL;
		return ret;
	}
	if(frame[12] != (MESH_ETHERTYPE >> 8) || frame[13] != (MESH_ETHERTYPE & 0xff)){
		ret = -EAGAIN;
		return ret;
	}
	packet = frame + 14;
	/* check if in the header the first 4 bytes are 'EXSM' (assembled in the function above) */
	if(memcmp(packet, "EXSM", 4) || packet[4] != MESH_PROTOCOL_VERSION || packet[5] < MESH_MESSAGE_HELLO || packet[5] > MESH_MESSAGE_BYE){
		ret = -EINVAL;
		return ret;
	}
	length = wire_get_u16(packet + 6);
	if(length > MESH_MESSAGE_MAX || frame_length != 14 + MESH_PROTOCOL_HEADER + length || (packet[5] == MESH_MESSAGE_BYE && length != 0)){
		ret = -EINVAL;
		return ret;
	}
	memcpy(source, frame + 6, 6);
	*type = packet[5];
	*sequence = wire_get_u32(packet + 8);
	*payload = packet + MESH_PROTOCOL_HEADER;
	*payload_length = length;
	return ret;
}

static int terminal_ready(void){
	int ret = 0;
	fd_set descriptors;
	struct timeval timeout = {};

	FD_ZERO(&descriptors);
	FD_SET(STDIN_FILENO, &descriptors);
	ret = select(STDIN_FILENO + 1, &descriptors, NULL, NULL, &timeout);
	if(ret < 0){
		if(errno == EINTR){
			ret = 0;
		}else{
			ret = -errno;
		}
		return ret;
	}
	ret = ret != 0 && FD_ISSET(STDIN_FILENO, &descriptors);
	return ret;
}

/* print_prompt: print the current mesh messaging shell */
static void print_prompt(const char *peer_name, const unsigned char *peer){
	int ret = 0;

	if(peer_name != NULL && peer_name[0] != '\0'){
		printf("[%s] > ", peer_name);
	}else{
		printf("[%02x:%02x:%02x:%02x:%02x:%02x] > ", peer[0], peer[1], peer[2], peer[3], peer[4], peer[5]);
	}
	fflush(stdout);
}

static int wait_for_mesh_peer(unsigned char *address){
	int ret = 0;
	unsigned int state = 0;
	bool discovered[MESH_PEERS] = {};
	unsigned int previous_state[MESH_PEERS] = {};

	if(address == NULL){
		ret = -EINVAL;
		return ret;
	}
	for(int attempt = 0; attempt < 300; attempt++){
		for(int index = 0; index < MESH_PEERS; index++){
			ret = wifi_mesh_get_peer(index, address, &state);
			if(ret == 0 && (discovered[index] == false || previous_state[index] != state)){
				printf("[*] mesh peer mac: %02x:%02x:%02x:%02x:%02x:%02x state=\'%s\'\n",
					address[0], address[1], address[2], address[3], address[4], address[5], state_to_string(state));
				discovered[index] = true;
				previous_state[index] = state;
			}
			if(ret == 0 && state == NL80211_PLINK_ESTAB){
				return ret;
			}
		}
		usleep(100000);
	}
	ret = -ETIMEDOUT;
	return ret;
}

int main(void){
	int ret = 0;
	int ready = 0;
	int leave_ret = 0;
	int frame_length = 0;
	int mesh_frequency = 0;
	bool mesh_joined = false;
	bool running = true;
	bool have_rx_sequence = false;
	bool prompt_visible = false;
	uint32_t tx_sequence = 1;
	uint32_t rx_sequence = 0;
	uint32_t message_sequence = 0;
	unsigned int message_type = 0;
	unsigned int payload_length = 0;
	char mesh_id[33] = {};
	unsigned char peer[6] = {};
	char mesh_node_name[33] = {};
	char peer_node_name[33] = {};
	unsigned char source[6] = {};
	unsigned char frame[2400] = {};
	char mesh_frequency_string[16] = {};
	char message[MESH_MESSAGE_MAX + 1] = {};
	const unsigned char *payload = NULL;

	ret = elf_initialize_driver();
	if(ret != 0){
		printf("[!] driver initialization failed: %d\n", ret);
		return ret;
	}
	/* stop the Wi-Fi scan for avoiding collision in the fw commands */
	ret = wifi_scan_stop();
	if(ret != 0){
		printf("[!] failed to stop the Wi-Fi scan: %d\n", ret);
		return ret;
	}

	printf("[*] insert the mesh frequency in MHz: ");
	ret = read_line(mesh_frequency_string, sizeof(mesh_frequency_string));
	if(ret != 0 || parse_frequency(mesh_frequency_string, &mesh_frequency) != 0){
		printf("[!] invalid mesh frequency, aborting!\n");
		ret = -EINVAL;
		return ret;
	}
	printf("[*] insert the mesh id: ");
	ret = read_line(mesh_id, sizeof(mesh_id));
	if(ret != 0){
		printf("[!] invalid mesh id, aborting!\n");
		return ret;
	}
	printf("[*] insert the mesh node name: ");
	ret = read_line(mesh_node_name, sizeof(mesh_node_name));
	if(ret != 0 || mesh_text_valid((const unsigned char *)mesh_node_name, strlen(mesh_node_name)) == false){
		printf("[!] invalid mesh node name, aborting!\n");
		ret = -EINVAL;
		return ret;
	}
	/* join the Wi-Fi mesh if possible */
	ret = wifi_mesh_join(mesh_id, mesh_frequency);
	if(ret != 0){
		printf("[!] mesh join failed: %d\n", ret);
		return ret;
	}
	mesh_joined = true;
	printf("[*] %s joined mesh \"%s\" on %u MHz\n", mesh_node_name, mesh_id, mesh_frequency);
	printf("[*] waiting for an established peer...\n");
	ret = wait_for_mesh_peer(peer);
	if(ret != 0){
		printf("[!] no established mesh peer: %d\n", ret);
		goto out;
	}
	/* send the hello message to the new peer node */
	ret = mesh_message_send(peer, MESH_MESSAGE_HELLO, tx_sequence++, mesh_node_name, strlen(mesh_node_name));
	if(ret != 0){
		printf("[!] failed to send the mesh greeting: %d\n", ret);
		goto out;
	}
	while(running == true){
		if(prompt_visible == false){
			print_prompt(peer_node_name, peer);
			prompt_visible = true;
		}

		ret = wifi_mesh_receive(frame, sizeof(frame), 100);
		if(ret >= 0){
			frame_length = ret;

			ret = mesh_message_parse(frame, frame_length, source, &message_type, &message_sequence, &payload, &payload_length);
			if(ret == -EAGAIN){
				ret = 0;
				continue;
			}
			if(ret != 0){
				printf("\n[!] discarded an invalid mesh message\n");
				prompt_visible = false;
				ret = 0;
				continue;
			}
			if(memcmp(source, peer, sizeof(peer))){
				printf("\n[!] ignored a message from an unselected mesh peer %02x:%02x:%02x:%02x:%02x:%02x\n",
					source[0], source[1], source[2], source[3], source[4], source[5]);
				prompt_visible = false;
				continue;
			}
			if(have_rx_sequence == true && (int32_t)(message_sequence - rx_sequence) <= 0){
				continue;
			}
			have_rx_sequence = true;
			rx_sequence = message_sequence;
			if(message_type == MESH_MESSAGE_HELLO){
				if(payload_length == 0 || payload_length > MESH_NODE_NAME_MAX || mesh_text_valid(payload, payload_length) == false){
					printf("\n[!] discarded an invalid mesh greeting\n");
				}else{
					memcpy(peer_node_name, payload, payload_length);
					peer_node_name[payload_length] = '\0';
					printf("\n[*] %s is connected!\n", peer_node_name);
				}
			}else if(message_type == MESH_MESSAGE_TEXT){
				if(payload_length == 0 || mesh_text_valid(payload, payload_length) == false){
					printf("\n[!] discarded an invalid text message\n");
				}else{
					printf("\n%s: %.*s\n", peer_node_name[0] != '\0' ? peer_node_name : "peer", (int)payload_length, payload);
				}
			}else if(message_type == MESH_MESSAGE_BYE){
				printf("\n[*] %s left the chat\n", peer_node_name[0] != '\0' ? peer_node_name : "peer");
				running = false;
			}
			prompt_visible = false;
			ret = 0;
		}else if(ret != -EAGAIN){
			printf("\n[!] mesh receive failed: %d\n", ret);
			goto out;
		}else{
			ret = 0;
		}
		if(running == false){
			break;
		}

		ready = terminal_ready();
		if(ready < 0){
			ret = ready;
			printf("\n[!] terminal input failed: %d\n", ret);
			goto out;
		}
		if(ready == 0){
			continue;
		}
		ret = read_line(message, sizeof(message));
		prompt_visible = false;
		if(ret != 0){
			if(feof(stdin)){
				mesh_message_send(peer, MESH_MESSAGE_BYE, tx_sequence++, NULL, 0);
				ret = 0;
				break;
			}
			printf("[!] invalid message; maximum length is %u characters\n", MESH_MESSAGE_MAX);
			ret = 0;
			continue;
		}
		/* implement the "quit" and "help" commands */
		if(!strcmp(message, "/quit")){
			ret = mesh_message_send(peer, MESH_MESSAGE_BYE, tx_sequence++, NULL, 0);
			if(ret != 0){
				printf("[!] failed to send the leave message: %d\n", ret);
			}
			break;
		}
		if(!strcmp(message, "/help")){
			printf("commands: /help, /quit\n");
			continue;
		}
		ret = mesh_message_send(peer, MESH_MESSAGE_TEXT, tx_sequence++, message, strlen(message));
		if(ret != 0){
			printf("[!] mesh transmission failed: %d\n", ret);
			goto out;
		}
		printf("you: %s\n", message);
	}

	out:
	if(mesh_joined == true){
		leave_ret = wifi_mesh_leave();
		if(ret == 0 && leave_ret != 0){
			ret = leave_ret;
		}
	}
	return ret;
}
