#ifndef _FUNC_H_
#define _FUNC_H_

#include <sys/types.h>         
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <crypt.h>
#include <sys/sendfile.h>
#define IP_CONF "ip"
#define PORT_CONF "port"

#define MAXBUFSIZE 10000
int send_n(int sfd,void* buf,int len);
int recv_n(int sfd,void* buf,int len);
int sendfile_n(int sfd, int fd, off_t *buf, off_t len);
void recv_file(int sfd, char *filename);
int send_file(int sfd, char *filename);

typedef struct{
    int len;//控制数据
    char buf[MAXBUFSIZE];//内容存储
}train,*ptrain;

#endif
