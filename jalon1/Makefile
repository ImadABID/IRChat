CFLAGS=-Wall
#LDFLAGS=-lpthread
all: client_corr server_corr

client_corr : client.c
	gcc $(CFLAGS) -o client client.c

server_corr : server.c client_list
	gcc $(CFLAGS) -o server server.c client_list.o


client_list : client_list.c client_list.h
	gcc $(CFLAGS) -o client_list.o -c client_list.c

clean:
	rm *.o
	rm -f client_corr server_corr
