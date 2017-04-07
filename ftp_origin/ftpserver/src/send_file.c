#include "func.h"
//发送文件函数
void hand_request(int new_fd)
{
	train t;//定义小火车
	//第一步传输文件名
	bzero(&t,sizeof(t));
	t.len=strlen(FILENAME);
	strcpy(t.buf,FILENAME);
	send(new_fd,&t,4+t.len,0);//发送文件名到客户端
	//发送文件内容
	int fd;
	fd=open(FILENAME,O_RDWR);
	if(-1==fd)
	{
		perror("open");
		close(new_fd);
		return;
	}
	int ret;
	//循环读取文件内容，并发送
	while((t.len=read(fd,t.buf,sizeof(t.buf)))>0)
	{
		ret=send_n(new_fd,&t,4+t.len);
		if(-1==ret)
		{
			goto end;
		}
	}
	//发送结束
	t.len=0;
	send(new_fd,&t.len,4,0);//发送结束符
end:	
	close(fd);
	close(new_fd);
}
