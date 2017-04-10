#include "func.h"

void  insert_log(int uid, char *ip, int port, char *operate)
{
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
	char query[128] = {0};
	sprintf(query, "insert into log(uid,ip,port,operate) values(%d,'%s',%d,'%s')", uid, ip, port, operate);
    
    int t,r;
    mysql_get_conn(&conn);
    t=mysql_query(conn,query);
    if(t)
    {
        printf("Error making query:%s\n",mysql_error(conn));
    }else{
        printf("insert success\n");
    }
    mysql_close(conn);
    return;
}

void find_log_by_username(char *username, int lognum, char *buf)
{
	MYSQL *conn;
	MYSQL_RES *res;
	MYSQL_ROW row;
	char tmp[512] = {0};
	char query[128] = {0};

	if(lognum == 0 || lognum > 50){
		lognum = 50;
	}
	sprintf(query, "select ip,port,operate,operate_time from log where uid=(select uid from user where username='%s') order by operate_time desc limit %d", username, lognum);
	int t,r;
	mysql_get_conn(&conn);
	t=mysql_query(conn,query);
	if(t)
	{
		printf("Error making query:%s\n",mysql_error(conn));
	}else{
		printf("Query made...\n");
		res=mysql_use_result(conn);
		if(res)
		{
			while((row=mysql_fetch_row(res))!=NULL)
			{	
				//printf("num=%d\n",mysql_num_fields(res));//列数
				bzero(tmp, sizeof(tmp));
				for(t=0;t<mysql_num_fields(res);t++)
				{	
					bzero(tmp, sizeof(tmp));
					sprintf(tmp, "%-20s ",row[t]); 
					strcat(buf, tmp);
				}
				strcat(buf, "\n");
				
			}
		}
		mysql_free_result(res);
	}
	mysql_close(conn);
	return;
}

/*
int main(){
//	insert_log(2, "127.0.0.1", 2000, "dangerous");
	char buf[MAXBUFSIZE];
	find_log_by_username("machao", 5, buf);
	puts(buf);	
}
*/
