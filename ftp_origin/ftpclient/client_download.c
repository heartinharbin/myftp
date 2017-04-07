#include "func.h"

int main(int argc,char* argv[])
{
	if(argc!=3)
	{
		printf("error args\n");
		return -1;
	}
	int sfd;//socket描述符
	sfd=socket(AF_INET,SOCK_STREAM,0);
	if(-1==sfd)
	{
		perror("socket");
		return -1;
	}
	struct sockaddr_in ser;
	memset(&ser,0,sizeof(ser));
	ser.sin_family=AF_INET;
	ser.sin_port=htons(atoi(argv[2]));//将端口号转换为网络字节序
	ser.sin_addr.s_addr=inet_addr(argv[1]);//将点分十进制IP地址转换为网络字节序
	int ret;
	ret=connect(sfd,(struct sockaddr*)&ser,sizeof(ser));
	if(-1==ret)
	{
		perror("connect");
		return -1;
	}
	printf("connect succss\n");
	int len;
	char buf[1000];
	//先接收文件名
	bzero(buf,sizeof(buf));
	ret=recv(sfd,&len,sizeof(len),0);
	recv(sfd,buf,len,0);
	int fd;
	fd=open(buf,O_CREAT|O_RDWR,0600);
	if(-1==fd)
	{
		perror("open");
		close(sfd);
		return -1;
	}
	while(1)
	{
		recv_n(sfd,&len,sizeof(len));//先读4个字节
		if(len>0)
		{
			bzero(buf,sizeof(buf));
			recv_n(sfd,buf,len);
			write(fd,buf,len);
		}else{
			break;
		}
	}
	close(fd);
	close(sfd);
}
