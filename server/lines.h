#ifndef LINES_H
#define LINES_H

#include <unistd.h>
#include <errno.h>

int sendMessage(int socket, char * buffer, int len);
int recvMessage(int socket, char *buffer, int len);
ssize_t readLine(int fd, void *buffer, size_t n);

#endif
