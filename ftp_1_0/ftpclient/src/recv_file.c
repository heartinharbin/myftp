#include "func.h"

void recv_file(int sfd,  char *filename){
    int len;
    char buf[1000];
	int ret;
	//接收文件名
    //先接收文件名
    bzero(buf,sizeof(buf));
    ret=recv(sfd,&len,sizeof(len),0);
    printf("len:%d\n", len);
    recv(sfd,buf,len,0);
    printf("buf:%s\n", buf);
    int fd;
    fd=open(buf,O_CREAT|O_RDWR,0600);
    if(-1==fd)
    {
        perror("open");
        close(sfd);
        return -1;
    }

	recv_n(sfd, &len, 4);
	if(-1 == len){
		goto end;
	}	
    off_t flen;
    recv_n(sfd, (char*)&flen, sizeof(flen));
    printf("接收到文件大小：%ld\n", flen);
    off_t total = flen;
    off_t retlen;
    off_t bufsize = sizeof(buf);
    while(total > 0){
        bzero(buf, bufsize);
        if(total > bufsize){
            recv_n(sfd, buf, bufsize);
            write(fd, buf, bufsize);
            total -= bufsize;
        }else if(total > 0){
            recv_n(sfd, buf, total);
            write(fd, buf, total);
            total -= total;
        }else{
            break;
        }
    }
	printf("recv end\n");
//	recv(sfd, &len, 4, 0);
    close(fd);
end:
	return;
}
