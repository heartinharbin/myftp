#include "func.h"

void recv_file(int sfd,  char *filename){
    int len;
    char buf[MAXBUFSIZE];
	int ret;
	//接收文件名
    //先接收文件名
    bzero(buf,sizeof(buf));
    ret=recv(sfd,&len,sizeof(len),0);
//    printf("len:%d\n", len);
    recv(sfd,buf,len,0);
//    printf("buf:%s\n", buf);
    int fd;

	/*
    fd=open(buf,O_CREAT|O_RDWR,0600);
    if(-1==fd)
    {
        perror("open");
       // close(sfd);
        return;
    }
	*/
	recv_n(sfd, &len, 4);
	if(-1 == len){//服务器端文件不存在
		goto end;
	}
	//else if(0 == len){
	//服务器端文件存在
	off_t filesize;
	off_t filesize_tmp;	
	recv_n(sfd, &filesize, sizeof(filesize));

	
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
		
	}else if(0 == filesize){
		//文件大小为0，不需要下载
		printf("文件大小为0，不需要下载\n");
		filesize_tmp = -2;
		send_n(sfd, &filesize_tmp, sizeof(filesize_tmp));
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
	
	printf("recv end\n");
    close(fd);
end:
	return;
}


int send_file(int new_fd, char *filename)
{
	train t;//定义小火车
	//第一步传输文件名
	bzero(&t,sizeof(t));
	t.len=strlen(filename);
	strcpy(t.buf,filename);
	int  len;
	send(new_fd,&t,4+t.len,0);//发送文件名到客户端
	//发送文件内容
//	printf("len:%d buf:%s\n", t.len, t.buf);
	int fd;
	int ret;
	fd=open(filename,O_RDWR);
	if(-1==fd)
	{	//服务器端文件不存在
		perror("open");
		len = -1;
		send_n(new_fd, &len, 4);
//		close(new_fd);
		return -1;
	}else{

		//服务器端文件存在，可以下载
		len = 0;
		send_n(new_fd, &len, 4);
		
		struct stat filestat;
		fstat(fd, &filestat);
//		printf("filename:%s st_size:%ld\n", filename, filestat.st_size);
		
		off_t filesize = filestat.st_size;
		send_n(new_fd, &filesize, sizeof(filesize));
		off_t filesize_c; 
		recv_n(new_fd, &filesize_c, sizeof(filesize_c));
	
		if(0 == filesize_c){
			//文件已经在客户端完整存在
			printf("文件已经在客户端完整存在\n");
			goto end;
		}
	
/*	
		char *p = (char*)mmap(NULL, filestat.st_size, PROT_READ,MAP_SHARED,fd,0);
		if(p == (char*)-1){
			perror("mmap");
			return -1;
		}*/

		off_t total = filestat.st_size;
	//	char *q = p;
		off_t offset;
		
		if(1 == filesize_c){
			//文件不在客户端存在，需从头下载
			offset = 0;
			printf("文件不在客户端，需从头下载大小:%ld\n", total);				
		}else{
			//文件在客户端， 需继续下载
			total -= filesize_c;
			offset = filesize_c;
			printf("文件在客户端，需要继续下载大:%ld\n", total);
		}

		sendfile_n(new_fd, fd, &offset, total);
		/*
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
		*/
		//发送结束
	//	t.len = 0;
	//	send(new_fd, &t.len, 4, 0);
	}

	printf("send end!\n");
end:	
	close(fd);
//	close(new_fd);
	return 0;
}






