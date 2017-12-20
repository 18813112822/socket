#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h> //for gethostbyname
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#define MAX_BUF 4096
#define SERVER_PORT 12138


struct user
{
    int typ;
    char name[32];
    char pwd[32];
};

int regist(int sockfd, char* username, char* password)
{
    char recvBuf[MAX_BUF];
    struct user use;
    use.typ = 1;
    strcpy(use.name, username);
    strcpy(use.pwd, password);
    if(send(sockfd,(char *)&use,sizeof(struct user),0)==-1)
    {
        perror("fail to send datas.");
        exit(-1);
    }

    if(recv(sockfd,recvBuf,MAX_BUF,0)==-1)
    {
        perror("fail to receive datas.");
        exit(-1);
    }
    //printf("Server:%s\n",recvBuf);
    if(strcmp(recvBuf,"no")==0)
    {
        perror("密码或者用户名错误");
        return -1;
    }
    return 0;
}



int login(int sockfd, char* username, char* password)
{
    
    char recvBuf[MAX_BUF];
    struct user use;
    use.typ = 2;
    strcpy(use.name, username);
    strcpy(use.pwd, password);
    if(send(sockfd,(char *)&use,sizeof(struct user),0)==-1)
    {
        perror("fail to send datas.");
        exit(-1);
    }

    if(recv(sockfd,recvBuf,MAX_BUF,0)==-1)
    {
        perror("fail to receive datas.");
        exit(-1);
    }
    //printf("Server:%s\n",recvBuf);
    if(strcmp(recvBuf,"no")==0)
    {
        perror("密码或者用户名错误");
        return -1;
    }
    return 0;
}

/*void chatroom()
{

    //发送用户名和密码过去
    if(send(sockfd,(char *)&use,sizeof(struct user),0)==-1)
    {
        perror("fail to send datas.");
        exit(-1);
    }
    if((recvSize=recv(sockfd,recvBuf,MAX_BUF,0)==-1))
    {
        perror("fail to receive datas.");
        exit(-1);
    }
    //printf("Server:%s\n",recvBuf);
    if(strcmp(recvBuf,"no")==0)
    {
        perror("密码或者用户名错误");
        exit(-1);
    }


    //send-recv
    if((pid=fork())<0)
    {
        perror("fork error\n");
    }
    else if(pid==0)
    {
        while(1)
        {
	    printf("%s:",use.name);
            fgets(sendBuf,MAX_BUF,stdin);
            if(send(sockfd,sendBuf,strlen(sendBuf),0)==-1)
            {
                perror("fail to receive datas.");
            }
           
	    memset(sendBuf,0,sizeof(sendBuf));
        }
    }
    else
    {
        while(1)
        {
            if((recvSize=recv(sockfd,recvBuf,MAX_BUF,0)==-1))
            {
                printf("Server maybe shutdown!");
                break;
            }
            printf("others:%s\n",recvBuf);
	    
            memset(recvBuf,0,sizeof(recvBuf));
        }
       // kill(pid,SIGKILL);
    }

    close(sockfd);
}*/

int main(int argc,char *argv[])
{
    int sockfd;//socket
    char sendBuf[MAX_BUF],recvBuf[MAX_BUF];
    int sendSize,recvSize;//用于记录记录发送和接收到数据的大小
    struct hostent * host;
    struct sockaddr_in servAddr;
    
    char * p;
    int pid;
    struct user use;

    if(argc != 2)
    {
        perror("use: ./client [hostname]");
        exit(-1);
    }
    //strcpy(use.name,argv[2]);
    //strcpy(use.pwd,argv[3]);
    //printf("username:%s\n",use.name);
    //printf("password:%s\n",use.pwd);
    host=gethostbyname(argv[1]);
    
    if(host==NULL)
    {
        perror("fail to get host by name.");
        exit(-1);
    }
    printf("Success to get host by name ...\n");

    //创建socket
    if((sockfd=socket(AF_INET,SOCK_STREAM,0))==-1)
    {
        perror("fail to establish a socket");
        exit(1);
    }
    printf("Success to establish a socket...\n");

    /*init sockaddr_in*/
    servAddr.sin_family=AF_INET;
    servAddr.sin_port=htons(SERVER_PORT);
    servAddr.sin_addr=*((struct in_addr *)host->h_addr);
    bzero(&(servAddr.sin_zero),8);

    /*connect the socket*/
    if(connect(sockfd,(struct sockaddr *)&servAddr,sizeof(struct sockaddr_in))==-1)
    {
        perror("fail to connect the socket");
        exit(1);
    }
    printf("Success to connect the socket...\n");


    while(1)
    {
	int nn = 1;
 	printf("1.register\n");
	printf("2.login\n");
	printf("3.exit\n");
	printf("input:");
	scanf("%d",&nn);
	if(nn == 1)
        {
	    printf("input username:");
	    scanf("%s",use.name);
	    printf("input password:");
	    scanf("%s",use.pwd);
	    if(regist(sockfd, use.name, use.pwd) == 0)
	    {
		printf("register success!\n");
	    }
	    else
	    {
	        printf("register failed!\n");
	    }
	    
	}
	else if(nn == 2)
        {
	    printf("input username:");
	    scanf("%s",use.name);
	    printf("input password:");
	    scanf("%s",use.pwd);
	    if(login(sockfd, use.name, use.pwd) == 0)
	    {
	   	printf("login success!\n");
		//chatroom();
 	    }
	    else
	    {
	    	printf("register failed!\n");
	    }
        }
	else
	    break;
    }
    return 0;
}
