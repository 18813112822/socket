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
    MYSQL conn;
    int res;
    char username[32] = "user2";
    char password[32] = "66666";
    char sql[100]="insert into clients values('1','";
    strcat(sql, username);
    strcat(sql, "','");
    strcat(sql, password);
    strcat(sql, "');");
    printf("%s", sql);
    mysql_init(&conn);

    if(mysql_real_connect(&conn,HOST,USERNAME,PASSWORD,DATABASE,0,NULL,CLIENT_FOUND_ROWS))
    {
        printf("Connect Success!\n");
        res=mysql_query(&conn,sql);
        if(res)
        {
            printf("Insert Error!\n");
        }
        else
        {
            printf("Insert Success!\n");
        }
    }
    else
    {
        printf("Connect Failed!\n");
        exit(-1);
    }
    return 0;
}
