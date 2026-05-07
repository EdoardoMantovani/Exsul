/* Made by Edoardo Mantovani, 2026 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <poll.h>
#include <signal.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <asm-generic/errno-base.h>

/* remember to change those parameters accordingly to the main Exsul source code. */
#define MAX_CONNECTION  1
#define BUFFER_SIZE     1024 * 32
#define SOCKET_PATH	"/tmp/exsul_socket"

#define u8		uint8_t
#define u16		uint16_t
#define u32		uint32_t

#define s8		int8_t
#define s16		int16_t
#define s32		int32_t

/* protocol format for the standalone ELF ipc */
struct exsul_protocol_hdr {
	u8 type;
	u8 unused;
	u16 len;
};

/* protocol types FOR 'exsul_protocol_hdr' */
enum {
	STANDALONE_TCC_SCRIPTING = 1,
	STANDALONE_CONSOLE_MGMT  = 2,
};

static int send_all(int fd, const void *buffer, size_t length){
	int ret = 0;
	size_t sent = 0;
	const char *cursor = buffer;

	while(sent < length){
		ret = send(fd, cursor + sent, length - sent, 0);
		if(ret < 0){
			return ret;
		}
		if(ret == 0){
			return -EIO;
		}
		sent += (size_t)ret;
	}

	return 0;
}

int main(int argc, char *argv[]){
	int ret = 0;
	int numbytes = 0;
	int sock_server = 0;
	int packet_len = 0;
	FILE *infile = NULL;
	char *buffer = NULL;
	char *script_file = NULL;
	unsigned char *packet = NULL;
	unsigned char *payload = NULL;
	struct sockaddr_un addr_server = {};
	struct exsul_protocol_hdr *protocol_pkt = NULL;

	script_file = argv[1];
	if(script_file == NULL){
		printf("[!] usage ./%s <script_file.c>\n", argv[0]);
		ret = -EINVAL;
		goto end;
	}

	infile = fopen(script_file, "r");
	if(infile == NULL){
		ret = -EINVAL;
		return ret;
 	}

	fseek(infile, 0, SEEK_END);
	numbytes = ftell(infile);
	fseek(infile, 0, SEEK_SET);
	if(numbytes < 0 || numbytes > BUFFER_SIZE){
		printf("[!] script too large, maximum payload is %d bytes\n", BUFFER_SIZE);
		ret = -EINVAL;
		fclose(infile);
		goto end;
	}
	buffer = (char *)calloc(numbytes + 1, sizeof(char));
	if(buffer == NULL){
		fclose(infile);
		ret = -ENOMEM;
		goto end;
 	}
	fread(buffer, sizeof(char), numbytes, infile);
	fclose(infile);

	/* connect to the server */
	sock_server = socket(AF_UNIX, SOCK_STREAM, 0);
	if(sock_server < 0){
		printf("[!] failed to open socket!\n");
		ret = -EIO;
		goto end;
	}
	memset(&addr_server, 0, sizeof(struct sockaddr_un));
	addr_server.sun_family = AF_UNIX;
	strncpy(addr_server.sun_path, SOCKET_PATH, sizeof(addr_server.sun_path) - 1);

	ret = connect(sock_server, (struct sockaddr*)&addr_server, sizeof(addr_server));
	if(ret < 0){
		printf("[!] failed to connect to the standalone elf!\n");
		ret = -EIO;
		goto end;
	}

	packet_len = sizeof(struct exsul_protocol_hdr) + (size_t)numbytes;
	packet = calloc(packet_len, sizeof(unsigned char));
	if(packet == NULL){
		ret = -ENOMEM;
		goto end;
	}

	protocol_pkt = (struct exsul_protocol_hdr *)packet;
	payload = packet + sizeof(struct exsul_protocol_hdr);
	protocol_pkt->type = STANDALONE_TCC_SCRIPTING;
	protocol_pkt->len = (u16)numbytes;
	memcpy(payload, buffer, (size_t)numbytes);
	ret = send_all(sock_server, packet, packet_len);

	end:
	if(packet != NULL){
		free(packet);
	}
	if(buffer != NULL){
		free(buffer);
	}
	if(sock_server > 0){
		close(sock_server);
	}
	return ret;
}
