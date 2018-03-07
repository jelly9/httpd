#ifndef __HTTPD_H__ 
#define __HTTPD_H__

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>

#define SUCCESS 0
#define NOTICE  1
#define WARNING 2
#define ERROR   3
#define FATAL   4

int startup(const char *ip_addr, int port);
void print_log(const char *msg, int level);
void *handler_request(void *arg);

#endif
