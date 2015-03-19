#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>

#define BUFFER_SIZE 256

int main(int argc, char *argv[])
{
	FILE * file;
	int sock, sz, szpart, exist;
	char s[170], buffer[BUFFER_SIZE];
	struct sockaddr_in serv_addr;
	
	/* args check */
	if (argc != 3) {
		printf("Args: host port.\n");
		return 0;
	}

	/* * socket * */
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1) {
		printf("Socket creation error.\n");
		return 0;
	}

	/* serv_addr */
	memset(&serv_addr, 0, sizeof(struct sockaddr_in));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
        serv_addr.sin_port = htons(atoi(argv[2]));

	/* bind */
	if (connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1) {
		printf("Socket connection error.\n");
		close(sock);
		return 0;
	}

	/* * interaction * */
	/* send path */
	printf("Input file name : ");
	scanf("%s", s);
	sz = strlen(s) + 1;
	write(sock, &sz, sizeof(int));
	write(sock, s, sz);

	/* recieve file */
	read(sock, &exist, sizeof(int));
	if (exist) {
		file = fopen(s, "wb");
		do {
			szpart = read(sock, buffer, BUFFER_SIZE);
			fwrite(buffer, sizeof(char), szpart, file);
		} while (szpart > 0);
		/* close */
		close(file);
	} else {
		printf("Requested file not found on server.\n");
	}
	close(sock);

	return 0;
}
