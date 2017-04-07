#include "func.h"
//发送文件函数
int hand_request(int new_fd, char *filename)
{
	train t;//定义小火车
	//第一步传输文件名
	bzero(&t,sizeof(t));
	t.len=strlen(filename);
	strcpy(t.buf,filename);
	int  len;
	send(new_fd,&t,4+t.len,0);//发送文件名到客户端
	//发送文件内容
	printf("len:%d buf:%s\n", t.len, t.buf);
	int fd;
	fd=open(filename,O_RDWR);
	if(-1==fd)
	{
		perror("open");
		len = -1;
		send_n(new_fd, &len, 4);
//		close(new_fd);
		return -1;
	}else{
		len = 0;
		send_n(new_fd, &len, 4);
	}
	int ret;
	//循环读取文件内容，并发送
/*
	while((t.len=read(fd,t.buf,sizeof(t.buf)))>0)
	{
		ret=send_n(new_fd,&t,4+t.len);
		if(-1==ret)
		{
			goto end;
		}
	}
*/
	//ssize_t sendfile(int out_fd, int in_fd, off_t *offset, size_t count);
	
	struct stat buf;
	fstat(fd, &buf);
	printf("filename:%s buf->st_size:%ld\n", filename, buf.st_size);
	
	off_t filesize = buf.st_size;
	send_n(new_fd, (char*)&filesize, sizeof(filesize));

	sendfile_n(new_fd, fd, 0, buf.st_size);
	


	printf("send end!\n");
	//发送结束
	t.len=0;
//	send(new_fd,&t.len,4,0);//发送结束符
end:	
	close(fd);
//	close(new_fd);
	return 0;
}
