# Variables 
CC = gcc
CFLAGS = -Wall -Werror

#SERVIDOR 
SERVER_SOURCES = servidor.c claves.c list.c lines.c
SERVER_OBJECTS = $(SERVER_SOURCES:.c=.o)
SERVER = servidor


all: $(SERVER)

$(SERVER): $(SERVER_OBJECTS)
	$(CC) $(CFLAGS) -o $(SERVER) $(SERVER_OBJECTS) -lrt -lpthread

runs:
	./servidor

fclean: clean
	rm -f $(SERVER)
clean:
	rm -f $(SERVER_OBJECTS)

re: fclean all
