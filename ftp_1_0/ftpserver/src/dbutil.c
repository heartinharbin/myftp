#include "func.h"

void mysql_get_conn(MYSQL **conn){
	char* server="localhost";
	char* user="root";
	char* password="yuan";
	char* database="ftp";

	*conn=mysql_init(NULL);
	if(!mysql_real_connect(*conn,server,user,password,database,0,NULL,0))
	{
		printf("Error connecting to database:%s\n",mysql_error(*conn));
	}else{
		printf("Connected...\n");
	}
}

//salt随机字符串8位，1位存放'\0'
void get_rand_str(char s[],int num)
	//如果是在一个程序的循环中不断调用这个函数，那么是没有效果的
	//虽然也是使用的系统的时间函数来初始化随机数发生器，但程序的
	//执行速度太快了，可能执行1000次循环返回的秒数都是一样的
	//time返回时间戳
{
	//定义随机生成字符串表
	char *str = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz,./;\"'<>?";
	int i,lstr;
	char ss[2] = {0};
	lstr = strlen(str);//计算字符串长度
	srand((unsigned int)time((time_t *)NULL));//使用系统时间来初始化随机数发生器
	for(i = 1; i <= num; i++){//按指定大小返回相应的字符串
		sprintf(ss,"%c",str[(rand()%lstr)]);//rand()%lstr 可随机返回0-71之间的整数, str[0-71]可随机得到其中的字符
		strcat(s,ss);//将随机生成的字符串连接到指定数组后面
	}
}

/*
int main()
{
    char s[9]={0};//定义一个可接收1400个字符的字符数组
    get_rand_str(s,8);
    printf("%s\n",s); //输出随机生成的字符串
//  system("pause");
}
*/
