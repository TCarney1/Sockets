//
// Created by Tom Carney on 28/8/20.
//


#ifndef SOCKETS_HEADER_H
#define SOCKETS_HEADER_H


#define PORT_NUM 80
#define BUFF_SIZE 100

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <stdbool.h>


void make_file_path(char *file_name, char *arg0, char *arg1);
int give_forty(int client_socket, FILE* fp);
void ensure_compiled(char *file_name, char *arg0, char *arg1, struct stat st);




#endif //SOCKETS_HEADER_H
