#include <stdio.h>
#include <mysql.h>
#include <stdlib.h>

#define HOST "localhost"
#define USERNAME "root"
#define PASSWORD "2015lzm"
#define DATABASE "users"

int main(int argc,char *argv[])
{
    MYSQL conn;
    MYSQL_RES *res_ptr;//指向查询结果的指针
    MYSQL_FIELD *field;//字段结构体指针
    MYSQL_ROW result_row;//按行返回的查询信息

    int res;
    int i,j;
    int row,column;//查询返回的行数和列数
    char *sql="select * from clients;";
    mysql_init(&conn);


    if(mysql_real_connect(&conn,HOST,USERNAME,PASSWORD,DATABASE,0,NULL,CLIENT_FOUND_ROWS))
    {
        printf("Connect Success!\n");
        res=mysql_query(&conn,sql);
        //res=0表示查询成功
        if(res)
        {
            printf("Select Error!\n");
        }
        else
        {
            printf("Select Success!\n");
            //将查询的结果给res_ptr
            res_ptr=mysql_store_result(&conn);
            //如果结果不为空，就可以输出了
            if(res_ptr)//可以用这个的真假来判断用户名密码是否正确
            {
                //有结果了
                column=mysql_num_fields(res_ptr);//获取列数
                row=mysql_num_rows(res_ptr)+1;//加1是因为第一行是字段名
                printf("查询到的数据有 %d 行\n",row);

                //输出结果的字段名
                for(i=0;field=mysql_fetch_field(res_ptr);i++)
                {
                    printf("%s\t",field->name);
                }
                printf("\n");
                //按行输出结果
                for(i=1;i<row;i++)
                {
                    result_row=mysql_fetch_row(res_ptr);
                    for(j=0;j<column;j++)
                    {
                        printf("%s\t",result_row[j]);
                    }
                    printf("\n");
                }
            }
            else
            {
                //没有结果
                printf("没有查询到匹配的数据。");
            }
        }
    }
    else
    {
        printf("Connect Failed!\n");
        exit(-1);
    }
    mysql_close(&conn);
    return 0;
}
