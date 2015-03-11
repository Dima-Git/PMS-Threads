#include <stdio.h>

int main(int argc, char *argv[])
{
	#ifdef THREADS
	printf("Threads.\n");
	#else
	printf("Processes.\n");
	#endif
	return 0;
}
