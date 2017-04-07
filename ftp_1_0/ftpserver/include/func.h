#ifndef _FUNC_H_
#define _FUNC_H_

#include "dbutil.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/uio.h>
#include <string.h>
#include <sys/socket.h>
#include <strings.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/sendfile.h>
#include <dirent.h>
#include <grp.h>
#include <pwd.h>
#include <time.h>

typedef struct{
	pid_t pid;
	int fdw;//管道的写端
	short busy;//判断进程是否忙碌
}cdata,*pcdata;

typedef struct{
	int len;//控制数据
	char buf[1000];//内容存储
}train,*ptrain;

#define FILENAME "file1"
#define IP_CONF "ip"
#define PORT_CONF "port"
#define PNUM_CONF "pnum"
int hand_request(int, char*);
void send_fd(int fdw,int fd, char *ip, int port);
void recv_fd(int fdr,int* fd, char *ip_port);
int send_n(int sfd,void* buf,int len);
int sendfile_n(int sfd, int fd, off_t *buf, off_t len);
int recv_n(int sfd,void* buf,int len);
void make_child(pcdata p,int n);
void child_handle(int fdr);
void myls(char *lspath, char *buf);

#endif
