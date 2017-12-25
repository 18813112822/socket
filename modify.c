#include <stdio.h>
#include <mysql.h>

#define HOST "localhost"
#define USERNAME "root"
#define PASSWORD "2015lzm"
#define DATABASE "users"

int modify(char* name, char* sentence)
{
    MYSQL conn;
    int res;
    char sql[4196]="update uapam set message = '";
    strcat(sql, sentence);
    strcat(sql, "' where username = '");
    strcat(sql, name);
    strcat(sql, "';");
    mysql_init(&conn);

    if(mysql_real_connect(&conn,HOST,USERNAME,PASSWORD,DATABASE,0,NULL,CLIENT_FOUND_ROWS))
    {
        //printf("Connect Success!\n");
        res=mysql_query(&conn,sql);
        if(res)
        {
            printf("modify Error!\n");
        }
        else
        {
            //printf("Insert Success!\n");
        }
    }
    else
    {
        printf("Connect Failed!\n");
        return -1;
    }
    return 0;
}

int main()
{
	modify("lzm", "hi boy");
	return 0;
}
