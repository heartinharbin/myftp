#include "func.h"

void  insert_log(int uid, char *ip, int port, char *operate)
{
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
	char query[128];
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
/*
int main(){
	insert_log(2, "127.0.0.1", 2000, "dangerous");
}
*/
