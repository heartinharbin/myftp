#include <mysql/mysql.h>
#include <stdio.h>

void mysql_get_conn(MYSQL **conn){
	char* server="localhost";
	char* user="root";
	char* password="yuan";
	char* database="ftp";

	*conn=mysql_init(NULL);
	if(!mysql_real_connect(*conn,server,user,password,database,0,NULL,0))
	{
		printf("Error connecting to database:%s\n",mysql_error(conn));
	}else{
		printf("Connected...\n");
	}
}

int main()
{
	MYSQL *conn;
	MYSQL_RES *res;
	MYSQL_ROW row;
	char* query="insert into log(uid,ip,port,operate) values(2,'192.168.5.51','2000','login')";
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
	return 0;
}
