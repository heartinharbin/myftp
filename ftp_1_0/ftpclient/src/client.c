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
 //      printf("%s = %s\n", n , v);
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
	get_value_by_name(IP_CONF, ipvalue);
	get_value_by_name(PORT_CONF, portvalue);
	
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
	ret=connect(sfd,(struct sockaddr*)&ser,sizeof(ser));
	if(-1==ret)
	{
		perror("connect");
		return -1;
	}
	printf("connect succss\n");

	int len;
	char buf[MAXBUFSIZE];
/* //接收文件名

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
*/

	char cmd[50];
	train t;	
	char username[50] = {0};
	char salt[50] = {0};
	char passwd[128] = {0};
	char repasswd[128] = {0};
	//客户选择登录或者注册
	
	int select_num;
enrollbefore:
	while(1){
		system("clear");
		printf("\t\t\t\t客户登录\n\n");
		printf("\t\t1：用户登录\n");
		printf("\t\t2：用户注册\n");
		printf("\t\t3：用户退出\n");
		printf("请输入你的数字编号后回车：");
		scanf("%d", &select_num);
//		printf("select_num:%d\n", select_num);
		switch(select_num){
			case 1: 
					len = 1;
					send_n(sfd, &len, 4);
					goto printusername;
					break;
			case 2:
					len = 2;
					send_n(sfd, &len, 4);
enrollusername:
					printf("请输入你的用户名：");
					scanf("%s", username);
					bzero(&t, sizeof(t));
					t.len = strlen(username);
					strcpy(t.buf, username);
					send_n(sfd, &t, 4+t.len);

					//接收服务器端的反馈
					recv_n(sfd, &len, 4);
//					printf("接收反馈的len:%d\n", len);
					if(-1 == len){
						//用户名存在，不能注册
						printf("用户名已经存在，请重新输入!\n");
						goto enrollusername;
					}else if(0 == len){
						//可以注册
						recv_n(sfd, &len, 4);
//						printf("接收salt长度:%d\n", len);
						recv_n(sfd, salt, len);
//						printf("接收salt:%s\n", salt);						
						strcpy(passwd, getpass("请输入密码:"));
						strcpy(repasswd, getpass("请确认密码："));
						if(0 == strcmp(passwd, repasswd)){
							//2次密码相同
							printf("2次密码相同\n");
							len = 0;
							send_n(sfd, &len, 4);
						//	printf("1\n");	
							bzero(&t, sizeof(t));
//							printf("passwd:%s salt:%s\n", passwd, salt);
//							printf("crypt:%s\n", crypt(passwd, salt));
							strcpy(t.buf, crypt(passwd,salt));
						//	printf("2\n");
							t.len = strlen(t.buf);
							send_n(sfd, &t, 4+t.len);
						//	printf("3\n");
							recv_n(sfd, &len, 4);
							printf("注册成功，转向登录界面\n");
							goto printusername;
						}else{
							//2次密码不同
							printf("2次输入密码不相同，转回注册页面:\n");
							len = -1;
							send_n(sfd, &len, 4);
							goto  enrollusername;
						}
					}
					


					break;
			case 3:
					len = 3;
					send_n(sfd, &len, 4);
					goto logout;				
					break;
			defalut:
					len = -1;
					send_n(sfd, &len, 4);
					printf("输入编号错误！\n");
					goto enrollbefore;
					break;
		}
	}

	bzero(username, sizeof(username));
	bzero(salt, sizeof(salt));
	bzero(passwd, sizeof(passwd));


//客户登录验证：	
	while(1){
printusername:
		write(1, "username:", 9);		
		//读取用户名并发送给服务器
		read(0, username, sizeof(username));
		username[strlen(username)-1] = '\0';
		bzero(&t, sizeof(t));
		t.len = strlen(username);
		strcpy(t.buf, username);
		send_n(sfd, &t, 4+t.len);
			
		//接收服务器发送的salt或者用户名错误信息
		recv_n(sfd, &len, sizeof(len));
		if(0 == len){
			printf("用户名错误!\n");
			goto printusername;
		}else{
			recv_n(sfd, salt, len);
			strcpy(passwd, getpass("请输入密码:"));
		//	printf("passwd:%s\n", passwd);
			bzero(&t, sizeof(t));
			strcpy(t.buf, crypt(passwd, salt));
			t.len = strlen(t.buf);
			send_n(sfd, &t, 4+t.len);

			recv_n(sfd, &len, 4);
			if(0 == len){	//
				printf("登录成功！\n");
				break;
			}else{			//
				printf("登录失败！\n");
				goto printusername;
			}
		}		
	}

	
	while(1){

		printf("请输入命令：\n");
		bzero(&cmd, sizeof(cmd));
		read(0, cmd, sizeof(cmd)-1);
		cmd[strlen(cmd)-1] = '\0';
		char ccc[128] = {0};
		char ppp[128] = {0};
		sscanf(cmd, "%s %s", ccc, ppp);
//		printf("c:%s p:%s\n", ccc, ppp);
		bzero(cmd, sizeof(cmd));
		sprintf(cmd, "%s %s", ccc, ppp);
//		printf("cmd:%s\n", cmd);

	//	write(1, cmd, strlen(cmd));
	//	printf("长度：%ld\n", strlen(cmd));
		if(!strncmp("cd", cmd, 2)){
			bzero(&t, sizeof(t));
			t.len = strlen(cmd);
			strcpy(t.buf, cmd);
			send_n(sfd, &t, 4+t.len);
			
			bzero(buf, sizeof(buf));
			recv_n(sfd, &len, sizeof(len));
			recv_n(sfd, buf, len);
			system("clear");
			printf("%s\n", buf);
		}else if(!strncmp("ls", cmd, 2)){
			 bzero(&t, sizeof(t));
             t.len = strlen(cmd);
             strcpy(t.buf, cmd);
             send_n(sfd, &t, 4+t.len);			
			
			 bzero(buf, sizeof(buf));
             recv_n(sfd, &len, sizeof(len));
             recv_n(sfd, buf, len);
			 system("clear");
             printf("%s\n", buf);
		}else if(!strncmp("puts", cmd, 4)){
			 bzero(&t, sizeof(t));
	         t.len = strlen(cmd);
	         strcpy(t.buf, cmd);
	         send_n(sfd, &t, 4+t.len);
				
			 ret = send_file(sfd, cmd+5);
			 system("clear");
			 if(-1 == ret){
				printf("%s upload failed!\n", cmd+5);
			 }else{
				printf("%s upload success!\n", cmd+5);
			 }	
		}else if(!strncmp("gets", cmd, 4)){
			 bzero(&t, sizeof(t));
             t.len = strlen(cmd);
             strcpy(t.buf, cmd);
             send_n(sfd, &t, 4+t.len);
			
			 recv_file(sfd, cmd+5);
			
             bzero(buf, sizeof(buf));
             recv_n(sfd, &len, sizeof(len));
             recv_n(sfd, buf, len);
			 system("clear");
             printf("%s\n", buf);
		}else if(!strncmp("remove", cmd, 6)){
			bzero(&t, sizeof(t));
			t.len = strlen(cmd);
            strcpy(t.buf, cmd);
            send_n(sfd, &t, 4+t.len);		
			
    	    bzero(buf, sizeof(buf));
	        recv_n(sfd, &len, sizeof(len));
         	recv_n(sfd, buf, len);
			system("clear");
         	printf("%s\n", buf);	
		}else if(!strncmp("pwd", cmd, 3)){
			bzero(&t, sizeof(t));
            t.len = strlen(cmd);
            strcpy(t.buf, cmd);
            send_n(sfd, &t, 4+t.len);
			
			bzero(buf, sizeof(buf));
			recv_n(sfd, &len, sizeof(len));
			recv_n(sfd, buf, len);
			system("clear");
			puts(buf);
		}else if(!strncmp("mkdir", cmd, 5)){
			bzero(&t, sizeof(t));
            t.len = strlen(cmd);
            strcpy(t.buf, cmd);
            send_n(sfd, &t, 4+t.len);

            bzero(buf, sizeof(buf));
            recv_n(sfd, &len, sizeof(len));
            recv_n(sfd, buf, len);
			system("clear");
            puts(buf);
			
		}else if(!strncmp("log", cmd, 3)){
            bzero(&t, sizeof(t));
            t.len = strlen(cmd);
            strcpy(t.buf, cmd);
            send_n(sfd, &t, 4+t.len);

            bzero(buf, sizeof(buf));
            recv_n(sfd, &len, sizeof(len));
            recv_n(sfd, buf, len);
			system("clear");
            puts(buf);

		}else if(!strncmp("exit", cmd, 4) || !strncmp("quit", cmd, 4)){
            bzero(&t, sizeof(t));
            t.len = strlen(cmd);
            strcpy(t.buf, cmd);
            send_n(sfd, &t, 4+t.len);

			goto logout;
		}else{
			printf("命令错误!\n");
		}
			
	}


/* // 小火车方式接收
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
*/

/* //sendfile  接
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

	close(fd);
*/
logout:

	close(sfd);
}
