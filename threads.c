#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>

#define BUFFER_SIZE 256

static void * routine(void * args)
{
	char buffer[BUFFER_SIZE];
	int sock = *(int *)args;
	int szpath, szpart = 0, exist;
	char * spath;
	FILE * file;

	printf("Client conected. sockfd : %d\n", sock);

	/* recieve path */
	read(sock, &szpath, sizeof(int));
	spath = calloc(szpath, sizeof(char));
	read(sock, spath, szpath);
	printf("Requested file : %s\n", spath);

	/* read and send file */
	file = fopen(spath, "rb");
	if (file) {
		exist = 1;
		write(sock, &exist, sizeof(int));
		while ((szpart = fread(buffer, sizeof(char), BUFFER_SIZE, file)) > 0) {
			write(sock, buffer, szpart);
		}
		fclose(file);
	} else {
	/* file not found */
		exist = 0;
		write(sock, &exist, sizeof(int));
		printf("Requested file not found.\n");
	}

	/* clean */
	free(spath);
	free(args);
	close(sock);
	return 0;
}

static int create_tcp_socket(char * port)
{
	int sock, optval = 1;
	struct sockaddr_in serv_addr;

	/* socket */
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1) {
		printf("Socket creation error.\n");
		return -1;
	}

	/* serv_addr */
	memset(&serv_addr, 0, sizeof(struct sockaddr_in));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(atoi(port));
	serv_addr.sin_addr.s_addr= htonl(INADDR_ANY);

	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval) == -1) {
		printf("setsockopt error.\n");
		return -1;
	}
	/* bind */
	if (bind(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1) {
		printf("Socket bindind error.\n");
		return -1;
	}
	
	/* listen */
	listen(sock, 5);
	
	return sock;
}

int main(int argc, char *argv[])
{
	int sock;

	if (argc != 2) {
		printf("Args: port.\n");
		return 0;
	}

	/* socket bind listen */
	sock = create_tcp_socket(argv[1]);
	if (sock == -1)
		return 1;
	
	for (;;) {
		/* accept */
		int * pclient_sock = (int *)malloc(sizeof(int));
		struct sockaddr_in client_addr;
		pthread_t pt;
		socklen_t client_len = sizeof(client_addr);
		
		*pclient_sock = accept(sock, (struct sockaddr *)&client_addr, &client_len);
		if (*pclient_sock == -1) {
			printf("Socket accept error.\n");
		}

		/* interaction */
#ifdef THREADS
		pthread_create(&pt, 0, routine, pclient_sock);
#else
		switch (fork()) {
		case -1 :
			printf("Fork error!\n");
			break;
		case  0 :
			routine(pclient_sock);
			break;
		default :
			close(*pclient_sock);
			free(pclient_sock);
			break;
		}
#endif
	}

	/* close */
	close(sock);

	return 0;
}
