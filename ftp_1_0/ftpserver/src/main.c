#include "func.h"

void get_value_by_name(char *name, char *value){
	FILE *fp = fopen("../conf/ftp.conf", "r");
	if(NULL == fp){
		perror("open");
		return;
	}
	char n[50] = {0};
	char v[50] = {0};
	while(fscanf(fp, "%s = %s", n, v) != EOF){
//		printf("%s = %s\n", n , v);
		if(strcmp(n, name) == 0){
			strcpy(value,  v);
			break;
		}
	}
	return;
}

int main(int argc,char* argv[])
{
	if(argc!=1)
	{
		printf("error args\n");
		return -1;
	}
	char ipvalue[50];
	char portvalue[50];
	char pnumvalue[50];
	get_value_by_name(IP_CONF, ipvalue);
	get_value_by_name(PORT_CONF, portvalue);
	get_value_by_name(PNUM_CONF, pnumvalue);
//	printf("value=%s\n", value);
	int num=atoi(pnumvalue);//需要创建的进程数量
	pcdata p=(pcdata)calloc(num,sizeof(cdata));
	make_child(p,num);//创建子进程并初始化子进程的数据结构
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
	ser.sin_port=htons(atoi(portvalue));//将端口号转换为网络字节序
	ser.sin_addr.s_addr=inet_addr(ipvalue);//将点分十进制IP地址转换为网络字节序
	int ret;
	ret=bind(sfd,(struct sockaddr*)&ser,sizeof(ser));
	if(ret==-1)
	{
		perror("bind");
		return -1;
	}
	//epoll_ctl注册每一个子进程的管道的另外一端，sfd
	int epfd;
	epfd=epoll_create(1);
	struct epoll_event event,*evs;
	evs=(struct epoll_event*)calloc(num+1,sizeof(struct epoll_event));
	bzero(&event,sizeof(event));
	event.events=EPOLLIN;
	event.data.fd=sfd;
	ret=epoll_ctl(epfd,EPOLL_CTL_ADD,sfd,&event);
	if(-1==ret)
	{
		perror("epoll_ctl");
		return -1;
	}
	int i;
	for(i=0;i<num;i++)
	{
		event.events=EPOLLIN;
		event.data.fd=p[i].fdw;
		epoll_ctl(epfd,EPOLL_CTL_ADD,p[i].fdw,&event);
	}
	ret=listen(sfd,num);
	if(-1==ret)
	{
		perror("listen");
		return -1;
	}
	int new_fd;
	int j;
	short flag;
	struct sockaddr_in clientaddr;
	int clientaddr_len;
	while(1)
	{
		bzero(evs,(num+1)*sizeof(struct epoll_event));
		ret=epoll_wait(epfd,evs,num+1,-1);
		if(ret>0)
		{
			for(i=0;i<ret;i++)
			{
				if(sfd==evs[i].data.fd)
				{
					bzero(&clientaddr, sizeof(clientaddr));
					clientaddr_len = sizeof(clientaddr);
					new_fd=accept(sfd,(struct sockaddr*)&clientaddr, &clientaddr_len);
					
					for(j=0;j<num;j++)
					{
						if(p[j].busy==0)
						{
							printf("process %d will busy\n",p[j].pid);
							send_fd(p[j].fdw,new_fd,inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));//把new_fd传递给空闲的子进程
							p[j].busy=1;
							break;	
						}
					}
					close(new_fd);//关闭以后，因为引用计数为1，实际没有断开连接									
				}
				for(j=0;j<num;j++)
				{
					if(p[j].fdw==evs[i].data.fd)
					{
						read(p[j].fdw,&flag,sizeof(flag));
						printf("process %d is not busy\n",p[j].pid);
						p[j].busy=0;//子进程变为非忙碌	
					}
				}	
			}	
		}
	}
}
