#ifndef _DBUTIL_H_
#define _DBUTIL_H_


#include <mysql/mysql.h>


void mysql_get_conn(MYSQL **conn);
int find_user_by_name(char *name, int *uid, char *salt, char *passwd, char *filepath);
void insert_user(char *username, char *salt, char *passwd, char *filepath);
void insert_log(int uid, char *ip, int port, char *operate);

void get_rand_str(char s[],int num);
#endif
