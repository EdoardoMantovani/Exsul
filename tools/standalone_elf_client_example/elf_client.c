/* Made by Edoardo Mantovani, 2026
 * Simple ipc application which interacts with the standalone ELF driver.
 */
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
#include <termios.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <asm-generic/errno-base.h>

/* remember to change those parameters accordingly to the main Exsul source code. */
#define MAX_CONNECTION  1
#define BUFFER_SIZE     1024
#define SOCKET_PATH	"/tmp/exsul_socket"

#define u8		uint8_t
#define u16		uint16_t
#define u32		uint32_t

/* protocol format for the standalone ELF ipc */
struct exsul_protocol_hdr {
	u8 type;
	u8 unused;
	u16 len;
	u8  data[BUFFER_SIZE];
};

/* protocol types FOR 'exsul_protocol_hdr' */
enum {
	STANDALONE_TCC_SCRIPTING = 1,
	STANDALONE_CONSOLE_MGMT  = 2,
};

struct termios orig_termios;

void disable_raw_mode(void) {
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

int enable_raw_mode(void) {
	int ret = 0;
	struct termios raw = {};

	ret = tcgetattr(STDIN_FILENO, &orig_termios);
	if(ret < 0){
		ret = -EIO;
		return ret;
	}

	atexit(disable_raw_mode);
	raw = orig_termios;
	raw.c_lflag &= ~(ICANON | ECHO);
	raw.c_cc[VMIN] = 1;
	raw.c_cc[VTIME] = 0;

	ret = tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
	return ret;
}

int main(int argc, char *argv[]){
	int ret = 0;
	int sock_server = 0;
	int current_line_len = 0;
	unsigned char *memarea = NULL;
	unsigned char *current_line = NULL;
	unsigned char *command_mode = NULL;
	struct sockaddr_un addr_server = {};
	struct exsul_protocol_hdr protocol_pkt = {};

	command_mode = argv[1];
	if(command_mode == NULL || strstr(command_mode, "tcc") == NULL || strstr(command_mode, "console") == NULL){
		printf("[!] usage ./%s <tcc|console>\n", argv[0]);
		ret = -EINVAL;
		goto end;
	}

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

	memset(&protocol_pkt, 0, sizeof(struct exsul_protocol_hdr));
	if(strstr(command_mode, "tcc") != NULL){
		protocol_pkt.type = STANDALONE_TCC_SCRIPTING;
	}else if(strstr(command_mode, "console") != NULL){
		protocol_pkt.type = STANDALONE_CONSOLE_MGMT;
	}

	printf("[*] starting the interactive shell, press ctrl + D for sending the message to the standalone ELF!\n");
	memset(protocol_pkt.data, 0, BUFFER_SIZE);

	fflush(stdout);
	fflush(stderr);

	while(true){
		ret = getline(&current_line, &current_line_len, stdin);
		if(ret < 0) {
			free(current_line);
			break;
		}
		memarea = realloc(memarea, (memarea ? strlen(memarea) : 0) + current_line_len + sizeof("\n"));
		if (memarea == NULL) {
			free(current_line);
			goto end;
		}
		strcat(memarea, current_line);
		free(current_line);
		current_line = NULL;
	}

	end:
	if(sock_server > 0){
		close(sock_server);
	}
	return ret;
}
