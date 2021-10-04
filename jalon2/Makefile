CFLAGS=-Wall
#LDFLAGS=-lpthread
all: client_corr server_corr

client_corr : client.c msg_IO
	gcc $(CFLAGS) -o client client.c msg_IO.o 

server_corr : server.c client_list msg_IO
	gcc $(CFLAGS) -o server server.c client_list.o msg_IO.o 


client_list : client_list.c client_list.h
	gcc $(CFLAGS) -o client_list.o -c client_list.c

msg_IO : msg_IO.c msg_IO.h
	gcc $(CFLAGS) -o msg_IO.o -c msg_IO.c

clean:
	rm *.o
	rm -f client_corr server_corr
