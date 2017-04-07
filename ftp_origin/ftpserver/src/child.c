#include "func.h"

void child_handle(int fdr)
{
	short flag=1;
	int new_fd;
	signal(SIGPIPE,SIG_IGN);//设置为忽略
	while(1)
	{
		recv_fd(fdr,&new_fd);
		hand_request(new_fd);//发送文件函数
		write(fdr,&flag,sizeof(flag));
	}
}

void make_child(pcdata p,int n)
{
	int fds[2];
	int i;
	int pid;
	for(i=0;i<n;i++)
	{
		socketpair(AF_LOCAL,SOCK_STREAM,0,fds);//新建一条管道
		pid=fork();
		if(0==pid)
		{
			close(fds[1]);
			child_handle(fds[0]);		
		}
		close(fds[0]);
		p[i].pid=pid;
		p[i].fdw=fds[1];
		p[i].busy=0;//标示为非忙碌
	}
}
