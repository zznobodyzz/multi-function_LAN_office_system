#include"server_head.h"

extern fd_set commonset;
extern sqlite3 * db;
extern char modpwd[7];

void * Passwd(void * arg)
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
		ser.sig=GET_PWD;
		write(sockfd,&ser,sizeof(ser));
		if(read(sockfd,&usr,sizeof(usr))<=0)
		{
				printf("客户端主动断开连接\n");
				close(sockfd);
				pthread_exit(NULL);
		}
		sprintf(cmd,"select name from client where pwd='%s' and name='%s'",usr.data,usr.name);
		if(sqlite3_get_table(db,cmd,&result,&nrow,&ncolumn,&errmsg)!=SQLITE_OK)
		{
				printf("确认旧密码 操作失败\n");
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
		ser.sig=PWD_NEW;
		write(sockfd,&ser,sizeof(ser));
		if(read(sockfd,&usr,sizeof(usr))<=0)
		{
				printf("客户端主动断开连接\n");
				close(sockfd);
				pthread_exit(NULL);
		}
		sprintf(cmd,"update client set pwd='%s' where name='%s'",usr.data,usr.name);
		if(sqlite3_exec(db,cmd,NULL,NULL,&errmsg)!=SQLITE_OK)
		{
				printf("修改用户密码 操作失败\n");
				printf("%s\n",errmsg);
				ser.sig=SER_ERR;
		}
		else
		{
				ser.sig=PWD_OK;
		}
		write(sockfd,&ser,sizeof(ser));
		FD_SET(sockfd,&commonset);
		kill(getpid(),SIGUSR2);
		pthread_exit(NULL);
}

void * Onlinelist(void * arg)
{
		pthread_detach(pthread_self());
		int sockfd;
		char cmd[MAX_CMD];
		SER ser;
		USR usr;
		int index;
		int i,j;
		char **result;
		char * errmsg;
		int nrow;
		int ncolumn;

		sockfd=*(int *)arg;
		free(arg);
		ser.sig=GET_ONL;
		write(sockfd,&ser,sizeof(ser));
		if(read(sockfd,&usr,sizeof(usr))<=0)
		{
				printf("客户端主动断开连接\n");
				close(sockfd);
				pthread_exit(NULL);
		}
		if(usr.cmd==ONL_ALL||usr.cmd==ALL_ALL||usr.cmd==ALL_NAME)
		{
				strcpy(cmd,"select * from client where name='%s' and mode=2");
				if(sqlite3_get_table(db,cmd,&result,&nrow,&ncolumn,&errmsg)!=SQLITE_OK)
				{
						printf("确认用户权限 操作失败\n");
						printf("%s\n",errmsg);
						ser.sig=SER_ERR;
						write(sockfd,&ser,sizeof(ser));
						FD_SET(sockfd,&commonset);
						kill(getpid(),SIGUSR2);
						pthread_exit(NULL);
				}
				if(nrow==0)
				{
						ser.sig=ONL_ERR;
						write(sockfd,&ser,sizeof(ser));
						FD_SET(sockfd,&commonset);
						kill(getpid(),SIGUSR2);
						pthread_exit(NULL);
				}
				else
				{
						if(usr.cmd==ONL_ALL)
								strcpy(cmd,"select * from client where stat=1");
						else if(usr.cmd==ALL_ALL)
								strcpy(cmd,"select * from client");
						else if(usr.cmd==ALL_NAME)
								strcpy(cmd,"select no,name from client");
				}
		}
		else if(usr.cmd==ONL_NAME)
		{
				sprintf(cmd,"select no,name from client where stat=1");
		}
		else if(usr.cmd==ONL_CEL)
		{
				FD_SET(sockfd,&commonset);
				kill(getpid(),SIGUSR2);
				pthread_exit(NULL);
		}
		if(sqlite3_get_table(db,cmd,&result,&nrow,&ncolumn,&errmsg)!=SQLITE_OK)
		{
				printf("确认用户权限 操作失败\n");
				printf("%s\n",errmsg);
				ser.sig=SER_ERR;
				write(sockfd,&ser,sizeof(ser));
				FD_SET(sockfd,&commonset);
				kill(getpid(),SIGUSR2);
				pthread_exit(NULL);
		}
		if(nrow==0)
		{
				ser.sig=ONL_ERR;
				write(sockfd,&ser,sizeof(ser));
				FD_SET(sockfd,&commonset);
				kill(getpid(),SIGUSR2);
				pthread_exit(NULL);
		}
		else
		{
				ser.sig=ONL_OK;
				index=ncolumn;
				for(i=0;i<nrow;i++)
				{
						for(j=0;j<ncolumn;j++)
						{
								sprintf(ser.data,"%s: %s",result[j],result[index++]);
								write(sockfd,&ser,sizeof(ser));
						}
						strcpy(ser.data,"-------------------------");
						write(sockfd,&ser,sizeof(ser));
				}
				ser.sig=ONL_END;
				write(sockfd,&ser,sizeof(ser));
		}
		FD_SET(sockfd,&commonset);
		kill(getpid(),SIGUSR2);
		pthread_exit(NULL);
}

void * Advcel(void * arg)
{
		pthread_detach(pthread_self());
		int sockfd;
		char cmd[MAX_CMD];
		SER ser;
		USR usr;
		char * errmsg;
		char ** result;
		int nrow;
		int ncolumn;

		sockfd=*(int *)arg;
		free(arg);
		ser.sig=GET_DVC;
		write(sockfd,&ser,sizeof(ser));
		if(read(sockfd,&usr,sizeof(usr))<=0)
		{
				printf("客户端主动断开连接\n");
				close(sockfd);
				pthread_exit(NULL);
		}
		sprintf(cmd,"select * from client where name='%s' and mode>0",usr.name);
		if(sqlite3_get_table(db,cmd,&result,&nrow,&ncolumn,&errmsg)!=SQLITE_OK)
		{
				printf("确认用户权限 操作失败\n");
				printf("%s\n",errmsg);
				ser.sig=SER_ERR;
				write(sockfd,&ser,sizeof(ser));
				FD_SET(sockfd,&commonset);
				kill(getpid(),SIGUSR2);
				pthread_exit(NULL);
		}
		if(nrow==0)
		{
				ser.sig=DVC_UNA;
				write(sockfd,&ser,sizeof(ser));
				FD_SET(sockfd,&commonset);
				kill(getpid(),SIGUSR2);
				pthread_exit(NULL);
		}
		else
		{
				ser.sig=DVC_ABL;
				write(sockfd,&ser,sizeof(ser));
				sprintf(cmd,"select adv from client where name='%s'",usr.name);
				if(sqlite3_get_table(db,cmd,&result,&nrow,&ncolumn,&errmsg)!=SQLITE_OK)
				{
						printf("确认用户推送状态 操作失败\n");
						printf("%s\n",errmsg);
						ser.sig=SER_ERR;
						write(sockfd,&ser,sizeof(ser));
						FD_SET(sockfd,&commonset);
						kill(getpid(),SIGUSR2);
						pthread_exit(NULL);
				}
				if(atoi(result[1])==1)
				{
						sprintf(cmd,"update client set adv=0 where name='%s'",usr.name);
						ser.sig=DVC_OFF;
				}
				else 
				{
						sprintf(cmd,"update client set adv=1 where name='%s'",usr.name);
						ser.sig=DVC_ON;
				}
				if(sqlite3_exec(db,cmd,NULL,NULL,&errmsg)!=SQLITE_OK)
				{
						printf("修改推送服务 操作失败\n");
						printf("%s\n",errmsg);
						ser.sig=SER_ERR;
				}
				write(sockfd,&ser,sizeof(ser));
				FD_SET(sockfd,&commonset);
				kill(getpid(),SIGUSR2);
				pthread_exit(NULL);
		}
}

void * Score(void * arg)
{
		pthread_detach(pthread_self());
		int sockfd;
		char cmd[MAX_CMD];
		SER ser;
		USR usr;
		char * errmsg;

		sockfd=*(int *)arg;
		free(arg);
		ser.sig=GET_SCO;
		write(sockfd,&ser,sizeof(ser));
		if(read(sockfd,&usr,sizeof(usr))<=0)
		{
				printf("客户端主动断开连接\n");
				close(sockfd);
				pthread_exit(NULL);
		}
		sprintf(cmd,"update client set sco=%s where name='%s'",usr.data,usr.name);
		if(sqlite3_exec(db,cmd,NULL,NULL,&errmsg)!=SQLITE_OK)
		{
				printf("评分服务 操作失败\n");
				printf("%s\n",errmsg);
				ser.sig=SER_ERR;
		}else
		{
				ser.sig=SCO_OK;
		}
		write(sockfd,&ser,sizeof(ser));
		FD_SET(sockfd,&commonset);
		kill(getpid(),SIGUSR2);
		pthread_exit(NULL);
}

void * Mod(void * arg)
{
		pthread_detach(pthread_self());
		int sockfd;
		char cmd[MAX_CMD];
		SER ser;
		USR usr;
		char * errmsg;
		char ** result;
		int nrow;
		int ncolumn;
		sockfd=*(int *)arg;
		free(arg);
		ser.sig=GET_MOD;
		write(sockfd,&ser,sizeof(ser));
		if(read(sockfd,&usr,sizeof(usr))<=0)
		{
				printf("客户端主动断开连接\n");
				close(sockfd);
				pthread_exit(NULL);
		}
		if(strcmp(usr.data,modpwd)==0)
		{
				ser.sig=MOD_ABL;
				write(sockfd,&ser,sizeof(ser));
				sprintf(cmd,"select mode from client where name='%s'",usr.name);
				if(sqlite3_get_table(db,cmd,&result,&nrow,&ncolumn,&errmsg)!=SQLITE_OK)
				{
						ser.sig=SER_ERR;
						write(sockfd,&ser,sizeof(ser));
						printf("查询权限,操作失败\n");
						printf("%s\n",errmsg);
						FD_SET(sockfd,&commonset);
						kill(getpid(),SIGUSR2);
						pthread_exit(NULL);
				}
				if(atoi(result[1])==0)
				{
						ser.sig=MOD_LOW;
				}
				else if(atoi(result[1])==1)
				{
						ser.sig=MOD_HIG;
				}
				else if(atoi(result[1])==2)
				{
						ser.sig=MOD_ADM;
				}
				write(sockfd,&ser,sizeof(ser));
				if(read(sockfd,&usr,sizeof(usr))<=0)
				{
						printf("客户端主动断开连接\n");
						close(sockfd);
						pthread_exit(NULL);
				}
				if(usr.cmd==MOD_YES)
				{
						if(ser.sig==MOD_LOW)
						{
								sprintf(cmd,"update client set mode=1 where name='%s'",usr.name);
						}
						else if(ser.sig==MOD_HIG)
						{
								sprintf(cmd,"update client set mode=0 where name='%s'",usr.name);
						}
						else if(ser.sig==MOD_ADM)
						{
								sprintf(cmd,"update client set mode=1 where name='%s'",usr.name);
						}
						if(sqlite3_exec(db,cmd,NULL,NULL,&errmsg)!=SQLITE_OK)
						{
								ser.sig=SER_ERR;
								write(sockfd,&ser,sizeof(ser));
								printf("修改权限,操作失败\n");
								printf("%s\n",errmsg);
						}
						else
						{
								ser.sig=MOD_OK;
						}
						write(sockfd,&ser,sizeof(ser));
						FD_SET(sockfd,&commonset);
						kill(getpid(),SIGUSR2);
						pthread_exit(NULL);
				}
				else if(usr.cmd==MOD_NO)
				{
						FD_SET(sockfd,&commonset);
						kill(getpid(),SIGUSR2);
						pthread_exit(NULL);
				}
		}
		else 
		{
				ser.sig=MOD_UNA;
				write(sockfd,&ser,sizeof(ser));
				kill(getpid(),SIGUSR2);
				FD_SET(sockfd,&commonset);
				pthread_exit(NULL);
		}
}
