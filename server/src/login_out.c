#include"server_head.h"

extern fd_set commonset;
extern sqlite3 * db;
extern int udpfd;

void * Register(void * arg)
{
		pthread_detach(pthread_self());
		int sockfd;
		char cmd[MAX_CMD];
		char logintime[MAX_TIME];
		SER ser;
		USR usr;
		struct sockaddr_in caddr;
		int len;
		len=sizeof(struct sockaddr_in);
		char **result;
		int nrow;
		int ncolumn;
		char * errmsg;


		sockfd=*(int *)arg;
		free(arg);
		if(getpeername(sockfd,(struct sockaddr *)&caddr,&len)<0)
		{
				printf("获取注册用户地址失败\n");
				perror("getpeername failed");
				ser.sig=SER_ERR;
				write(sockfd,&ser,sizeof(ser));
				FD_SET(sockfd,&commonset);
				kill(getpid(),SIGUSR2);
				pthread_exit(NULL);
		}
		ser.sig=GET_REG;
		write(sockfd,&ser,sizeof(ser));
		while(1)
		{
				if(read(sockfd,&usr,sizeof(usr))<=0)
				{
						printf("客户端主动断开连接\n");
						close(sockfd);
						pthread_exit(NULL);
				}
				sprintf(cmd,"select name from client where name='%s'",usr.name);
				if(sqlite3_get_table(db,cmd,&result,&nrow,&ncolumn,&errmsg)!=SQLITE_OK)
				{
						printf("检查用户名是否重复 操作失败\n");
						printf("%s\n",errmsg);
						ser.sig=SER_ERR;
						write(sockfd,&ser,sizeof(ser));
						FD_SET(sockfd,&commonset);
						kill(getpid(),SIGUSR2);
						pthread_exit(NULL);
				}
				if(nrow==0)
				{
						ser.sig=NAME_OK;
						write(sockfd,&ser,sizeof(ser));
						break;
				}
				else
				{
						ser.sig=NAME_ERROR;
						write(sockfd,&ser,sizeof(ser));
				}
		}
		if(read(sockfd,&usr,sizeof(usr))<=0)
		{
				printf("客户端主动断开连接\n");
				close(sockfd);
				pthread_exit(NULL);
		}
		time_t ti;
		ti=time(NULL);
		struct tm * t;
		t=localtime(&ti);
		sprintf(logintime,"%d-%02d-%02d %02d:%02d:%02d",t->tm_year+1900,t->tm_mon+1,t->tm_mday,t->tm_hour,t->tm_min,t->tm_sec);
		sprintf(cmd,"insert into client(name,addr,port,pwd,logintime) values('%s','%s',%d,'%s','%s')",usr.name,inet_ntoa(caddr.sin_addr),caddr.sin_port,usr.data,logintime);
		if(sqlite3_exec(db,cmd,NULL,NULL,&errmsg)!=SQLITE_OK)
		{
				printf("注册新用户失败\n");
				printf("%s\n",errmsg);
				ser.sig=SER_ERR;
				write(sockfd,&ser,sizeof(ser));
				FD_SET(sockfd,&commonset);
				kill(getpid(),SIGUSR2);
				pthread_exit(NULL);
		}
		ser.sig=REG_OK;
		write(sockfd,&ser,sizeof(ser));
		printf("一位用户已注册并登录--->用户名:%s\n",usr.name);
		FD_SET(sockfd,&commonset);
		kill(getpid(),SIGUSR2);
		pthread_exit(NULL);
}

void * Login(void * arg)
{
		pthread_detach(pthread_self());
		int sockfd;
		char cmd[MAX_CMD];
		char logintime[MAX_TIME];
		SER ser;
		USR usr;
		struct sockaddr_in caddr;
		int len;
		len=sizeof(struct sockaddr_in);
		char ** result;
		char ** result1;
		int nrow;
		int ncolumn;
		char * errmsg;
		char * p;
		int count=0;
		int i,j;
		char caredname[MAX_NAME];
		struct sockaddr_in caddr1;
		int len1;
		len1=sizeof(struct sockaddr_in);
		caddr1.sin_family=PF_INET;
		caddr1.sin_port=htons(SERVER_UDP_PORT);

		sockfd=*(int *)arg;
		free(arg);
		if(getpeername(sockfd,(struct sockaddr *)&caddr,&len)<0)
		{
				printf("获取登录用户地址失败\n");
				perror("getpeername failed");
				ser.sig=SER_ERR;
				write(sockfd,&ser,sizeof(ser));
				FD_SET(sockfd,&commonset);
				kill(getpid(),SIGUSR2);
				pthread_exit(NULL);
		}
		ser.sig=GET_LOG;
		write(sockfd,&ser,sizeof(ser));
		if(read(sockfd,&usr,sizeof(usr))<=0)
		{
				printf("客户端主动断开连接\n");
				close(sockfd);
				pthread_exit(NULL);
		}
		sprintf(cmd,"select name from client where name='%s'",usr.name);
		if(sqlite3_get_table(db,cmd,&result,&nrow,&ncolumn,&errmsg)!=SQLITE_OK)
		{
				printf("检查用户名是否存在 操作失败\n");
				printf("%s\n",errmsg);
				ser.sig=SER_ERR;
				write(sockfd,&ser,sizeof(ser));
				FD_SET(sockfd,&commonset);
				kill(getpid(),SIGUSR2);
				pthread_exit(NULL);
		}
		if(nrow==0)
		{
				ser.sig=LOG_ERR;
				write(sockfd,&ser,sizeof(ser));
				FD_SET(sockfd,&commonset);
				kill(getpid(),SIGUSR2);
				pthread_exit(NULL);
		}
		else
		{
				ser.sig=LOG_PWD;
				write(sockfd,&ser,sizeof(ser));
				if(read(sockfd,&usr,sizeof(usr))<=0)
				{
						printf("客户端主动断开连接\n");
						close(sockfd);
						pthread_exit(NULL);
				}
				sprintf(cmd,"select * from client where name='%s' and pwd='%s'",usr.name,usr.data);
				if(sqlite3_get_table(db,cmd,&result,&nrow,&ncolumn,&errmsg)!=SQLITE_OK)
				{
						printf("检查密码是否正确 操作失败\n");
						printf("%s\n",errmsg);
						ser.sig=SER_ERR;
						write(sockfd,&ser,sizeof(ser));
						FD_SET(sockfd,&commonset);
						kill(getpid(),SIGUSR2);
						pthread_exit(NULL);
				}
				if(nrow==0)
				{
						ser.sig=PWD_ERR;
						write(sockfd,&ser,sizeof(ser));
						FD_SET(sockfd,&commonset);
						kill(getpid(),SIGUSR2);
						pthread_exit(NULL);
				}
				else
				{
						ser.sig=PWD_OK;
						write(sockfd,&ser,sizeof(ser));
						time_t ti;
						ti=time(NULL);
						struct tm * t;
						t=localtime(&ti);
						sprintf(logintime,"%d-%02d-%02d %02d:%02d:%02d",t->tm_year+1900,t->tm_mon+1,t->tm_mday,t->tm_hour,t->tm_min,t->tm_sec);
						sprintf(cmd,"update client set addr='%s',port=%d,stat=%d,logintime='%s',chat=0 where name='%s'",inet_ntoa(caddr.sin_addr),caddr.sin_port,1,logintime,usr.name);
						if(sqlite3_exec(db,cmd,NULL,NULL,&errmsg)!=SQLITE_OK)
						{
								printf("用户登陆失败\n");
								printf("%s\n",errmsg);
								ser.sig=SER_ERR;
								write(sockfd,&ser,sizeof(ser));
								FD_SET(sockfd,&commonset);
								kill(getpid(),SIGUSR2);
								pthread_exit(NULL);
						}
						ser.sig=LOG_OK;
						write(sockfd,&ser,sizeof(ser));
						printf("一位用户已登录--->用户名:%s\n",usr.name);
						ser.sig=SYS_INFO;
						sprintf(cmd,"select cared from client where name='%s'",usr.name);
						if(sqlite3_get_table(db,cmd,&result,&nrow,&ncolumn,&errmsg)!=SQLITE_OK)
						{
								printf("遍历被特别关心列表 操作失败\n");
								printf("%s\n",errmsg);
								FD_SET(sockfd,&commonset);
								kill(getpid(),SIGUSR2);
								pthread_exit(NULL);
						}
						if(result[1]!=NULL)
						{
								p=result[1];
								while(*p!='\0')
								{
										if(*p==' ')
												count++;
								}
								p=result[1];
								for(i=0;i<=count;i++)
								{
										for(j=0;*p!=' '&&*p!='\0';j++)
										{
												caredname[j]=*p++;
										}	
										p++;
										sprintf(cmd,"select addr from client where stat=1 and name='%s' and care=1",caredname);
										if(sqlite3_get_table(db,cmd,&result1,&nrow,&ncolumn,&errmsg)!=SQLITE_OK)
										{
												printf("获取被特别关心用户ip 操作失败\n");
												printf("%s\n",errmsg);
												FD_SET(sockfd,&commonset);
												kill(getpid(),SIGUSR2);
												pthread_exit(NULL);
										}
										if(nrow!=0)
										{
												caddr.sin_addr.s_addr=inet_addr(result1[1]);
												sprintf(ser.data,"/attention/ : 特别关心提醒: %s已上线\n",usr.name);
												sendto(udpfd,&ser,sizeof(ser),0,(struct sockaddr *)&caddr1,len1);
										}
								}
						}
						FD_SET(sockfd,&commonset);
						kill(getpid(),SIGUSR2);
						pthread_exit(NULL);
				}
		}

}

void * Logout(void * arg)
{
		pthread_detach(pthread_self());
		int sockfd;
		char cmd[MAX_CMD];
		char logouttime[MAX_TIME];
		SER ser;
		USR usr;
		char * errmsg;
		char * p;
		int count=0;
		int i,j;
		char caredname[MAX_NAME];
		struct sockaddr_in caddr;
		int len;
		char ** result;
		char ** result1;
		int nrow;
		int ncolumn;
		len=sizeof(struct sockaddr_in);
		caddr.sin_family=PF_INET;
		caddr.sin_port=htons(SERVER_UDP_PORT);

		sockfd=*(int *)arg;
		free(arg);
		ser.sig=GET_LOU;
		write(sockfd,&ser,sizeof(ser));
		if(read(sockfd,&usr,sizeof(usr))<=0)
		{
				printf("客户端主动断开连接\n");
				close(sockfd);
				pthread_exit(NULL);
		}
		time_t ti;
		ti=time(NULL);
		struct tm * t;
		t=localtime(&ti);
		sprintf(logouttime,"%d-%02d-%02d %02d:%02d:%02d",t->tm_year+1900,t->tm_mon+1,t->tm_mday,t->tm_hour,t->tm_min,t->tm_sec);
		sprintf(cmd,"update client set stat=%d,logouttime='%s',chat=0 where name='%s'",0,logouttime,usr.name);
		if(sqlite3_exec(db,cmd,NULL,NULL,&errmsg)!=SQLITE_OK)
		{
				printf("用户登出失败\n");
				printf("%s\n",errmsg);
				ser.sig=SER_ERR;
		}
		else
		{
				ser.sig=LOU_OK;
		}
		write(sockfd,&ser,sizeof(ser));
		close(sockfd);
		printf("一位用户已下线--->用户名:%s\n",usr.name);
		ser.sig=SYS_INFO;
		sprintf(cmd,"select cared from client where name='%s'",usr.name);
		if(sqlite3_get_table(db,cmd,&result,&nrow,&ncolumn,&errmsg)!=SQLITE_OK)
		{
				printf("遍历被特别关心列表 操作失败\n");
				printf("%s\n",errmsg);
				FD_SET(sockfd,&commonset);
				kill(getpid(),SIGUSR2);
				pthread_exit(NULL);
		}
		if(result[1]!=NULL)
		{
				p=result[1];
				while(*p!='\0')
				{
						if(*p==' ')
								count++;
				}
				p=result[1];
				for(i=0;i<count+1;i++)
				{
						for(j=0;*p!=' '&&*p!='\0';j++)
						{
								caredname[j]=*p++;
						}	
						p++;
						sprintf(cmd,"select addr from client where stat=1 and name='%s' and care=1",caredname);
						if(sqlite3_get_table(db,cmd,&result1,&nrow,&ncolumn,&errmsg)!=SQLITE_OK)
						{
								printf("获取被特别关心用户ip 操作失败\n");
								printf("%s\n",errmsg);
								FD_SET(sockfd,&commonset);
								kill(getpid(),SIGUSR2);
								pthread_exit(NULL);
						}
						if(nrow!=0)
						{
								caddr.sin_addr.s_addr=inet_addr(result1[1]);
								sprintf(ser.data,"/attention/ : 特别关心提醒: %s已下线\n",usr.name);
								sendto(udpfd,&ser,sizeof(ser),0,(struct sockaddr *)&caddr,len);
						}
				}
		}
		pthread_exit(NULL);
}
