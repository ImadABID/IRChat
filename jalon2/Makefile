CFLAGS=-Wall
#LDFLAGS=-lpthread
all: client_corr server_corr

client_corr : client.c msg_IO req_reader
	gcc $(CFLAGS) -o client client.c msg_IO.o req_reader.o

server_corr : server.c client_list msg_IO
	gcc $(CFLAGS) -o server server.c client_list.o msg_IO.o 


client_list : client_list.c client_list.h
	gcc $(CFLAGS) -o client_list.o -c client_list.c

msg_IO : msg_IO.c msg_IO.h
	gcc $(CFLAGS) -o msg_IO.o -c msg_IO.c

req_reader : req_reader.c req_reader.h
	gcc $(CFLAGS) -o req_reader.o -c req_reader.c

clean:
	rm *.o
	rm -f client_corr server_corr
