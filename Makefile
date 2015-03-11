all:
	gcc threads.c -o threads.out

threads:
	gcc threads.c -pthread -DTHREADS -o threads.out

clean:
	rm -f threads.out
