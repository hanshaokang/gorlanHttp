#ifndef _HTTPD_H_
#define _HTTPD_H_

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/sendfile.h>
#define SIZE 1024
#define SUCCESS 0
#define NOTICE  1
#define WARNING 2
#define ERROR   3
#define FATAL   4
void echo_www(int fd,char *path,int size);
static void clear_fdhead(int fd);
static void request_404(int sock);
static void echo_client(int nums);
int  startup( const char *ip,int port);
static int get_line(int fd,char buf[],int len);
static int exec_cgi(int sock,char *method,char *path,char *query_strin);
void* handler_request(void*arg);
#endif
