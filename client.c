#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>

int main(int argc, char *argv[])
{
	char * name = "thesocketname";
	int sock;
	char s[70];
	int sz;
	struct sockaddr_un serv_addr;
	
	/* socket */
	sock = socket(AF_LOCAL, SOCK_STREAM, 0);
	if (sock == -1) {
		printf("Socket creation error.\n");
		return 0;
	}
	
	/* serv_addr */
	memset(&serv_addr, 0, sizeof(struct sockaddr_un));
	serv_addr.sun_family = AF_LOCAL;
	strncpy(serv_addr.sun_path, name, sizeof(serv_addr.sun_path) - 1);
	
	/* bind */
	if (connect(sock, (struct sockaddr *) &serv_addr, SUN_LEN(&serv_addr)) == -1) {
		printf("Socket connection error.\n");
		close(sock);
		return 0;
	}
	
	/* interaction */
	scanf("%s", s);
	sz = strlen(s) + 1;
	write(sock, &sz, sizeof(int));
	write(sock, s, sz);
	
	/* close */
	close(sock);
	
	return 0;
}
