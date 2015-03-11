#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

static void * routine(void * args)
{
	sleep(3);
	printf("Child thread.\n");
	return 0;
}

int main(int argc, char *argv[])
{
	int x = 0, p = 0;
	pthread_t pt = 0;
	for (;;) {
		scanf("%d", &x);
		if (x == 0) {
			break;
		} else {
		#ifdef THREADS
		p = pthread_create(&pt, 0, routine, 0);
		printf("Threads.\n");
		#else
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
		#endif
		}
	}
	return 0;
}
