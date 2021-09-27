CFLAGS=-Wall
#LDFLAGS=-lpthread
all: client_corr server_corr

client_corr : client.c
	gcc $(CFLAGS) -o client client.c

server_corr :
	gcc $(CFLAGS) -o server server.c

clean:
	rm -f client_corr server_corr
