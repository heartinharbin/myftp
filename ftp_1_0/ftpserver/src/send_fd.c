#include "func.h"
void send_fd(int fdw,int fd, char *ip, int port)
{
	struct msghdr msg;
	bzero(&msg,sizeof(msg));
	struct iovec iov[3];
	char buf0[30] = {0};
	char buf1[10] = {0};
	char buf2[10] = {0};
	strcpy(buf0, ip);
	iov[0].iov_base=buf0;
	iov[0].iov_len=strlen(buf0);
	
	strcpy(buf1, " : ");
	iov[1].iov_base=buf1;
	iov[1].iov_len=strlen(buf1);
	
	sprintf(buf2, "%d", port);
	iov[2].iov_base=buf2;
	iov[2].iov_len=strlen(buf2);
	msg.msg_iov=iov;
	msg.msg_iovlen=3;
	struct cmsghdr *cmsg;
	int len=CMSG_LEN(sizeof(int));
	cmsg=(struct cmsghdr *)calloc(1,len);
	cmsg->cmsg_len=len;
	cmsg->cmsg_level=SOL_SOCKET;
	cmsg->cmsg_type=SCM_RIGHTS;
	*(int*)CMSG_DATA(cmsg)=fd;
	msg.msg_control=cmsg;
	msg.msg_controllen=len;
	int ret;
	ret=sendmsg(fdw,&msg,0);
	if(-1==ret)
	{
		perror("sendmsg");
		return;
	}
}
void recv_fd(int fdr,int* fd, char *ip_port)
{
	struct msghdr msg;
	bzero(&msg,sizeof(msg));
	struct iovec iov[2];
	char buf[30]={0};

	iov[0].iov_base=buf;
	iov[0].iov_len=sizeof(buf);
	msg.msg_iov=iov;
	msg.msg_iovlen=1;
	struct cmsghdr *cmsg;
	int len=CMSG_LEN(sizeof(int));
	cmsg=(struct cmsghdr *)calloc(1,len);
	cmsg->cmsg_len=len;
	cmsg->cmsg_level=SOL_SOCKET;
	cmsg->cmsg_type=SCM_RIGHTS;
	msg.msg_control=cmsg;
	msg.msg_controllen=len;
	int ret;
	ret=recvmsg(fdr,&msg,0);
	if(-1==ret)
	{
		perror("recvmsg");
		return;
	}
	*fd=*(int*)CMSG_DATA(cmsg);
	strcpy(ip_port, msg.msg_iov[0].iov_base);
//	printf("ip:port=%s\n", msg.msg_iov[0].iov_base);
}

