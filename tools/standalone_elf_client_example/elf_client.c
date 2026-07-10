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
	u8 len_hi;
	u16 len;
};

static inline void exsul_protocol_set_len(struct exsul_protocol_hdr *hdr,
					  u32 len)
{
	hdr->len_hi = (u8)((len >> 16) & 0xff);
	hdr->len = (u16)(len & 0xffff);
}

/* protocol types FOR 'exsul_protocol_hdr' */
enum {
	STANDALONE_TCC_SCRIPTING = 1,
	STANDALONE_CONSOLE_MGMT  = 2,
};

struct termios orig_termios;

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
			ret = -EIO;
			return ret;
		}
		sent += (size_t)ret;
	}

	return ret;
}

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
	raw.c_cc[VMIN]  = 1;
	raw.c_cc[VTIME] = 0;

	ret = tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
	return ret;
}

int main(int argc, char *argv[]){
	int ret = 0;
	int total_len = 0;
	int sock_server = 0;
	unsigned char  current_c = 0;
	unsigned char *memarea = NULL;
	unsigned char *tmp_buffer = NULL;
	char *command_mode = NULL;
	unsigned char *packet = NULL;
	unsigned char *payload = NULL;
	struct sockaddr_un addr_server = {};
	struct exsul_protocol_hdr *protocol_pkt = NULL;

	(void)argc;
	command_mode = argv[1];
	if(command_mode == NULL){
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

	packet = calloc(sizeof(struct exsul_protocol_hdr) + BUFFER_SIZE, sizeof(unsigned char));
	if(packet == NULL){
		ret = -ENOMEM;
		goto end;
	}
	protocol_pkt = (struct exsul_protocol_hdr *)packet;
	payload = packet + sizeof(struct exsul_protocol_hdr);

	if(strstr(command_mode, "tcc") != NULL){
		protocol_pkt->type = STANDALONE_TCC_SCRIPTING;
	}else if(strstr(command_mode, "console") != NULL){
		protocol_pkt->type = STANDALONE_CONSOLE_MGMT;
	}

	/* enable raw terminal mode for reading the CTRL + E sequence */
	ret = enable_raw_mode();
	if(ret < 0){
		printf("[!] failed to init raw mode!\n");
		ret = -EIO;
		goto end;
	}

	printf("[*] starting the interactive shell, press ctrl + E for sending the message to the standalone ELF!\n");
	memset(payload, 0, BUFFER_SIZE);

	fflush(stdout);
	fflush(stderr);

	while(true){
		current_c = 0;
		ret = read(STDIN_FILENO, &current_c, 1);
		if (ret <= 0){
			continue;
		}
		/* Ctrl + E is '0x05', send the buffer to the userspace driver through IPC */
		if (current_c == 0x05) {
			if (total_len > BUFFER_SIZE){
				total_len = BUFFER_SIZE;
			}
			memset(payload, 0, BUFFER_SIZE);
			if(total_len > 0){
				memcpy(payload, memarea, total_len);
			}
			exsul_protocol_set_len(protocol_pkt, (u32)total_len);
			send_all(sock_server, packet, sizeof(struct exsul_protocol_hdr) + (size_t)total_len);
		}

		/* handle cancellation of a character */
		if (current_c == 0x7f || current_c == 0x08) {
			if (total_len > 0) {
				total_len--;
				memarea[total_len] = '\0';
				write(STDOUT_FILENO, "\b \b", 3);
			}
			continue;
		}

		tmp_buffer = realloc(memarea, total_len + 1);
		if (tmp_buffer == NULL) {
			free(memarea);
			ret = -ENOMEM;
			return ret;
		}
		memarea = tmp_buffer;
		memarea[total_len++] = current_c;
		write(STDOUT_FILENO, &current_c, 1);
	}

	end:
	if(packet != NULL){
		free(packet);
	}
	if(memarea != NULL){
		free(memarea);
	}
	if(sock_server > 0){
		close(sock_server);
	}
	return ret;
}
