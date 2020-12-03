#include"server_head.h"

extern sqlite3 * db;
extern fd_set commonset;
extern int udpfd;

void * Joinchat(void * arg)
{
		pthread_detach(pthread_self());
		int sockfd;
		char cmd[MAX_CMD];
		SER ser;
		USR usr;
		char ** result;
		int nrow;
		int ncolumn;
		char * errmsg;

		sockfd=*(int *)arg;
		free(arg);
		ser.sig=GET_JCH;
		write(sockfd,&ser,sizeof(ser));
		if(read(sockfd,&usr,sizeof(usr))<=0)
		{
				printf("客户端主动断开连接\n");
				close(sockfd);
				pthread_exit(NULL);
		}
		sprintf(cmd,"update client set chat=1 where name='%s'",usr.name);
		if(sqlite3_exec(db,cmd,NULL,NULL,&errmsg)!=SQLITE_OK)
		{
				printf("修改用户聊天标记 操作失败\n");
				printf("%s\n",errmsg);
				ser.sig=SER_ERR;
		}
		else
		{
				ser.sig=JCH_OK;
		}
		write(sockfd,&ser,sizeof(ser));
		FD_SET(sockfd,&commonset);
		kill(getpid(),SIGUSR2);
		pthread_exit(NULL);
}

void * Exitchat(void * arg)
{
		pthread_detach(pthread_self());
		int sockfd;
		char cmd[MAX_CMD];
		SER ser;
		USR usr;
		char ** result;
		int nrow;
		int ncolumn;
		char * errmsg;

		sockfd=*(int *)arg;
		free(arg);
		ser.sig=GET_ECH;
		write(sockfd,&ser,sizeof(ser));
		if(read(sockfd,&usr,sizeof(usr))<=0)
		{
				printf("客户端主动断开连接\n");
				close(sockfd);
				pthread_exit(NULL);
		}
		sprintf(cmd,"update client set chat=0 where name='%s'",usr.name);
		if(sqlite3_exec(db,cmd,NULL,NULL,&errmsg)!=SQLITE_OK)
		{
				printf("修改用户聊天标记 操作失败\n");
				printf("%s\n",errmsg);
				ser.sig=SER_ERR;
		}
		else
		{
				ser.sig=ECH_OK;
		}
		write(sockfd,&ser,sizeof(ser));
		FD_SET(sockfd,&commonset);
		kill(getpid(),SIGUSR2);
		pthread_exit(NULL);
}

void * Chatlist(void * arg)
{
		pthread_detach(pthread_self());
		int sockfd;
		char cmd[MAX_CMD];
		SER ser;
		USR usr;
		char ** result;
		int nrow;
		int ncolumn;
		char * errmsg;
		int index;
		int i,j;

		sockfd=*(int *)arg;
		free(arg);
		ser.sig=GET_CHL;
		write(sockfd,&ser,sizeof(ser));
		sprintf(cmd,"select name from client where chat=1");
		if(sqlite3_get_table(db,cmd,&result,&nrow,&ncolumn,&errmsg)!=SQLITE_OK)
		{
				printf("获取聊天室用户列表 操作失败\n");
				printf("%s\n",errmsg);
				ser.sig=SER_ERR;
				write(sockfd,&ser,sizeof(ser));
				FD_SET(sockfd,&commonset);
				kill(getpid(),SIGUSR2);
				pthread_exit(NULL);
		}
		if(nrow==1)
		{
				ser.sig=CHL_ERR;
				write(sockfd,&ser,sizeof(ser));
				FD_SET(sockfd,&commonset);
				kill(getpid(),SIGUSR2);
				pthread_exit(NULL);
		}
		else
		{
				int count=0;
				ser.sig=CHL_OK;
				write(sockfd,&ser,sizeof(ser));
				index=ncolumn;
				for(i=0;i<nrow;i++)
				{
						for(j=0;j<ncolumn;j++)
						{
								sprintf(ser.data,"%s: %s",result[j],result[index++]);
								write(sockfd,&ser,sizeof(ser));
								count++;
						}
						strcpy(ser.data,"-------------------");
						write(sockfd,&ser,sizeof(ser));
				}
				ser.sig=CHL_END;
				write(sockfd,&ser,sizeof(ser));
				ser.sig=count;
				write(sockfd,&ser,sizeof(ser));
		}
		FD_SET(sockfd,&commonset);
		kill(getpid(),SIGUSR2);
		pthread_exit(NULL);
}

void * Chatforall(void * arg)
{
		pthread_detach(pthread_self());
		int sockfd;
		char cmd[MAX_CMD];
		SER ser;
		USR usr;
		char ** result;
		int nrow;
		int ncolumn;
		char * errmsg;
		struct sockaddr_in caddr;
		caddr.sin_family=PF_INET;
		caddr.sin_port=htons(SERVER_UDP_PORT);
		int len;
		len=sizeof(struct sockaddr_in);
		char chattime[MAX_TIME];
		int index;
		int i,j;

		sockfd=*(int *)arg;
		free(arg);
		ser.sig=GET_CFA;
		write(sockfd,&ser,sizeof(ser));
		while(1)
		{
				if(read(sockfd,&usr,sizeof(usr))<=0)
				{
						printf("客户端主动断开连接\n");
						close(sockfd);
						pthread_exit(NULL);
				}
				if(usr.cmd==CFA_CEL)
						break;
				sprintf(cmd,"select addr from client where stat=1 and chat=1");
				if(sqlite3_get_table(db,cmd,&result,&nrow,&ncolumn,&errmsg)!=SQLITE_OK)
				{
						printf("获取聊天室成员IP 操作失败\n");
						printf("%s\n",errmsg);
						ser.sig=SER_ERR;
						write(sockfd,&ser,sizeof(ser));
						break;
				}
				if(nrow==1)
				{
						ser.sig=CFA_ERR;
						write(sockfd,&ser,sizeof(ser));
						continue;
				}
				sprintf(ser.data,"%s-> %s",usr.name,usr.data);
				ser.sig=BROA_INFO;
				index=ncolumn;
				for(i=0;i<nrow;i++)
				{
						caddr.sin_addr.s_addr=inet_addr(result[index++]);
						sendto(udpfd,&ser,sizeof(ser),0,(struct sockaddr *)&caddr,len);	
				}
				time_t ti;
				ti=time(NULL);
				struct tm * t;
				t=localtime(&ti);
				sprintf(chattime,"%d-%02d-%02d %02d:%02d:%02d",t->tm_year+1900,t->tm_mon+1,t->tm_mday,t->tm_hour,t->tm_min,t->tm_sec);
				sprintf(cmd,"insert into chat values('%s','broadcast','%s','%s')",usr.name,usr.data,chattime);
				if(sqlite3_exec(db,cmd,NULL,NULL,&errmsg)!=SQLITE_OK)
				{
						printf("添加群聊记录 操作失败\n");
						printf("%s\n",errmsg);
				}
				ser.sig=CFA_OK;
				write(sockfd,&ser,sizeof(ser));
		}
		FD_SET(sockfd,&commonset);
		kill(getpid(),SIGUSR2);
		pthread_exit(NULL);
}

void * Chatforone(void * arg)
{
		pthread_detach(pthread_self());
		int sockfd;
		char cmd[MAX_CMD];
		SER ser;
		USR usr;
		char ** result;
		int nrow;
		int ncolumn;
		char * errmsg;
		struct sockaddr_in caddr;
		caddr.sin_family=PF_INET;
		caddr.sin_port=htons(SERVER_UDP_PORT);
		int len;
		len=sizeof(struct sockaddr_in);
		char dstname[MAX_NAME];
		char chattime[MAX_TIME];

		sockfd=*(int *)arg;
		free(arg);
		ser.sig=GET_CFO;
		write(sockfd,&ser,sizeof(ser));
		while(1)
		{
				if(read(sockfd,&usr,sizeof(usr))<=0)
				{
						printf("客户端主动断开连接\n");
						close(sockfd);
						pthread_exit(NULL);
				}
				if(usr.cmd==CFO_CEL)
						printf("test\n");
				sprintf(cmd,"select addr from client where stat=1 and chat=1 and name='%s'",usr.data);
				if(sqlite3_get_table(db,cmd,&result,&nrow,&ncolumn,&errmsg)!=SQLITE_OK)
				{
						printf("确认目标是否在线 操作失败\n");
						printf("%s\n",errmsg);
						ser.sig=SER_ERR;
						write(sockfd,&ser,sizeof(ser));
						break;
				}
				if(nrow==0)
				{
						ser.sig=CFO_ERR;
						write(sockfd,&ser,sizeof(ser));
						break;
				}
				strcpy(dstname,usr.data);
				ser.sig=CFO_DATA;
				write(sockfd,&ser,sizeof(ser));
				if(read(sockfd,&usr,sizeof(usr))<=0)
				{
						printf("客户端主动断开连接\n");
						close(sockfd);
						pthread_exit(NULL);
				}
				if(usr.cmd==CFO_CEL)
						break;
				ser.sig=PRIV_INFO;
				sprintf(ser.data,"%s>>> %s",usr.name,usr.data);
				caddr.sin_addr.s_addr=inet_addr(result[1]);
				if(sendto(udpfd,&ser,sizeof(ser),0,(struct sockaddr *)&caddr,len)<0)
				{
						ser.sig=CFO_ERR;
						write(sockfd,&ser,sizeof(ser));
						break;
				}
				else
				{
						time_t ti;
						ti=time(NULL);
						struct tm * t;
						t=localtime(&ti);
						sprintf(chattime,"%d-%02d-%02d %02d:%02d:%02d",t->tm_year+1900,t->tm_mon+1,t->tm_mday,t->tm_hour,t->tm_min,t->tm_sec);
						sprintf(cmd,"insert into chat values('%s','%s','%s','%s')",usr.name,dstname,usr.data,chattime);
						if(sqlite3_exec(db,cmd,NULL,NULL,&errmsg)!=SQLITE_OK)
						{
								printf("添加私聊记录 操作失败\n");
								printf("%s\n",errmsg);
						}
						ser.sig=CFO_OK;
				}
				write(sockfd,&ser,sizeof(ser));
		}
		FD_SET(sockfd,&commonset);
		kill(getpid(),SIGUSR2);
		pthread_exit(NULL);
}

void * Chathistoryreturn(void * arg)
{
		pthread_detach(pthread_self());
		int sockfd;
		char cmd[MAX_CMD];
		SER ser;
		USR usr;
		char ** result;
		int nrow;
		int ncolumn;
		char * errmsg;
		int index;
		int i,j;

		sockfd=*(int *)arg;
		free(arg);
		ser.sig=GET_CHR;
		write(sockfd,&ser,sizeof(ser));
		printf("test2\n");
		if(read(sockfd,&usr,sizeof(usr))<=0)
		{
				printf("客户端主动断开连接\n");
				close(sockfd);
				pthread_exit(NULL);
		}
		sprintf(cmd,"select * from chat where srcname='%s' or dstname='%s'",usr.name,usr.name);
		if(sqlite3_get_table(db,cmd,&result,&nrow,&ncolumn,&errmsg)!=SQLITE_OK)
		{
				printf("获取用户聊天记录 操作失败\n");
				printf("%s\n",errmsg);
				ser.sig=SER_ERR;
				write(sockfd,&ser,sizeof(ser));
				FD_SET(sockfd,&commonset);
				kill(getpid(),SIGUSR2);
				pthread_exit(NULL);
		}
		if(nrow==0)
		{
				ser.sig=CHR_ERR;
				write(sockfd,&ser,sizeof(ser));
				FD_SET(sockfd,&commonset);
				kill(getpid(),SIGUSR2);
				pthread_exit(NULL);
		}
		ser.sig=CHR_OK;
		write(sockfd,&ser,sizeof(ser));
		index=ncolumn;
		for(i=0;i<nrow;i++)
		{
				for(j=0;j<ncolumn;j++)
				{
						sprintf(ser.data,"%s",result[index++]);
						write(sockfd,&ser,sizeof(ser));
				}
		}
		ser.sig=CHR_END;
		write(sockfd,&ser,sizeof(ser));
		FD_SET(sockfd,&commonset);
		kill(getpid(),SIGUSR2);
		pthread_exit(NULL);
}

void * Chatreport(void * arg)
{
		pthread_detach(pthread_self());
		char cmd[MAX_CMD];
		int sockfd;
		SER ser;
		USR usr;
		char targetname[MAX_NAME];
		char ** result;
		int nrow;
		int ncolumn;
		char * errmsg;

		sockfd=*(int *)arg;
		free(arg);
		ser.sig=GET_CRP;
		write(sockfd,&ser,sizeof(ser));
		if(read(sockfd,&usr,sizeof(usr))<=0)
		{
				printf("客户端主动断开连接\n");
				close(sockfd);
				pthread_exit(NULL);
		}
		if(usr.cmd==CRP_CEL)
		{
				write(sockfd,&ser,sizeof(ser));
				FD_SET(sockfd,&commonset);
				kill(getpid(),SIGUSR2);
				pthread_exit(NULL);
		}
		sprintf(cmd,"select name from client where name='%s'",usr.data);
		if(sqlite3_get_table(db,cmd,&result,&nrow,&ncolumn,&errmsg)!=SQLITE_OK)
		{
				printf("验证被举报人 操作失败\n");
				printf("%s\n",errmsg);
				ser.sig=SER_ERR;
				write(sockfd,&ser,sizeof(ser));
				FD_SET(sockfd,&commonset);
				kill(getpid(),SIGUSR2);
				pthread_exit(NULL);
		}
		if(nrow==0)
		{
				ser.sig=CRP_ERR;
				write(sockfd,&ser,sizeof(ser));
				FD_SET(sockfd,&commonset);
				kill(getpid(),SIGUSR2);
				pthread_exit(NULL);
		}
		strcpy(targetname,usr.data);
		ser.sig=CRP_DATA;
		write(sockfd,&ser,sizeof(ser));
		if(read(sockfd,&usr,sizeof(usr))<=0)
		{
				printf("客户端主动断开连接\n");
				close(sockfd);
				pthread_exit(NULL);
		}
		if(usr.cmd==CRP_CEL)
		{
				write(sockfd,&ser,sizeof(ser));
				FD_SET(sockfd,&commonset);
				kill(getpid(),SIGUSR2);
				pthread_exit(NULL);
		}
		printf("-----收到聊天举报信息-----\n举报人: <%s>\n被举报人: <%s>\n举报理由:{%s}",usr.name,targetname,usr.data);
		printf("请服务器管理员及时做出处理\n");
		ser.sig=CRP_OK;
		write(sockfd,&ser,sizeof(ser));
		FD_SET(sockfd,&commonset);
		kill(getpid(),SIGUSR2);
		pthread_exit(NULL);
}
