CFLAGS=-Wall
LDFLAGS=-lpthread
all: client server

client : client.c msg_IO.o req_reader.o file_transfer.o
	gcc $(CFLAGS) -o client client.c msg_IO.o req_reader.o file_transfer.o $(LDFLAGS)

server : server.c client_list.o msg_IO.o salon_list.o
	gcc $(CFLAGS) -o server server.c client_list.o salon_list.o msg_IO.o $(LDFLAGS)

client_list.o : client_list.c client_list.h msg_struct.h
	gcc $(CFLAGS) -o client_list.o -c client_list.c

salon_list.o : salon_list.c salon_list.h
	gcc $(CFLAGS) -o salon_list.o -c salon_list.c

msg_IO.o : msg_IO.c msg_IO.h msg_struct.h
	gcc $(CFLAGS) -o msg_IO.o -c msg_IO.c

req_reader.o : req_reader.c req_reader.h
	gcc $(CFLAGS) -o req_reader.o -c req_reader.c

file_transfer.o : file_transfer.c file_transfer.h
	gcc $(CFLAGS) -o file_transfer.o -c file_transfer.c

clean:
	rm *.o
	rm -f client_corr server_corr
