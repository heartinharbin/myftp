#include "func.h"
//发送文件函数
void hand_request(int new_fd)
{
	train t;//定义小火车
	int len;
	char buf[1000];
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

	struct stat filestat;
	fstat(fd, &filestat);
	printf("filename:%s st_size:%ld\n", FILENAME, filestat.st_size);
	
	off_t filesize = filestat.st_size;
	send_n(new_fd, &filesize, sizeof(filesize));
	off_t filesize_c; 
	recv_n(new_fd, &filesize_c, sizeof(filesize_c));

	if(0 == filesize_c){
		//文件已经在客户端完整存在
		printf("文件已经在客户端完整存在\n");
		goto end;
	}


	char *p = (char*)mmap(NULL, filestat.st_size, PROT_READ,MAP_SHARED,fd,0);
	if(p == (char*)-1){
		perror("mmap");
		return;
	}
	off_t total = filestat.st_size;
	char *q = p;
	
	if(1 == filesize_c){
		//文件不在客户端存在，需从头下载
		printf("文件不在客户端，需从头下载大小:%ld\n", total);				
	}else{
		//文件在客户端， 需继续下载
		total -= filesize_c;
		q += filesize_c;
		printf("文件在客户端，需要继续下载大:%ld\n", total);
	}
	while(total > 0){
		if(total > 1000){
			t.len = 1000;
		}else if(total > 0){
			t.len = total;
		}else{
			break;
		}
		memcpy(t.buf, q, t.len);
		ret = send_n(new_fd, &t, 4+t.len);
		if(-1 == ret){
			goto end;
		}
		total -= t.len;
		q += t.len;
	}
	
	munmap(p, filestat.st_size);

/*
	//循环读取文件内容，并发送
	while((t.len=read(fd,t.buf,sizeof(t.buf)))>0)
	{
		ret=send_n(new_fd,&t,4+t.len);
		if(-1==ret)
		{
			goto end;
		}
	}

*/
	//发送结束
	t.len=0;
	send(new_fd,&t.len,4,0);//发送结束符
end:	
	close(fd);
	close(new_fd);
}
