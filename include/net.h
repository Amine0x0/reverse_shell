#ifndef NET_H
#define NET_H

#define SERVER_IP   "127.0.0.1"
#define SERVER_PORT 4444
#define BUFFER_SIZE 1024

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

void establish(void);


#endif