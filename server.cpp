#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/un.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <mysql.h>

#define SERVER_PORT 12138
#define BACKLOG 20
#define MAX_CON_NO 10
#define MAX_DATA_SIZE 4096

#define HOST "localhost"
#define USERNAME "root"
#define PASSWORD "2015lzm"
#define DATABASE "users"

char listname[MAX_DATA_SIZE];

struct user
{
    int typ;
    char name[32];
    char pwd[32];
    char friendname[32];
    char sentence[200];
};

int MAX(int a,int b)
{
    if(a>b) return a;
    return b;
}

int regist(char* username, char* password)
{
    MYSQL conn;
    char* sentence = "no message\n";
    int res;
    char sql[4196]="insert into uapam values('";
    strcat(sql, username);
    strcat(sql, "','");
    strcat(sql, password);
    strcat(sql, "','");
    strcat(sql, sentence);
    strcat(sql, "');");
    mysql_init(&conn);

    if(mysql_real_connect(&conn,HOST,USERNAME,PASSWORD,DATABASE,0,NULL,CLIENT_FOUND_ROWS))
    {
        //printf("Connect Success!\n");
        res=mysql_query(&conn,sql);
        if(res)
        {
            printf("Insert Error!\n");
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

int query(char* username)
{
    if(strcmp(username, "root") == 0)
    	memset(listname,0,sizeof(listname));

    MYSQL conn;
    MYSQL_RES *res_ptr;//指向查询结果的指针
    MYSQL_FIELD *field;//字段结构体指针
    MYSQL_ROW result_row;//按行返回的查询信息

    int res;
    int i;
    int row,column;//查询返回的行数和列数
    char *sql="select * from uapam;";
    mysql_init(&conn);


    if(mysql_real_connect(&conn,HOST,USERNAME,PASSWORD,DATABASE,0,NULL,CLIENT_FOUND_ROWS))
    {
        //printf("Connect Success!\n");
        res=mysql_query(&conn,sql);
        //res=0表示查询成功
        if(res)
        {
            printf("Select Error!\n");
        }
        else
        {
           // printf("Select Success!\n");
            //将查询的结果给res_ptr
            res_ptr=mysql_store_result(&conn);
            //如果结果不为空，就可以输出了
            if(res_ptr)//可以用这个的真假来判断用户名密码是否正确
            {
                //有结果了
                column=mysql_num_fields(res_ptr);//获取列数
                row=mysql_num_rows(res_ptr)+1;//加1是因为第一行是字段名
               
                //按行输出结果
                for(i=1;i<row;i++)
                {
                    result_row=mysql_fetch_row(res_ptr);
		    if(strcmp(username, "root") == 0)
		    {
			strcat(listname, result_row[0]);
		        strcat(listname, "\n");
		    }
		    else
                    if(strcmp(result_row[0], username) == 0)
		    {
			mysql_close(&conn);
     			//printf("%s\n", result_row[0]);
			return 1;
  		    }
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
        return -1;
    }
    mysql_close(&conn);
    return 0;
}

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

int login(char* username, char* password)
{
    MYSQL conn;
    MYSQL_RES *res_ptr;//指向查询结果的指针
    MYSQL_FIELD *field;//字段结构体指针
    MYSQL_ROW result_row;//按行返回的查询信息

    int res;
    int i;
    int row,column;//查询返回的行数和列数
    char *sql="select * from uapam;";
    mysql_init(&conn);


    if(mysql_real_connect(&conn,HOST,USERNAME,PASSWORD,DATABASE,0,NULL,CLIENT_FOUND_ROWS))
    {
        //printf("Connect Success!\n");
        res=mysql_query(&conn,sql);
        //res=0表示查询成功
        if(res)
        {
            printf("Select Error!\n");
        }
        else
        {
           // printf("Select Success!\n");
            //将查询的结果给res_ptr
            res_ptr=mysql_store_result(&conn);
            //如果结果不为空，就可以输出了
            if(res_ptr)//可以用这个的真假来判断用户名密码是否正确
            {
                //有结果了
                column=mysql_num_fields(res_ptr);//获取列数
                row=mysql_num_rows(res_ptr)+1;//加1是因为第一行是字段名
               
                //按行输出结果
                for(i=1;i<row;i++)
                {
                    result_row=mysql_fetch_row(res_ptr);
		   // printf("%s %s\n", result_row[0], result_row[1]);
                    if(strcmp(result_row[0], username) == 0 && strcmp(result_row[1], password) == 0)
		    {
 			mysql_close(&conn);
			return 0;
  		    }
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
        return -1;
    }
    mysql_close(&conn);
    return -1;
}

//create a table
int create(char* tablename)
{ 
    MYSQL conn;
    int res;
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

//create a table
int createcache(char* tablename)
{ 
    MYSQL conn;
    int res;
    char sql[100]="create table ";
    strcat(sql, tablename);
    strcat(sql, "cache(sentence varchar(4096));");
    mysql_init(&conn);
   // printf("%s\n", sql);
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

void cache(char* username, char* sentence)
{
    MYSQL conn;
    int res;
    char sql[4196]="insert into ";
    strcat(sql, username);
    strcat(sql, "cache values('");
    strcat(sql, sentence);
    strcat(sql, "');");
    mysql_init(&conn);

    if(mysql_real_connect(&conn,HOST,USERNAME,PASSWORD,DATABASE,0,NULL,CLIENT_FOUND_ROWS))
    {
        //printf("Connect Success!\n");
        res=mysql_query(&conn,sql);
        if(res)
        {
            printf("Insert Error!\n");
        }
        else
        {
            //printf("Insert Success!\n");
        }
    }
    else
    {
        printf("Connect Failed!\n");
        return;
    }
    return;
}


//add friend list
int add(char* username, char* friendname)
{
    MYSQL conn;
    int res;
    char sql[100]="insert into ";
    strcat(sql, username);
    strcat(sql, " values('");
    
    strcat(sql, friendname);
    strcat(sql, "');");
    printf("%s\n", sql);
    mysql_init(&conn);

    if(mysql_real_connect(&conn,HOST,USERNAME,PASSWORD,DATABASE,0,NULL,CLIENT_FOUND_ROWS))
    {
        //printf("Connect Success!\n");
        res=mysql_query(&conn,sql);
        if(res)
        {
            printf("Insert Error!\n");
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

int getcache(char* username, char* sendm)
{
	 MYSQL conn;
    MYSQL_RES *res_ptr;//指向查询结果的指针
    MYSQL_FIELD *field;//字段结构体指针
    MYSQL_ROW result_row;//按行返回的查询信息

    int res;
    int i;
    int row,column;//查询返回的行数和列数
    char sql[100]="select * from ";
	strcat(sql, username);
	strcat(sql, "cache;");
    mysql_init(&conn);


    if(mysql_real_connect(&conn,HOST,USERNAME,PASSWORD,DATABASE,0,NULL,CLIENT_FOUND_ROWS))
    {
        //printf("Connect Success!\n");
        res=mysql_query(&conn,sql);
        //res=0表示查询成功
        if(res)
        {
            printf("Select Error!\n");
        }
        else
        {
           // printf("Select Success!\n");
            //将查询的结果给res_ptr
            res_ptr=mysql_store_result(&conn);
            //如果结果不为空，就可以输出了
            if(res_ptr)//可以用这个的真假来判断用户名密码是否正确
            {
                //有结果了
                column=mysql_num_fields(res_ptr);//获取列数
                row=mysql_num_rows(res_ptr)+1;//加1是因为第一行是字段名
               
				int begin = 1;
                //按行输出结果
				if(row > 11)
					begin = row - 10;
       			if(row == 1)
					return -1;
                for(i=begin;i<row;i++)
                {
                    result_row=mysql_fetch_row(res_ptr);
                    strcat(sendm, result_row[0]);
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
        return -1;
    }
    mysql_close(&conn);
    return 0;
}

void deletecache(char* username)
{
	MYSQL conn;
	int res;
    char sql[100]="delete from ";
    strcat(sql, username);
    strcat(sql, "cache;");
    mysql_init(&conn);

    if(mysql_real_connect(&conn,HOST,USERNAME,PASSWORD,DATABASE,0,NULL,CLIENT_FOUND_ROWS))
    {
        //printf("Connect Success!\n");
        res=mysql_query(&conn,sql);
        if(res)
        {
            printf("delete Error!\n");
        }
        else
        {
            //printf("Insert Success!\n");
        }
    }
    else
    {
        printf("Connect Failed!\n");
        return;
    }
    return;
}

int isfriend(char* tablename, char* name)
{
    MYSQL conn;
    MYSQL_RES *res_ptr;//指向查询结果的指针
    MYSQL_FIELD *field;//字段结构体指针
    MYSQL_ROW result_row;//按行返回的查询信息

    int res;
    int i;
    int row,column;//查询返回的行数和列数
    char sql[100]="select * from ";
	strcat(sql, tablename);
	strcat(sql, ";");
    mysql_init(&conn);


    if(mysql_real_connect(&conn,HOST,USERNAME,PASSWORD,DATABASE,0,NULL,CLIENT_FOUND_ROWS))
    {
        //printf("Connect Success!\n");
        res=mysql_query(&conn,sql);
        //res=0表示查询成功
        if(res)
        {
            printf("Select Error!\n");
        }
        else
        {
           // printf("Select Success!\n");
            //将查询的结果给res_ptr
            res_ptr=mysql_store_result(&conn);
            //如果结果不为空，就可以输出了
            if(res_ptr)//可以用这个的真假来判断用户名密码是否正确
            {
                //有结果了
                column=mysql_num_fields(res_ptr);//获取列数
                row=mysql_num_rows(res_ptr)+1;//加1是因为第一行是字段名
               
                for(i=1;i<row;i++)
                {
                    result_row=mysql_fetch_row(res_ptr);
                    if(strcmp(result_row[0], name) == 0)
						return 1;
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
        return -1;
    }
    mysql_close(&conn);
    return 0;
}

int friendlist(char* name, char* fl)
{
    MYSQL conn;
    MYSQL_RES *res_ptr;//指向查询结果的指针
    MYSQL_FIELD *field;//字段结构体指针
    MYSQL_ROW result_row;//按行返回的查询信息

    int res;
    int i;
    int row,column;//查询返回的行数和列数
    char sql[100]="select * from ";
	strcat(sql, name);
	strcat(sql, ";");
    mysql_init(&conn);


    if(mysql_real_connect(&conn,HOST,USERNAME,PASSWORD,DATABASE,0,NULL,CLIENT_FOUND_ROWS))
    {
        //printf("Connect Success!\n");
        res=mysql_query(&conn,sql);
        //res=0表示查询成功
        if(res)
        {
            printf("Select Error!\n");
        }
        else
        {
           // printf("Select Success!\n");
            //将查询的结果给res_ptr
            res_ptr=mysql_store_result(&conn);
            //如果结果不为空，就可以输出了
            if(res_ptr)//可以用这个的真假来判断用户名密码是否正确
            {
                //有结果了
                column=mysql_num_fields(res_ptr);//获取列数
                row=mysql_num_rows(res_ptr)+1;//加1是因为第一行是字段名
               
                for(i=1;i<row;i++)
                {
                    result_row=mysql_fetch_row(res_ptr);
                    strcat(fl, result_row[0]);
					strcat(fl, "\n");
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
        return -1;
    }
    mysql_close(&conn);
    return 0;
}

int main(int argc,char *argv[])
{
    //init listname
    char* r = "root";
    query(r);
    
    struct sockaddr_in serverSockaddr,clientSockaddr;
    char sendm[40960];
    char sendBuf[MAX_DATA_SIZE],recvBuf[MAX_DATA_SIZE];
    int sendSize,recvSize;
    int sockfd,clientfd;
    fd_set servfd,recvfd;//用于select处理用的
    int fd_A[BACKLOG+1];//保存客户端的socket描述符
    char fd_C[BACKLOG+1][32];//用于保存客户端的用户名
    int chaton[BACKLOG+1]; 
    int conn_amount;//用于计算客户端的个数
    int max_servfd,max_recvfd;
    int on=1;
    socklen_t sinSize=0;
    char username[32];
    int pid;
    int i, j;
    struct timeval timeout;
    
    if(argc != 2)
    {
        printf("usage: ./server [username]\n");
        exit(1);
    }
    strcpy(username,argv[1]);
    printf("username:%s\n",username);

    /*establish a socket*/
    if((sockfd = socket(AF_INET,SOCK_STREAM,0))==-1)
    {
        perror("fail to establish a socket");
        exit(1);
    }
    printf("Success to establish a socket...\n");

    /*init sockaddr_in*/
    serverSockaddr.sin_family=AF_INET;
    serverSockaddr.sin_port=htons(SERVER_PORT);
    serverSockaddr.sin_addr.s_addr=htonl(INADDR_ANY);
    bzero(&(serverSockaddr.sin_zero),8);

    /*
     * SOL_SOCKET.SO_REUSEADDR 允许重用本地地址
     * */
    setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));

    /*bind socket*/
    if(bind(sockfd,(struct sockaddr *)&serverSockaddr,sizeof(struct sockaddr))==-1)
    {
        perror("fail to bind");
        exit(1);
    }
    printf("Success to bind the socket...\n");

    /*listen on the socket*/
    if(listen(sockfd,BACKLOG)==-1)
    {
        perror("fail to listen");
        exit(1);
    }

    timeout.tv_sec=1;//1秒遍历一遍
    timeout.tv_usec=0;
    sinSize=sizeof(clientSockaddr);//注意要写上，否则获取不了IP和端口

    FD_ZERO(&servfd);//清空所有server的fd
    FD_ZERO(&recvfd);//清空所有client的fd
    FD_SET(sockfd,&servfd);
    conn_amount=0;
    max_servfd=sockfd;//记录最大的server端描述符
    max_recvfd=0;//记录最大的client端的socket描述符
    memset(fd_A,0,sizeof(fd_A));
    memset(fd_C,0,sizeof(fd_C));
    memset(chaton,0,sizeof(chaton));
    while(1)
    {
        FD_ZERO(&servfd);//清空所有server的fd
        FD_ZERO(&recvfd);//清空所有client的fd
        FD_SET(sockfd,&servfd);
        //timeout.tv_sec=30;
         switch(select(max_servfd+1,&servfd,NULL,NULL,&timeout))
        {
            case -1:
                perror("select error");
                break;
            case 0:
                break;
            default:
                //printf("has datas to offer accept\n");
                if(FD_ISSET(sockfd,&servfd))//sockfd 有数据表示可以进行accept
                {
                    /*accept a client's request*/
                    if((clientfd=accept(sockfd,(struct sockaddr  *)&clientSockaddr, &sinSize))==-1)
                    {
                        perror("fail to accept");
                        exit(1);
                    }
                    printf("Success to accpet a connection request...\n");
                    printf(">>>>>> %s:%d join in! ID(fd):%d \n",inet_ntoa(clientSockaddr.sin_addr),ntohs(clientSockaddr.sin_port),clientfd);
		   
                   
                    //加入一个客户端
                    while(fd_A[conn_amount] != 0)
                    	conn_amount = (conn_amount+1) % BACKLOG;
                   	fd_A[conn_amount]=clientfd;
		   			max_recvfd=MAX(max_recvfd,clientfd);
                }
                break;
        }
        
	
        for(i=0;i<BACKLOG;i++)//最大队列进行判断
        {
            if(fd_A[i]!=0)
            {
                FD_SET(fd_A[i],&recvfd);//对所有还连着服务器的客户端都放到fd_set中用于下面select的判断
            }
        }

  	switch(select(max_recvfd+1,&recvfd,NULL,NULL,&timeout))
        {
            case -1:
                //select error
                break;
            case 0:
                //timeout
                break;
            default:
                for(i=0;i<BACKLOG;i++)
                {
                    if(FD_ISSET(fd_A[i],&recvfd))
                    {
                        /*receive datas from client*/
						struct user use;
						use.typ = 0;
						memset(use.name,0,sizeof(use.name));
						memset(use.pwd,0,sizeof(use.pwd));
						memset(use.friendname,0,sizeof(use.friendname));
						memset(use.sentence,0,sizeof(use.sentence));
						memset(sendBuf,0,sizeof(sendBuf));
                        if((recvSize=recv(fd_A[i],(char *)&use,sizeof(struct user),0))==-1)
                        {
                            //perror("fail to receive datas");
                            //表示该client是关闭的
                            printf("close\n");
                            FD_CLR(fd_A[i],&recvfd);
                            fd_A[i]=0;//表示该描述符已经关闭
			    			chaton[i] = 0;
                        }
                        else
                        {
                            if(use.typ==1)
                   	   		{
                       			if(query(use.name) == 0)
								{
								  strcat(listname, use.name);
								  strcat(listname, "\n");
								  create(use.name);
				 				  createcache(use.name);
			 	 				  if(regist(use.name, use.pwd) == 0)
									  strcpy(sendBuf, "yes");
			 					  else
					   				  strcpy(sendBuf, "no");
								}			
								else
			  	   				  	strcpy(sendBuf, "no");
								if((sendSize=send(fd_A[i],sendBuf,strlen(sendBuf),0))!=strlen(sendBuf))
                                    {
                                        perror("fail");
                                        exit(1);
                                    }		
								continue;
			    			 }

		 	    			 if(use.typ == 2)
			    			 {
								 memset(fd_C[i],0,sizeof(fd_C[i]));
								 strcpy(fd_C[i], use.name);
			   					 if(login(use.name, use.pwd) == 0)
			    			   	 	strcpy(sendBuf, "yes");
			   					 else
									strcpy(sendBuf, "no");
			   					 if((sendSize=send(fd_A[i],sendBuf,strlen(sendBuf),0))!=strlen(sendBuf))
                                    {
                                        perror("fail");
                                        exit(1);
                                    }
									continue;
			   				  }
		
			   				  if(use.typ == 3)
			  				   {
			     					if(add(use.name, use.friendname) == 0)
			    					 	strcpy(sendBuf, "yes");
			   						else
										strcpy(sendBuf, "no");
			    					
									if((sendSize=send(fd_A[i],sendBuf,strlen(sendBuf),0))!=strlen(sendBuf))
                                    {
                                        perror("fail");
                                        exit(1);
                                    }
									continue;
			    			 }
			    
			    			 if(use.typ == 4)
			   				  {
								if((send(fd_A[i],listname,strlen(listname),0))==-1)
                                    {
                                        perror("fail");
                                        exit(1);
                                    }
									continue;
			    			 }
			     
			   				 if(use.typ == 5)
			   				  {
								if(strcmp(use.name, "root") == 0)
								{
									if((send(fd_A[i],(char*)&use,sizeof(struct user),0))==-1)
                                    	{
                                       		 perror("fail");
                                       		 exit(1);
                                    	}
									chaton[i] = 0;
									continue;
								}
								//printf("type = 5\n");
								int flag = 0;
								for(int k = 0; k < BACKLOG; k++)
								{
								    if(strcmp(fd_C[k], use.friendname) == 0 && fd_A[k] != 0 && chaton[k] == 1)
				 				   {
										flag = 1;
										if((send(fd_A[k],(char*)&use,sizeof(struct user),0))==-1)
                                    	{
                                       		 perror("fail");
                                       		 exit(1);
                                    	}
				    				}
								}
								printf("%d\n", flag);
								if(flag == 0)
								{
    								strcat(sendBuf, use.name);
								    strcat(sendBuf, ":");
								    strcat(sendBuf, use.sentence);
								    strcat(sendBuf, "\n");
				  					cache(use.friendname, sendBuf);
								}
								continue;
			   				  }
			  	  
		 	    			 if(use.typ == 6)
			     			{
								memset(sendBuf,0, sizeof(sendBuf));
								if(isfriend(use.name, use.friendname) == 1 && isfriend(use.friendname, use.name) == 1)
									strcpy(sendBuf, "yes");
								else
									strcpy(sendBuf, "no");
								//printf("%s\n", sendBuf);
								if((send(fd_A[i],sendBuf,sizeof(sendBuf),0))==-1)
                                    	{
                                       		 perror("fail");
                                       		 exit(1);
                                    	}
								chaton[i] = 1;
								continue;
			    			 }
			     
			    			 if(use.typ == 7)
			    			 {
								
			       				memset(sendm, 0, sizeof(sendm));
								if(getcache(use.name, sendm) != 0)
								{
								    memset(sendm, 0, sizeof(sendm));
								    strcpy(sendm, "no message\n");
								}
				
								if((send(fd_A[i],sendm,sizeof(sendm),0))==-1)
                                    	{
                                       		 perror("fail");
                                       		 exit(1);
                                    	}
								deletecache(use.name);
								continue;
			    			 }
							
                     
							if(use.typ == 8)							
							{
								memset(sendBuf, 0, sizeof(sendBuf));
								friendlist(use.name, sendBuf);
							//	printf("%s\n", sendBuf);
								if((send(fd_A[i],sendBuf,sizeof(sendBuf),0))==-1)
                                    	{
                                       		 perror("fail");
                                       		 exit(1);
                                    	}
								continue;
							}

                        }
                    }
                }
                break;
        }

    }
    return 0;
}
