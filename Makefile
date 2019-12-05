all: DUMBserver.c DUMBclient.c
	gcc DUMBserver.c -o DUMBserve -lpthread
	gcc DUMBclient.c -o DUMBclient

serve: DUMBserver.c
	gcc DUMBserver.c -o DUMBserve -lpthread

client: DUMBclient.c
	gcc DUMBclient.c -o DUMBclient

clean:
	rm -f DUMBserve
	rm -f DUMBclient
