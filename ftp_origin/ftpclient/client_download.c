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
	off_t filesize;
	off_t filesize_tmp;
	//先接收文件名
	bzero(buf,sizeof(buf));
	ret=recv(sfd,&len,sizeof(len),0);
	recv(sfd,buf,len,0);

	recv_n(sfd, &filesize, sizeof(filesize));

	int fd;
	fd=open(buf,O_CREAT|O_RDWR|O_EXCL,0600);
	struct stat filestat;
	if(-1==fd)
	{
		fd = open(buf, O_RDWR|O_APPEND);
		if(-1 == fd){
			perror("open 2");
		}
		fstat(fd, &filestat);
		//文件已经存在
		if(filesize == filestat.st_size){
			//文件已经下载完毕
			printf("文件已经下载完毕\n");
			filesize_tmp = 0;
			send_n(sfd, &filesize_tmp, sizeof(filesize_tmp));
		}else{
			//文件还未下载完毕，从文件尾部开始下载
			printf("文件还未下载完毕，需继续下载\n");
			filesize_tmp = filestat.st_size;
			send_n(sfd, &filesize_tmp, sizeof(filesize_tmp));
			fd = open(buf, O_RDWR|O_APPEND);
			if(-1 == fd){
				perror("open 2");
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
		}		
		
	}else{
		//文件不存在，正常下载流程，从头下载
		printf("文件不存在，正常流程从头开始下载\n");
		filesize_tmp = 1;
		send_n(sfd, &filesize_tmp, sizeof(filesize_tmp));
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
	}
	close(fd);
	close(sfd);
}
