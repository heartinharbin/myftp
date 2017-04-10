#include "func.h"

void child_handle(int fdr)
{
	short flag=1;
	int new_fd;
	signal(SIGPIPE,SIG_IGN);//设置为忽略
	int len;
	char buf[MAXBUFSIZE];
	train t;
	int ret;

	char username[50];
	int uid;
	char salt[50];
	char passwd[128];
	char filepath[256];	

	char ip_port[50];
	char ip[30];
	char port[10];
	while(1)
	{	
		bzero(ip_port, sizeof(ip_port));
		recv_fd(fdr,&new_fd, ip_port);
		sscanf(ip_port, "%s : %s", ip, port);
		printf("ip=%s port=%s\n", ip, port);
		//	hand_request(new_fd);//发送文件函数

enrollbefore:
		//用户登录注册模块
		while(1){
			sizeof(buf, sizeof(buf));
			recv_n(new_fd, &len, 4);
			if(1 == len){
			//用户直接登录
				goto recvusername;	//break也可以的
			}else if(2 == len){
enrollusername:
				bzero(username, sizeof(username));
				bzero(salt, sizeof(salt));
				bzero(filepath, sizeof(filepath));
				bzero(passwd, sizeof(passwd));
				bzero(&t, sizeof(t));
				recv_n(new_fd, &len, 4);
				recv_n(new_fd, username, len);
			//	printf("接收到用户名：%s\n", username);
				find_user_by_name(username, &uid, NULL, NULL, NULL);
				if(-1 == uid){
					//用户名不存在，可以注册
					len = 0;
					send_n(new_fd, &len, 4);
					
					//发送salt
					bzero(buf, sizeof(buf));
					get_rand_str(buf,8);
					sprintf(salt, "$6$%s", buf);
					printf("salt:%s生成\n", salt);
					bzero(&t, sizeof(t));
					t.len = strlen(salt);
			//		printf("发送salt长度:%d\n", t.len);
					strcpy(t.buf, salt);
			//		printf("小火车中salt内容:%s\n", t.buf);
					send_n(new_fd, &t, 4+t.len);

					//接收反馈
					recv_n(new_fd, &len, 4);
			//		printf("反馈len：%d\n", len);
					if(0 == len){
						//用户2次密码匹配，接收用户的passwd
						recv_n(new_fd, &len, 4);
						recv_n(new_fd, passwd, len);
						sprintf(filepath, "%s/%s", FTP_PATH, username);
						mkdir(filepath, 0777);
			//			printf("filepath:%s 目录创建成功\n", filepath);
						insert_user(username, salt, passwd, filepath);
						len = 520;
						send_n(new_fd, &len, 4);
						goto recvusername;
					}else if(-1 == len){
						//用户2次密码不匹配
						goto  enrollusername;						
					}
					
				}else{
					//用户名已经存在，不能使用
					len = -1;
					send_n(new_fd, &len, 4);
					goto  enrollusername;
				}
			}else if(3 == len){
				goto logout;
			}else if(-1 == len){
				goto enrollbefore;
			}

		}	



		//登录模块
		while(1){	
recvusername:	//接收客户端发的登录用户名
			bzero(username, sizeof(username));
			bzero(salt, sizeof(salt));
			bzero(passwd, sizeof(passwd));
			bzero(filepath, sizeof(filepath));
			recv_n(new_fd, &len, sizeof(len));
			recv_n(new_fd, username, len);
		//	printf("接收到用户名:%s\n", username);
			
			find_user_by_name(username, &uid, salt, passwd, filepath);
			if(-1 == uid){
				bzero(&t, sizeof(t));
				t.len = 0;
				send_n(new_fd, &t, 4+t.len);
			}else{
				bzero(&t, sizeof(t));
				t.len = strlen(salt);
				strcpy(t.buf, salt);
				send_n(new_fd, &t, 4+t.len);
					
				bzero(buf, sizeof(buf));
				recv_n(new_fd, &len, 4);
				recv_n(new_fd, buf, len);
		//		printf("passwd:%s\n", buf);
				if(strcmp(passwd, buf) == 0){
					len = 0;//登录成功
					send_n(new_fd, &len, 4);
					chdir(filepath);//切换到用户自己的文件目录
				    insert_log(uid, ip, atoi(port), "login");	//记录登录日志
					printf("chdir filepath:%s\n", filepath);	
					break;
				}else{
					len = 1;//登录失败
					send_n(new_fd, &len, 4);
					goto recvusername;
				}
			}
		}		
	
		char ccc[128];
		char ppp[128];//存放命令和参数
		while(1){
			recv_n(new_fd, &len, sizeof(len));	
		//	printf("buf len:%d\n", len);
			bzero(buf, sizeof(buf));
			recv_n(new_fd, buf, len);
		//	printf("buf:%s\n", buf);
	
			bzero(&t, sizeof(t));
            if(!strncmp("cd", buf, 2)){
				ret = chdir(buf+3);
				if(-1 == ret){
					strcpy(t.buf, "cd failed");
					t.len = strlen(t.buf);
					send_n(new_fd, &t, 4+t.len); 
				}else{
					insert_log(uid, ip, atoi(port), buf);  //插入日志
					strcpy(t.buf, "cd success");
					t.len = strlen(t.buf);
		//			printf("t.len:%d\n", t.len);
					send_n(new_fd, &t,  4+t.len);
				}         		
			}else if(!strncmp("ls", buf, 2)){
				 insert_log(uid, ip, atoi(port), buf);  //插入日志
   				 myls(NULL, t.buf);
			//	 ret = 1;
            //     if(-1 == ret){
            //         strcpy(t.buf, "remove failed");
            //         t.len = strlen(t.buf);
            //         send_n(new_fd, &t, 4+t.len);
            //     }else{
                     t.len = strlen(t.buf);
                     send_n(new_fd, &t,  4+t.len);
            //     }
		
		    }else if(!strncmp("puts", buf, 4)){
				insert_log(uid, ip, atoi(port), buf);  //插入日志
				recv_file(new_fd, buf+5);   				
            }else if(!strncmp("gets", buf, 4)){
				ret = hand_request(new_fd, buf+5);
				if(-1 == ret){
					strcpy(t.buf, buf+5);
					strcat(t.buf, " download failed!");
					t.len = strlen(t.buf);
					send_n(new_fd, &t, 4+t.len);   
						                              
				}else{
					insert_log(uid, ip, atoi(port), buf);  //插入日志
					strcpy(t.buf, buf+5);
					strcat(t.buf, " download success!");
					t.len = strlen(t.buf);
					send_n(new_fd, &t, 4+t.len);

				}				   				
            }else if(!strncmp("remove", buf, 6)){
   				ret = remove(buf+7);
				if(-1 == ret){
                    strcpy(t.buf, "remove failed");
                    t.len = strlen(t.buf);
                    send_n(new_fd, &t, 4+t.len);
                }else{
					insert_log(uid, ip, atoi(port), buf);  //插入日志
                    strcpy(t.buf, "remove success");
                    t.len = strlen(t.buf);
          //        printf("t.len:%d\n", t.len);
                    send_n(new_fd, &t,  4+t.len);
                }

            }else if(!strncmp("pwd", buf, 3)){
				insert_log(uid, ip, atoi(port), buf);  //插入日志
   				bzero(buf, sizeof(buf));
				getcwd(buf, sizeof(buf));
				bzero(&t, sizeof(t));
				t.len = strlen(buf);
				strcpy(t.buf, buf);
				send_n(new_fd, &t, 4+t.len);	 
            }else if(!strncmp("log", buf, 3)){
				insert_log(uid, ip, atoi(port), buf);  //插入日志
				printf("log cmd:%s\n", buf);	
				int logn = 0;
				bzero(ccc, sizeof(ccc));
				sscanf(buf, "%s %d", ccc, &logn);
		//		printf("log num:%d\n", atoi(buf+4));
				printf("log n:%d\n", logn);
				bzero(buf, sizeof(buf));
				find_log_by_username(username, logn, buf);
				t.len = strlen(buf);
				strcpy(t.buf, buf);
				send_n(new_fd, &t, 4+t.len);
			}else if(!strncmp("mkdir", buf, 5)){
				ret = mkdir(buf+6, 0777);
                if(-1 == ret){
                    strcpy(t.buf, "mkdir failed");
                    t.len = strlen(t.buf);
                    send_n(new_fd, &t, 4+t.len);
                }else{
					insert_log(uid, ip, atoi(port), buf);  //插入日志
                    strcpy(t.buf, "mkdir success");
                    t.len = strlen(t.buf);
        //          printf("t.len:%d\n", t.len);
                    send_n(new_fd, &t,  4+t.len);
                }
				
			}else if(!strncmp("exit", buf, 4) || !strncmp("quit", buf, 4)){
				goto logout;	
			}
			
		//	send_n(new_fd, "iloveyang", 9);	
		}
logout:
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
