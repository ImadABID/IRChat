CFLAGS=-Wall
#LDFLAGS=-lpthread
all: client_corr server_corr

client_corr : client.c socket_IO
	gcc $(CFLAGS) -o client client.c socket_IO.o 

server_corr : server.c client_list socket_IO
	gcc $(CFLAGS) -o server server.c client_list.o socket_IO.o 


client_list : client_list.c client_list.h
	gcc $(CFLAGS) -o client_list.o -c client_list.c

socket_IO : socket_IO.c socket_IO.h
	gcc $(CFLAGS) -o socket_IO.o -c socket_IO.c

clean:
	rm *.o
	rm -f client_corr server_corr
