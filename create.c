#include <stdio.h>
#include <mysql.h>
#include <stdlib.h>
#include <string.h>
#define HOST "localhost"
#define USERNAME "root"
#define PASSWORD "2015lzm"
#define DATABASE "users"

int main(int argc,char *argv[])
{
    char* tablename = "lz"; 
    MYSQL conn;
    int res;
    char username[32] = "user2";
    char password[32] = "66666";
    char sql[100]="create table ";
    strcat(sql, tablename);
    strcat(sql, "(friend varchar(32));");
    mysql_init(&conn);
    printf("%s\n", sql);
    if(mysql_real_connect(&conn,HOST,USERNAME,PASSWORD,DATABASE,0,NULL,CLIENT_FOUND_ROWS))
    {
        printf("Connect Success!\n");
        res=mysql_query(&conn,sql);
        if(res)
        {
            printf("create table Error!\n");
        }
        else
        {
            printf("create table Success!\n");
        }
    }
    else
    {
        printf("Connect Failed!\n");
        exit(-1);
    }
    return 0;
}
