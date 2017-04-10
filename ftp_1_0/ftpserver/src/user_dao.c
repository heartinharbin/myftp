#include "func.h"

int find_user_by_name(char *name, int *uid, char *salt, char *passwd, char *filepath)
{
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
	char query[128] = {0};
	sprintf(query, "select uid,salt,passwd,filepath from user where username='%s'", name);
	printf("%s\n", query);
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
            if((row=mysql_fetch_row(res))!=NULL)
            {
                //printf("num=%d\n",mysql_num_fields(res));//列数
              //  for(t=0;t<mysql_num_fields(res);t++)
              //	    printf("%s ",row[t]);
              //  printf("\n");
				if(NULL != uid){
					*uid = atoi(row[0]);
				}
				if(NULL != salt){
					strcpy(salt, row[1]);
				}
				if(NULL != passwd){
					strcpy(passwd, row[2]);
				}
				if(NULL != filepath){
					strcpy(filepath, row[3]);
				}
            }else{
				*uid = -1;
			}
        }
        mysql_free_result(res);
    }
    mysql_close(conn);
	printf("uid = %d\n", *uid);
    return 0;
}

void  insert_user(char *username, char *salt, char *passwd, char *filepath)
{
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    char query[512];
    sprintf(query, "insert into user(username,salt,passwd,filepath) values('%s','%s','%s','%s')", username, salt, passwd, filepath);
	printf("%s\n", query);
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
void main(){
	int uid;
	char salt[50] = {0};
	char passwd[128] = {0};
	
	find_user_by_name("yangwen", &uid, salt, passwd);
	printf("uid = %d\n", uid);	
	printf("uid=%d salt=%s passwd=%s\n", uid ,salt, passwd);
	printf("%d\n", salt == NULL);
}
*/
