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
	
	char* query="select * from user where username='mustafa'";
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
				for(t=0;t<mysql_num_fields(res);t++)
					printf("%s ",row[t]);
				printf("\n");
			}
		}
		mysql_free_result(res);
	}
	mysql_close(conn);
	return 0;
}

