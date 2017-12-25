#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h> //for gethostbyname
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>

#define MAX_BUF 4096
#define SERVER_PORT 12138

char listname[MAX_BUF];
char friendlist[MAX_BUF];
struct user
{
    int typ;
    char name[32];
    char pwd[32];
    char friendname[32];
    char sentence[200];
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
   // printf("Server:%s\n",recvBuf);
    if(strcmp(recvBuf,"no")==0)
    {
     //   printf("密码或者用户名错误");
        return -1;
    }
    return 0;
}

int add(int sockfd, char* username, char* friendname)
{
    char recvBuf[4];
    memset(recvBuf,0,sizeof(recvBuf));
    struct user use;
    use.typ = 3;
    strcpy(use.name, username);
    strcpy(use.friendname, friendname);
    if(send(sockfd,(char *)&use,sizeof(struct user),0)==-1)
    {
        printf(" add fail to send datas.");
        exit(-1);
    }

    if(recv(sockfd,recvBuf,MAX_BUF,0)==-1)
    {
        printf("add fail to receive datas.");
        exit(-1);
    }
   // printf("%s\n",recvBuf);
    if(strcmp(recvBuf,"no")==0)
    {
        return -1;
    }
    return 0;
}

void ls(int sockfd)
{
    struct user use;
    
    use.typ = 4;
    if(send(sockfd,(char *)&use,sizeof(struct user),0)==-1)
    {
        printf(" ls fail to send datas.");
	return;
    }

    if(recv(sockfd,listname,sizeof(listname),0)==-1)
    {
        printf("ls fail to receive datas.");
	return;
    }
   // printf("%s\n", listname);
    return;
}

void chat(int sockfd, char* usename, char* friendname)
{
    FILE* stream;
    char sendBuf[MAX_BUF];
    char sendmessage[MAX_BUF];
    char recvBuf[MAX_BUF];
    char filename[MAX_BUF];
    memset(recvBuf, 0, sizeof(recvBuf));

    struct user use;
    use.typ = 6;
    memset(use.name, 0, sizeof(use.name));
    memset(use.friendname, 0, sizeof(use.friendname));
    strcpy(use.name, usename);
    strcpy(use.friendname, friendname);
    if(send(sockfd,(char *)&use,sizeof(struct user),0)==-1)
    {
        printf(" chat fail to send datas.");
	return;
    }
    
    if(recv(sockfd,recvBuf,sizeof(recvBuf),0)==-1)
    {
        printf(" chat fail to send datas.");
	return;
    }

    if(strcmp(recvBuf, "no") == 0)
    {
	printf("you are not friends\n");
	return;
    }
    printf("*******chat********\n");
    printf("1.sendmessage\n");
    printf("2.sendfile\n");
    printf("3.exit\n");

   

    int pid;

    //send-recv
    if((pid=fork())<0)
    {
        perror("fork error\n");
    }
    else if(pid==0)
    {
        while(1)
        { 
	    memset(sendBuf,0,sizeof(sendBuf));
     	    gets(sendBuf);
            char op[20];
 	    memset(op, 0, sizeof(op));
	    strncpy(op, sendBuf, 11);
 	    op[11] = '\0';
            if(strcmp(op, "sendmessage") == 0)
	    {
		memset(sendmessage, 0, sizeof(sendmessage));
		strcpy(sendmessage, sendBuf+11);
	      //  printf("%s\n", sendmessage);
	        
   	        use.typ = 5;
 		memset(use.name, 0, sizeof(use.name));
        	memset(use.friendname, 0, sizeof(use.friendname));
		memset(use.sentence, 0, sizeof(use.sentence));	        
		strcpy(use.name, usename);
   	        strcpy(use.friendname, friendname);
		strcpy(use.sentence, sendmessage);

      	        if(send(sockfd,(char *)&use,sizeof(struct user),0)==-1)
  	        {
        		printf(" chat fail to send datas.");
	         	return;
    	        }
	    	continue;
	    }

	    memset(op, 0, sizeof(op));
	    strncpy(op, sendBuf, 8);
 	    op[8] = '\0';
	    if(strcmp(op, "sendfile") == 0)
	    {
		use.typ = 9;
 		memset(use.name, 0, sizeof(use.name));
		memset(filename, 0, sizeof(filename));

		strcpy(use.name, usename);
		if(send(sockfd,(char *)&use,sizeof(struct user),0)==-1)
  	        {
        		printf(" chat fail to send datas.");
	         	return;
    	        }

		strcpy(filename, sendBuf+9);
		if((stream = fopen(filename, "r")) == NULL)
		{
		    printf("the file is not exit\n");
		    continue;
		}
		
		memset(sendBuf, 0, sizeof(sendBuf)); 
      		int length = 0;  
      		while((length = fread(buffer, sizeof(char), sizeof(sendBuf), fp)) > 0) 
      		{ 
       	 	    if(send(sockfd, buffer, length, 0) < 0) 
        	    { 
          		printf("Send File:%s Failed./n", filename); 
          		break; 
        	    } 
        	    memset(sendBuf, 0, sizeof(sendBuf));  
		}
		continue;
	    }

	    memset(op, 0, sizeof(op));
	    strncpy(op, sendBuf, 4);
 	    op[4] = '\0';
	    if(strcmp(op, "exit") == 0)
	    {
		use.typ = 5;
 		memset(use.name, 0, sizeof(use.name));
		strcpy(use.name, "root");
		if(send(sockfd,(char *)&use,sizeof(struct user),0)==-1)
  	        {
        		printf(" chat fail to send datas.");
	         	return;
    	        }
		return;
	    }
        }
    }
    else
    {
        while(1)
        {
	    struct user use;
	    memset(use.name,0,sizeof(use.name));
	    memset(use.sentence,0,sizeof(use.sentence));
            if(recv(sockfd,(char *)&use,sizeof(struct user),0)==-1)
            {
                printf("Server maybe shutdown!");
                break;
            }
	    if(strcmp(use.name,"root") == 0)
	    {
		kill(pid, SIGKILL);
		return;		
	    }
            if(use.typ == 5)
	    {
		printf("%s:%s\n", use.name, use.sentence);
		continue;
	    }
	    
            
        }
    }
    
    return;
    
}

void recvmessage(int sockfd, char* username)
{
    char revBuf[40960];
    memset(revBuf, 0, sizeof(revBuf));
    struct user use;
    use.typ = 7;
    memset(use.name, 0, sizeof(use.name));
    strcpy(use.name, username);
    if(send(sockfd,(char *)&use,sizeof(struct user),0)==-1)
    {
        printf(" revm fail to send datas.");
	return;
    }

    if(recv(sockfd,revBuf,sizeof(revBuf),0)==-1)
    {
        printf("revm fail to receive datas.");
	return;
    }
    printf("%s\n", revBuf);

    return;
}

void sync(int sockfd, char* username)
{
    struct user use;
    use.typ = 8;
    memset(use.name, 0, sizeof(use.name));
    strcpy(use.name, username);
    if(send(sockfd,(char *)&use,sizeof(struct user),0)==-1)
    {
        printf(" sync fail to send datas.");
	return;
    }

    if(recv(sockfd,friendlist,sizeof(friendlist),0)==-1)
    {
        printf("sync fail to receive datas.");
	return;
    }
   // printf("%s\n", revBuf);

    return;
}

void chatroom(int sockfd, char* usename)
{
    
    while(1)
    {
	printf("*******chatroom********\n");
    	printf("1. add\n");
   	printf("2. ls\n");
   	printf("3. chat\n");
    	printf("4. recvmessage\n");
    	printf("5. profile\n");
   	printf("6. sync\n");
    	printf("7. exit\n");
        int n = 0;
        printf("input:");
        scanf("%d",&n);

        if(n == 1)
	{
	    char name[32];
	    printf("input the username:");
            scanf("%s", name);
            if(add(sockfd, usename, name) == 0)
		printf("add success\n");
	    else
		printf("add failed\n");
	    continue;
	}
	
	if(n == 2)
  	{
	    memset(listname,0,sizeof(listname));
	    ls(sockfd);
	    printf("all username:");
	    printf("%s\n",listname);
  	    continue;
	}
	
	if(n == 3)
	{
	   char name[32];
	   printf("input the username:"); 
	   scanf("%s", name);
	   chat(sockfd, usename, name);	
	   continue;
	}
        if(n == 4)
	{
	    recvmessage(sockfd, usename);
	    continue;
	}
	if(n == 5)
	{
	    printf("current user:%s\n", usename);
	    printf("friendlist\n");
	    printf("%s", friendlist);
	    continue;
	}
	if(n == 6)
	{
	    sync(sockfd, usename);
	    continue;
	}
	if(n == 7)
	    break;
    }
    return;
    
}


int main(int argc,char *argv[])
{
    memset(friendlist, 0, sizeof(friendlist));
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
		chatroom(sockfd, use.name);
 	    }
	    else
	    {
	    	printf("login failed!\n");
	    }
        }
	else
	    break;
    }
    return 0;
}
