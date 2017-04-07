#include "func.h"

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
