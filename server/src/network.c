#include"server_head.h"

extern char adv_flag;
extern sem_t sem;
extern char adv_per_time;
extern int udpfd;
extern char advbuf[MAX_DATA];
extern sqlite3 * db;

void * Tell_client_ip(void * arg)
{
		int tellipfd,slen,clen,ret;
		struct sockaddr_in saddr ,caddr;
		USR usr;
		SER ser;

		tellipfd = socket(PF_INET,SOCK_DGRAM,0);
		if(tellipfd < 0)
		{
				perror("客户端自动连接服务出错(socket)\n");
				pthread_exit(NULL);
		}	
		saddr.sin_family = PF_INET;
		saddr.sin_port   = htons(SERVER_IP_PORT);
		saddr.sin_addr.s_addr = INADDR_ANY;		
		slen = sizeof(struct sockaddr_in);
		if(bind(tellipfd,(struct sockaddr* )&saddr,slen)<0)
		{
				perror("客户端自动连接服务出错(bind)\n");
				pthread_exit(NULL);
		}
		clen = sizeof(struct sockaddr_in);
		ser.sig=SERVER_IP;
		strcpy(ser.data,(char *)arg);
		while(1)
		{
				ret=recvfrom(tellipfd,&usr,sizeof(usr),0,(struct sockaddr*)&caddr,&clen);
				if(ret<0)
						continue;
				if(usr.cmd==GET_SERVER_IP)
				{
						printf("get an ip request\n");
						sendto(tellipfd,&ser,sizeof(ser),0,(struct sockaddr*)&caddr,clen);
				}
		}
}

void * Adv(void * arg)
{
		pthread_detach(pthread_self());
		int nrow;
		int ncolumn;
		char ** result;
		char * errmsg;
		int index;
		int i,j;
		SER ser;
		ser.sig=SYS_INFO;
		struct sockaddr_in caddr;
		caddr.sin_family=PF_INET;
		caddr.sin_port=htons(SERVER_UDP_PORT);
		int len;
		len=sizeof(struct sockaddr_in);
		//先测试一下数据库是否正常
		char cmd[MAX_CMD]="select addr from client where stat=1 and adv=1 and game=0";
		if(sqlite3_get_table(db,cmd,&result,&nrow,&ncolumn,&errmsg)!=SQLITE_OK)
		{
				printf("初始化推送服务失败\n");
				adv_flag=0;
				printf("%s\n",errmsg);
				sem_post(&sem);
				pthread_exit(NULL);
		}
		else
		{
				printf("初始化推送服务成功\n");
				adv_flag=1;
				printf("请设置定时广播内容\n");
				fgets(advbuf,MAX_DATA,stdin);
				advbuf[strlen(advbuf)-1]='\0';
		}
		//释放信号量，主线程继续运行
		sem_post(&sem);
		while(1)
		{
				sleep(adv_per_time);
				if(sqlite3_get_table(db,cmd,&result,&nrow,&ncolumn,&errmsg)!=SQLITE_OK)
				{
						printf("推送服务发生故障，已自动关闭\n");
						printf("%s\n",errmsg);
						adv_flag=0;
						pthread_exit(NULL);
				}
				if(nrow==0)
						continue;
				strcpy(ser.data,advbuf);
				index=ncolumn;
				for(i=0;i<nrow;i++)
				{
						caddr.sin_addr.s_addr=inet_addr(result[index++]);
						sendto(udpfd,&ser,sizeof(ser),0,(struct sockaddr *)&caddr,len);
				}
		}
}

unsigned long My_inet_addr(char * addr)
{
		unsigned long s_addr=0;
		char p[4];
		int i,j=0,k;
		for(k=0;k<4;k++)
		{
				for(i=0;*addr!='.'&&*addr!='\0';i++)
						p[i]=*addr++;
				p[i]='\0';
				s_addr|=atoi(p)<<(8*j);
				j++;
				addr++;
		}
		return s_addr;
}

int Init_fd(char * arg)
{
		int sockfd,slen;
		struct sockaddr_in saddr;
		sockfd=socket(PF_INET,SOCK_STREAM,0);
		if(sockfd<0)
		{
				printf("服务器遇到致命错误\n");
				perror("socket failed\n");
				exit(0);
		}
		saddr.sin_family=PF_INET;
		saddr.sin_port=htons(SERVER_TCP_PORT);
		saddr.sin_addr.s_addr=My_inet_addr(arg);
		slen=sizeof(struct sockaddr_in);
		if(bind(sockfd,(struct sockaddr *)&saddr,slen)<0)
		{
				printf("服务器遇到致命错误\n");
				perror("bind failed\n");
				close(sockfd);
				exit(0);
		}
		if(listen(sockfd,50)<0)
		{
				printf("服务器遇到致命错误\n");
				perror("listen failed");
				exit(0);
		}
		udpfd=socket(PF_INET,SOCK_DGRAM,0);
		if(udpfd<0)
		{
				printf("服务器遇到致命错误\n");
				perror("socket failed\n");
				exit(0);
		}
		return sockfd;
}

int Get_current_ip(char * addr)
{
		system("./get_ip.sh");
		int fd;
		fd=open("./ip.config",O_RDONLY);
		if(fd<0)
		{
				perror(" ");
				return -1;
		}
		char buf[128]={0};
		int ret;
		ret=read(fd,buf,sizeof(buf));
		if(ret<=0)
		{
				perror(" ");
				return -1;
		}
		char * p;
		int i;
		p=buf;
		while(*p==' ')
				p++;
		if(strncmp(p,"inet addr",9)==0)
		{
				p+=10;
				for(i=0;*p!=' ';i++)
						addr[i]=*p++;
				addr[i]='\0';
				return 0;
		}
		else
		{
				printf("网络不通,请检查网络\n");
				return -1;
		}
}
