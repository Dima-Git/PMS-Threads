#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>

int main(int argc, char *argv[])
{
	int sock;
	char s[70], * sfile;
	int sz, szfile;
	struct sockaddr_in serv_addr;
	
	/* socket */
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
	
	/* interaction */
	/* send path */
	scanf("%s", s);
	sz = strlen(s) + 1;
	write(sock, &sz, sizeof(int));
	write(sock, s, sz);
	
	/* recieve file */
	read(sock, &szfile, sizeof(int));
	if (szfile != -1) {
		sfile = (char *)malloc(szfile);
		read(sock, sfile, szfile);
		printf("File :\n%s", sfile);
		free(sfile);
	} else {
		printf("No file.\n");
	}
	
	/* close */
	close(sock);
	
	return 0;
}
