# Variables 
CC = gcc
CFLAGS = -Wall -Werror -I/usr/include/tirpc

#SERVIDOR y CLIENTE DE RPC
SERVER_SOURCES = ./server/servidor.c ./server/list.c ./server/lines.c
SERVER_OBJECTS = $(SERVER_SOURCES:.c=.o)
SERVER = servidor
TARGETS_CLNT = ./server_rpc/mensaje_clnt.c ./server_rpc/mensaje_xdr.c
OBJECTS_CLNT = $(TARGETS_CLNT:%.c=%.o)

#SERVIDOR RPC
TARGETS_SVC = ./server_rpc/mensaje_svc.c ./server_rpc/server_rpc.c 
OBJECTS_SVC = $(TARGETS_SVC:%.c=%.o)
SERVER_RPC = servidor_rpc

all: $(SERVER) $(SERVER_RPC)

$(OBJECTS_CLNT) :  %.o: %.c
	$(CC) $(CFLAGS) -c $^ -o $@

$(SERVER): $(SERVER_OBJECTS) $(OBJECTS_CLNT)
	$(CC) $(CFLAGS) -o $(SERVER) $(SERVER_OBJECTS) $(OBJECTS_CLNT) -lrt -lpthread -lnsl -ltirpc


$(OBJECTS_SVC):  %.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(SERVER_RPC): $(OBJECTS_SVC)
	$(CC) $(CFLAGS) -o $(SERVER_RPC) $(OBJECTS_SVC) ./server_rpc/mensaje_xdr.o -lrt -lpthread -lnsl -ltirpc

runs:
	./servidor

runrpc: 
	./servidor_rpc

fclean: clean
	rm -f $(SERVER) $(SERVER_RPC)
clean:
	rm -f $(SERVER_OBJECTS) $(OBJECTS_CLNT) $(OBJECTS_SVC)

re: fclean all
