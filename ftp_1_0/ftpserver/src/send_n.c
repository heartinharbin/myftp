#include "func.h"

//准确发送len个字节到达对方
int send_n(int sfd,void* buf,int len)
{
	int total=0;
	int ret;
	while(total<len)
	{
		ret=send(sfd,buf+total,len-total,0);
		if(-1==ret)
		{
			printf("client close\n");
			return -1;
		}
		total=total+ret;
	}
	return 0;
}

int sendfile_n(int sfd, int fd, off_t *buf, off_t len){
	off_t total = 0;
	int ret;
	while(total < len){
		ret = sendfile(sfd, fd, buf, len-total);
		if(-1 == ret){
			printf("client close\n");
			return -1;	
		}
		total += ret;
	}
	return 0;
}

int recv_n(int sfd,void* buf,int len)
{
	int total=0;
	int ret;
	while(total<len)
	{
		ret=recv(sfd,buf+total,len-total,0);
		total=total+ret;
	}
	return 0;
}
