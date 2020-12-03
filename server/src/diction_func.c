#include"server_head.h"

extern fd_set commonset;
extern sqlite3 * db;

void * Diction(void * arg)
{
		pthread_detach(pthread_self());
		int sockfd;
		char cmd[MAX_CMD];
		char dictime[MAX_TIME];
		SER ser;
		USR usr;
		struct sockaddr_in caddr;
		int len;
		char **result;
		int nrow;
		int ncolumn;
		char * errmsg;

		sockfd=*(int *)arg;
		free(arg);
		ser.sig=GET_DIC;
		write(sockfd,&ser,sizeof(ser));
		while(1)
		{
				if(read(sockfd,&usr,sizeof(usr))<=0)
				{
						printf("客户端主动断开连接\n");
						close(sockfd);
						pthread_exit(NULL);
				}
				if(usr.cmd==DIC_END)
						break;
				usr.data[strlen(usr.data)]=' ';
				usr.data[strlen(usr.data)+1]='\0';
				sprintf(cmd,"select explain from dictionary where word='%s'",usr.data);
				if(sqlite3_get_table(db,cmd,&result,&nrow,&ncolumn,&errmsg)!=SQLITE_OK)
				{
						printf("查找字典 操作失败\n");
						printf("%s\n",errmsg);
						ser.sig=SER_ERR;
						write(sockfd,&ser,sizeof(ser));
						FD_SET(sockfd,&commonset);
						kill(getpid(),SIGUSR2);
						pthread_exit(NULL);
				}
				if(nrow==0)
				{
						ser.sig=DIC_ERR;
				}
				else
				{
						ser.sig=DIC_OK;
						strcpy(ser.data,result[1]);
				}
				write(sockfd,&ser,sizeof(ser));
				time_t ti;
				ti=time(NULL);
				struct tm * t;
				t=localtime(&ti);
				sprintf(dictime,"%d-%02d-%02d %02d:%02d:%02d",t->tm_year+1900,t->tm_mon+1,t->tm_mday,t->tm_hour,t->tm_min,t->tm_sec);
				sprintf(cmd,"insert into history(name,word,dictime) values('%s','%s','%s')",usr.name,usr.data,dictime);
				if(sqlite3_exec(db,cmd,NULL,NULL,&errmsg)!=SQLITE_OK)
				{
						printf("增添查找记录失败\n");
						printf("%s\n",errmsg);
				}
		}
		FD_SET(sockfd,&commonset);
		kill(getpid(),SIGUSR2);
		pthread_exit(NULL);
}

void * History(void * arg)
{
		pthread_detach(pthread_self());
		int sockfd;
		char cmd[MAX_CMD];
		SER ser;
		USR usr;
		int index;
		char **result;
		int nrow;
		int ncolumn;
		char * errmsg;
		int i,j;

		sockfd=*(int *)arg;
		free(arg);
		ser.sig=GET_HIS;
		write(sockfd,&ser,sizeof(ser));
		if(read(sockfd,&usr,sizeof(usr))<=0)
		{
				printf("客户端主动断开连接\n");
				close(sockfd);
				pthread_exit(NULL);
		}
		if(usr.cmd==HIS_ALL)
		{
				sprintf(cmd,"select name from client where mode=2 and name='%s'",usr.name);
				if(sqlite3_get_table(db,cmd,&result,&nrow,&ncolumn,&errmsg)!=SQLITE_OK)
				{
						printf("确认权限 操作失败\n");
						printf("%s\n",errmsg);
						ser.sig=SER_ERR;
						write(sockfd,&ser,sizeof(ser));
						FD_SET(sockfd,&commonset);
						kill(getpid(),SIGUSR2);
						pthread_exit(NULL);
				}
				if(nrow==0)
				{
						ser.sig=HIS_UNA;
						write(sockfd,&ser,sizeof(ser));
						FD_SET(sockfd,&commonset);
						kill(getpid(),SIGUSR2);
						pthread_exit(NULL);
				}
				else
				{
						strcpy(cmd,"select * from history");
				}
		}
		else if(usr.cmd==HIS_SEF)
		{
				sprintf(cmd,"select word,dictime from history where name='%s'",usr.name);
		}
		else if(usr.cmd==HIS_CEL)
		{
				FD_SET(sockfd,&commonset);
				kill(getpid(),SIGUSR2);
				pthread_exit(NULL);
		}
		if(sqlite3_get_table(db,cmd,&result,&nrow,&ncolumn,&errmsg)!=SQLITE_OK)
		{
				printf("查找记录 操作失败\n");
				printf("%s\n",errmsg);
				ser.sig=SER_ERR;
				write(sockfd,&ser,sizeof(ser));
				FD_SET(sockfd,&commonset);
				kill(getpid(),SIGUSR2);
				pthread_exit(NULL);
		}
		if(nrow==0)
		{
				ser.sig=HIS_ERR;
				write(sockfd,&ser,sizeof(ser));
				FD_SET(sockfd,&commonset);
				kill(getpid(),SIGUSR2);
				pthread_exit(NULL);
		}
		else
		{
				ser.sig=HIS_OK;
				index=ncolumn;
				for(i=0;i<nrow;i++)
				{
						for(j=0;j<ncolumn;j++)
						{
								sprintf(ser.data,"%s>> %s",result[j],result[index++]);
								write(sockfd,&ser,sizeof(ser));
						}
						strcpy(ser.data,"----------------------");
						write(sockfd,&ser,sizeof(ser));
				}
				ser.sig=HIS_END;
				write(sockfd,&ser,sizeof(ser));
		}
		FD_SET(sockfd,&commonset);
		kill(getpid(),SIGUSR2);
		pthread_exit(NULL);
}
