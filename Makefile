# Variables 
CC = gcc
CFLAGS = -Wall -Werror -I/usr/include/tirpc

#SERVIDOR y CLIENTE DE RPC
SERVER_SOURCES = servidor.c list.c lines.c
SERVER_OBJECTS = $(SERVER_SOURCES:.c=.o)
SERVER = servidor
TARGETS_CLNT = mensaje_clnt.c mensaje_xdr.c
OBJECTS_CLNT = $(TARGETS_CLNT:%.c=%.o)

#SERVIDOR RPC
TARGETS_SVC = mensaje_svc.c server_rpc.c 
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
	$(CC) $(CFLAGS) -o $(SERVER_RPC) $(OBJECTS_SVC) mensaje_xdr.o -lrt -lpthread -lnsl -ltirpc

runs:
	./servidor

fclean: clean
	rm -f $(SERVER) $(SERVER_RPC)
clean:
	rm -f $(SERVER_OBJECTS) $(OBJECTS_CLNT) $(OBJECTS_SVC)

re: fclean all
