all:
	gcc threads.c -o threads.out
	gcc client.c -o client.out

threads:
	gcc threads.c -pthread -DTHREADS -o threads.out
	gcc client.c -o client.out

clean:
	rm -f threads.out
	rm -f client.out
