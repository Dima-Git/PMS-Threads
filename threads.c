#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	for (;;) {
		int x = 0;
		scanf("%d", &x);
		#ifdef THREADS
		printf("Threads.\n");
		#else
		if (x == 0) {
			break;
		} else {
			switch (fork()) {
			case -1 :
				printf("Fork error!\n");
				break;
			case  0 :
				sleep(3);
				printf("Child process.\n");
				return 0;
			default :
				printf("Main process.\n");
			}
		}
		#endif
	}
	return 0;
}
