#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

static void * routine(void * args)
{
	int sock = *(int *)args;
	int szpath, szfile = -1, i = 0;
	char * spath, * sfile, c;
	FILE * file;
	
	/*sleep(3);*/
	printf("Child routine. Args : %d\n", sock);
	
	/* recieve path */
	read(sock, &szpath, sizeof(int));
	printf("Given size : %d\n", szpath);
	spath = calloc(szpath, sizeof(char));
	read(sock, spath, szpath);
	printf("Given string : %s\n", spath);
	
	/* read file */
	file = fopen(spath, "r");
	if (file) {
		fseek(file, 0, SEEK_END);
		szfile = ftell(file);
		rewind(file);
		sfile = (char *)calloc(szfile + 1, sizeof(char));
		printf("Size : %d\n", szfile);
		while (fscanf(file, "%c", &c) == 1) {
			sfile[i] = c;
			++ i;
		}
	} else {
	}
	/* send file */
	write(sock, &szfile, sizeof(int));
	write(sock, sfile, szfile);
	
	/* clean */
	if (file) {
		free(sfile);
		fclose(file);
	}
	free(spath);
	free(args);
	close(sock);
	return 0;
}

static int create_named_local_socket(char * name)
{
	int sock;
	struct sockaddr_un serv_addr;
	
	/* socket */
	sock = socket(AF_LOCAL, SOCK_STREAM, 0);
	if (sock == -1) {
		printf("Socket creation error.\n");
		return -1;
	}
	
	/* serv_addr */
	memset(&serv_addr, 0, sizeof(struct sockaddr_un));
	serv_addr.sun_family = AF_LOCAL;
	strncpy(serv_addr.sun_path, name, sizeof(serv_addr.sun_path) - 1);
	
	/* bind */
	if (bind(sock, (struct sockaddr *) &serv_addr, SUN_LEN(&serv_addr)) == -1) {
		printf("Socket bindind error.\n");
		return -1;
	}
	
	/* listen */
	listen(sock, 5);
	
	return sock;
}

int main(int argc, char *argv[])
{
	char * name = "thesocketname";
	int sock, i;
	
	/* socket bind listen */
	sock = create_named_local_socket(name);
	if (sock == -1)
		return 1;
	
	for (i = 0; i < 3; ++ i) {
		/* accept */
		int * pclient_sock = (int *)malloc(sizeof(int));
		struct sockaddr_un client_addr;
		pthread_t pt;
		int p;
		
		socklen_t client_len = sizeof(client_addr);
		*pclient_sock = accept(sock, (struct sockaddr *)&client_addr, &client_len);
		if (*pclient_sock == -1) {
			printf("Socket accept error.\n");
		}
		
		/* interaction */
#ifdef THREADS
		p = pthread_create(&pt, 0, routine, pclient_sock);
#else
		switch (fork()) {
		case -1 :
			printf("Fork error!\n");
			break;
		case  0 :
			routine(pclient_sock);
			return 0;
		default :
			break;
		}
#endif
	}
	
	/* close & unlink */
	close(sock);
	unlink(name);
	
	return 0;
}
