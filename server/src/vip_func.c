#include"server_head.h"

extern fd_set commonset;
extern sqlite3 * db;

void * Docmode(void * arg)
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
		ser.sig=GET_DMD;
		write(sockfd,&ser,sizeof(ser));
		if(read(sockfd,&usr,sizeof(usr))<=0)
		{
				printf("客户端主动断开连接\n");
				close(sockfd);
				pthread_exit(NULL);
		}
		sprintf(cmd,"select docmode from client where name='%s'",usr.name);
		if(sqlite3_get_table(db,cmd,&result,&nrow,&ncolumn,&errmsg)!=SQLITE_OK)
		{
				printf("确认用户文档上传下载特权 操作失败\n");
				printf("%s\n",errmsg);
				ser.sig=SER_ERR;
				write(sockfd,&ser,sizeof(ser));
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
						sprintf(cmd,"update client set docmode=1 where name='%s'",usr.name);
				}
				else if(ser.sig==MOD_HIG)
				{
						sprintf(cmd,"update client set docmode=0 where name='%s'",usr.name);
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

void * Caremode(void * arg)
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
		ser.sig=GET_CMD;
		write(sockfd,&ser,sizeof(ser));
		if(read(sockfd,&usr,sizeof(usr))<=0)
		{
				printf("客户端主动断开连接\n");
				close(sockfd);
				pthread_exit(NULL);
		}
		sprintf(cmd,"select care from client where name='%s'",usr.name);
		if(sqlite3_get_table(db,cmd,&result,&nrow,&ncolumn,&errmsg)!=SQLITE_OK)
		{
				printf("确认用户特别关心模式 操作失败\n");
				printf("%s\n",errmsg);
				ser.sig=SER_ERR;
				write(sockfd,&ser,sizeof(ser));
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
						sprintf(cmd,"update client set care=1 where name='%s'",usr.name);
				}
				else if(ser.sig==MOD_HIG)
				{
						sprintf(cmd,"update client set care=0 where name='%s'",usr.name);
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

void * Setcare(void * arg)
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
		ser.sig=GET_SCR;
		write(sockfd,&ser,sizeof(ser));
		while(1)
		{
				if(read(sockfd,&usr,sizeof(usr))<=0)
				{
						printf("客户端主动断开连接\n");
						close(sockfd);
						pthread_exit(NULL);
				}
				if(usr.cmd==SCR_CEL)
				{
						break;
				}
				sprintf(cmd,"select cared from client where name='%s'",usr.data);
				if(sqlite3_get_table(db,cmd,&result,&nrow,&ncolumn,&errmsg)!=SQLITE_OK)
				{
						printf("确认目标用户 操作失败\n");
						printf("%s\n",errmsg);
						ser.sig=SER_ERR;
						write(sockfd,&ser,sizeof(ser));
						break;
				}
				if(nrow==0)
				{
						ser.sig=SCR_ERR;
						write(sockfd,&ser,sizeof(ser));
						continue;
				}
				if(result[1]==NULL)
				{
						sprintf(cmd,"update client set cared='%s' where name='%s'",usr.name,usr.data);
				}
				else
				{
						strcat(result[1]," ");
						strcat(result[1],usr.name);
						sprintf(cmd,"update client set cared='%s' where name='%s'",result[1],usr.data);
				}
				if(sqlite3_exec(db,cmd,NULL,NULL,&errmsg)!=SQLITE_OK)
				{
						ser.sig=SER_ERR;
				}
				else
				{
						ser.sig=SCR_OK;
				}
				write(sockfd,&ser,sizeof(ser));
		}
		FD_SET(sockfd,&commonset);
		kill(getpid(),SIGUSR2);
		pthread_exit(NULL);
}

void * Supercheck(void * arg)
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
		ser.sig=GET_SCK;
		write(sockfd,&ser,sizeof(ser));
		if(read(sockfd,&usr,sizeof(usr))<=0)
		{
				printf("客户端主动断开连接\n");
				close(sockfd);
				pthread_exit(NULL);
		}
		sprintf(cmd,"select mode from client where name='%s'",usr.name);
		if(sqlite3_get_table(db,cmd,&result,&nrow,&ncolumn,&errmsg)!=SQLITE_OK)
		{
				printf("确认用户特权 操作失败\n");
				printf("%s\n",errmsg);
				ser.sig=SER_ERR;
				write(sockfd,&ser,sizeof(ser));
				FD_SET(sockfd,&commonset);
				kill(getpid(),SIGUSR2);
				pthread_exit(NULL);
		}
		if(atoi(result[1])>0)
		{
				ser.sig=SCK_OK;
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
						printf("确认密码 操作失败\n");
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
				ser.sig=PWD_OK;
		}
		else
				ser.sig=SCK_NO;
		write(sockfd,&ser,sizeof(ser));
		FD_SET(sockfd,&commonset);
		kill(getpid(),SIGUSR2);
		pthread_exit(NULL);
}
