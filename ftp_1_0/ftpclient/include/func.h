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
#define IP_CONF "ip"
#define PORT_CONF "port"

int send_n(int sfd,void* buf,int len);
int recv_n(int sfd,void* buf,int len);
void recv_file(int sfd,  char *filename);

typedef struct{
    int len;//控制数据
    char buf[1000];//内容存储
}train,*ptrain;

#endif
