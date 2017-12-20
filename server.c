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

#define SERVER_PORT 12138
#define BACKLOG 20
#define MAX_CON_NO 10
#define MAX_DATA_SIZE 4096

int MAX(int a,int b)
{
    if(a>b) return a;
    return b;
}

struct user
{
    char name[32];
    char pwd[32];
};

int main(int argc,char *argv[])
{
    struct sockaddr_in serverSockaddr,clientSockaddr;
    char sendBuf[MAX_DATA_SIZE],recvBuf[MAX_DATA_SIZE];
    int sendSize,recvSize;
    int sockfd,clientfd;
    fd_set servfd,recvfd;//用于select处理用的
    int fd_A[BACKLOG+1];//保存客户端的socket描述符
    char fd_C[BACKLOG+1][32];//用于保存客户端的用户名
    int conn_amount;//用于计算客户端的个数
    int max_servfd,max_recvfd;
    int on=1;
    socklen_t sinSize=0;
    char username[32];
    int pid;
    int i, j;
    struct timeval timeout;
    struct user use;
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
                    if((recvSize=recv(clientfd,(char *)&use,sizeof(struct user),0))==-1 || recvSize==0)
                    {
                        perror("fail to receive datas");
                    }
                    printf("客户端发来的用户名是:%s,密码:%s\n",use.name,use.pwd);
                    memset(recvBuf,0,sizeof(recvBuf));
                    if(strcmp(use.name,"admin")==0 || strcmp(use.pwd,"admin")==0)
                    {
                        printf("验证成功！\n");
                        strcpy(sendBuf,"yes");
                    }
                    else
                    {
                        printf("验证失败！\n");
                        strcpy(sendBuf,"no");
                    }
                    if((sendSize=send(clientfd,sendBuf,MAX_DATA_SIZE,0))==-1)
                    {
                        perror("fail to receive datas");
                    }
                    //每加入一个客户端都向fd_set写入
                    fd_A[conn_amount]=clientfd;
                    strcpy(fd_C[conn_amount],use.name);
                    conn_amount++;
                    max_recvfd=MAX(max_recvfd,clientfd);
                }
                break;
        }
        //FD_COPY(recvfd,servfd);
        for(i=0;i<MAX_CON_NO;i++)//最大队列进行判断，优化的话，可以使用链表
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
                for(i=0;i<conn_amount;i++)
                {
                    if(FD_ISSET(fd_A[i],&recvfd))
                    {
                        /*receive datas from client*/
                        if((recvSize=recv(fd_A[i],recvBuf,MAX_DATA_SIZE,0))==-1 || recvSize==0)
                        {
                            //perror("fail to receive datas");
                            //表示该client是关闭的
                            printf("fd %d close\n",fd_A[i]);
                            FD_CLR(fd_A[i],&recvfd);
                            fd_A[i]=0;
                        }
                        else//客户端发送数据过来，然后这里进行转发
                        {
			    strcpy(sendBuf,fd_C[i]);
                            strcat(sendBuf,":");
                            strcat(sendBuf,recvBuf);
                            
                            /*send datas to client*/
                            for(j=0;j<MAX_CON_NO;j++)
                            {
                                if(fd_A[j]!=0&&i!=j)
                                {
                                    printf("数据发往%d,",fd_A[j]);
                                    if((sendSize=send(fd_A[j],sendBuf,strlen(sendBuf),0))!=strlen(sendBuf))
                                    {
                                        perror("fail");
                                        exit(1);
                                    }
                                    else
                                    {
                                        printf("Success\n");
                                    }
                                }
                            }
                            //可以判断recvBuf是否为bye来判断是否可以close
                            memset(recvBuf,0,MAX_DATA_SIZE);
 			    memset(sendBuf,0,MAX_DATA_SIZE);
                        }
                    }
                }
                break;
        }
    }
    return 0;
}
