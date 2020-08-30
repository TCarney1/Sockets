//
// Created by Tom Carney on 28/8/20.
//


#ifndef SOCKETS_HEADER_H
#define SOCKETS_HEADER_H

#ifdef _WIN32
#define TYPE 0
#endif

#ifdef __APPLE__
#define TYPE 1
#endif

#ifdef __unix__
#define TYPE 2
#endif

#define PORT_NUM 80
#define BUFF_SIZE 100

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#include <sys/sysctl.h>
#include <errno.h>
#include <stdbool.h>


int print_forty(int server_socket);




#endif //SOCKETS_HEADER_H
