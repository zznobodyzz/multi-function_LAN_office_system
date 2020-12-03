#include"server_head.h"

extern sqlite3 * db;
extern fd_set commonset;
extern char doc_path[MAX_PATH];

void * Viewdoc(void * arg)
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
		ser.sig=GET_VDO;
		write(sockfd,&ser,sizeof(ser));
		if(read(sockfd,&usr,sizeof(usr))<=0)
		{
				printf("客户端主动断开连接\n");
				close(sockfd);
				pthread_exit(NULL);
		}
		if(usr.cmd==VDO_PRI)
		{
				sprintf(cmd,"select name,author,atime,dtimes from document where mode=1 and author='%s' and stat=1",usr.name);
		}
		else
		{
				sprintf(cmd,"select name,author,atime,dtimes from document where mode=0 and stat=1");
		}
		if(sqlite3_get_table(db,cmd,&result,&nrow,&ncolumn,&errmsg)!=SQLITE_OK)
		{
				printf("获取文档一览 操作失败\n");
				printf("%s\n",errmsg);
				ser.sig=SER_ERR;
				write(sockfd,&ser,sizeof(ser));
				FD_SET(sockfd,&commonset);
				kill(getpid(),SIGUSR2);
				pthread_exit(NULL);
		}
		if(nrow==0)
		{
				ser.sig=VDO_ERR;
				write(sockfd,&ser,sizeof(ser));
				FD_SET(sockfd,&commonset);
				kill(getpid(),SIGUSR2);
				pthread_exit(NULL);
		}
		else
		{
				ser.sig=VDO_OK;
				index=ncolumn;
				for(i=0;i<nrow;i++)
				{
						for(j=0;j<ncolumn;j++)
						{
								sprintf(ser.data,"%s: %s",result[j],result[index++]);
								write(sockfd,&ser,sizeof(ser));
						}
						strcpy(ser.data,"----------------------");
						write(sockfd,&ser,sizeof(ser));
				}
				ser.sig=VDO_END;
				write(sockfd,&ser,sizeof(ser));
		}
		FD_SET(sockfd,&commonset);
		kill(getpid(),SIGUSR2);
		pthread_exit(NULL);
}

void * Searchdoc(void * arg)
{
		pthread_detach(pthread_self());
		int sockfd;
		char cmd[MAX_CMD];
		char cmd2[MAX_CMD];
		char doctime[MAX_TIME];
		SER ser;
		USR usr;
		int index;
		char **result;
		int nrow;
		int ncolumn;
		char * errmsg;
		int i,j,k;
		char * p;
		struct tm * t;
		time_t ti;

		sockfd=*(int *)arg;
		free(arg);
		ser.sig=GET_SDO;
		write(sockfd,&ser,sizeof(ser));
		while(1)
		{
				if(read(sockfd,&usr,sizeof(usr))<=0)
				{
						printf("客户端主动断开连接\n");
						close(sockfd);
						pthread_exit(NULL);
				}
				if(usr.cmd==SDO_CEL)
				{
						FD_SET(sockfd,&commonset);
						kill(getpid(),SIGUSR2);
						pthread_exit(NULL);
				}
				ti=time(NULL);
				t=localtime(&ti);
				sprintf(doctime,"%d-%02d-%02d %02d:%02d:%02d",t->tm_year+1900,t->tm_mon+1,t->tm_mday,t->tm_hour,t->tm_min,t->tm_sec);
				if(usr.cmd==SDO_NAME_C)
				{
						sprintf(cmd,"select name,author,atime,dtimes,doc from document where name='%s' and mode=0 and stat=1",usr.data); 
						sprintf(cmd2,"insert into dochistory(name,type,docname,time) values('%s','namesearch','%s','%s')",usr.name,usr.data,doctime);
				}
				else if(usr.cmd==SDO_NAME_U)
				{
						sprintf(cmd,"select name,author,atime,dtimes,doc from document where name like '%%%s%%' and mode=0 and stat=1",usr.data);
						sprintf(cmd2,"insert into dochistory(name,type,docname,time) values('%s','namesearch','%s','%s')",usr.name,usr.data,doctime);
				}
				else if(usr.cmd==SDO_AUTH_C)
				{
						sprintf(cmd,"select name,author,atime,dtimes,doc from document where author='%s' and mode=0 and stat=1",usr.data);
						sprintf(cmd2,"insert into dochistory(name,type,docname,time) values('%s','authorsearch','%s','%s')",usr.name,usr.data,doctime);
				}
				else if(usr.cmd==SDO_AUTH_U)
				{
						sprintf(cmd,"select name,author,atime,dtimes,doc from document where author like '%%%s%%' and mode=0 and stat=1",usr.data);
						sprintf(cmd2,"insert into dochistory(name,type,docname,time) values('%s','authorsearch','%s','%s')",usr.name,usr.data,doctime);
				}
				else if(usr.cmd==SDO_TIME)
				{
						sprintf(cmd,"select name,author,atime,dtimes,doc from document where atime like '%s%%' and mode=0 and stat=1",usr.data);
						sprintf(cmd2,"insert into dochistory(name,type,docname,time) values('%s','timesearch','%s','%s')",usr.name,usr.data,doctime);
				}
				if(sqlite3_exec(db,cmd2,NULL,NULL,&errmsg)!=SQLITE_OK)
				{
						printf("添加查找记录失败\n");
						printf("%s\n",errmsg);
				}
				if(sqlite3_get_table(db,cmd,&result,&nrow,&ncolumn,&errmsg)!=SQLITE_OK)
				{
						printf("查找文档 操作失败\n");
						printf("%s\n",errmsg);
						ser.sig=SER_ERR;
						write(sockfd,&ser,sizeof(ser));
						FD_SET(sockfd,&commonset);
						kill(getpid(),SIGUSR2);
						pthread_exit(NULL);
				}
				if(nrow==0)
				{
						ser.sig=SDO_ERR;
						write(sockfd,&ser,sizeof(ser));
						FD_SET(sockfd,&commonset);
						kill(getpid(),SIGUSR2);
						pthread_exit(NULL);
				}
				else
				{
						ser.sig=SDO_OK;
						index=ncolumn;
						for(i=0;i<nrow;i++)
						{
								for(j=0;j<ncolumn;j++)
								{
										sprintf(ser.data,"%s: %s",result[j],result[index++]);
										write(sockfd,&ser,sizeof(ser));
								}
								strcpy(ser.data,"----------------------");
								write(sockfd,&ser,sizeof(ser));
						}
						ser.sig=SDO_END;
						write(sockfd,&ser,sizeof(ser));
				}
				FD_SET(sockfd,&commonset);
				kill(getpid(),SIGUSR2);
				pthread_exit(NULL);
		}
}

void * Historydoc(void * arg)
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
		int i,j;
		int index;

		sockfd=*(int *)arg;
		free(arg);
		ser.sig=GET_HDO;
		write(sockfd,&ser,sizeof(ser));
		if(read(sockfd,&usr,sizeof(usr))<=0)
		{
				printf("客户端主动断开连接\n");
				close(sockfd);
				pthread_exit(NULL);
		}
		sprintf(cmd,"select * from dochistory where name='%s'",usr.name);
		if(sqlite3_get_table(db,cmd,&result,&nrow,&ncolumn,&errmsg)!=SQLITE_OK)
		{
				printf("查找文档历史表 操作失败\n");
				printf("%s\n",errmsg);
				ser.sig=SER_ERR;
				write(sockfd,&ser,sizeof(ser));
				FD_SET(sockfd,&commonset);
				kill(getpid(),SIGUSR2);
				pthread_exit(NULL);
		}
		if(nrow==0)
		{
				ser.sig=HDO_ERR;
				write(sockfd,&ser,sizeof(ser));
				FD_SET(sockfd,&commonset);
				kill(getpid(),SIGUSR2);
				pthread_exit(NULL);
		}
		ser.sig=HDO_OK;
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
		ser.sig=HDO_END;
		write(sockfd,&ser,sizeof(ser));
		FD_SET(sockfd,&commonset);
		kill(getpid(),SIGUSR2);
		pthread_exit(NULL);
}

void * Downloaddoc_task(void * arg)
{
		pthread_detach(pthread_self());
		int sockfd;
		char cmd[MAX_CMD];
		SER ser;
		USR usr;
		int i,j;
		char ** result;
		int nrow;
		int ncolumn;
		char * errmsg;
		struct tm * t;
		time_t ti;
		char downloadtime[MAX_TIME];
		FILE * fp;
		int ret=0;
		char path[MAX_PATH]={0};
		int flag=0;
		int index;
	
		sockfd=*(int *)arg;
		free(arg);
		ser.sig=GET_DDO_TASK;
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
				printf("确认用户下载特权 操作失败\n");
				printf("%s\n",errmsg);
		}
		if(atoi(result[1])==0)
				flag=1;
		if(usr.cmd==DDO_PRI)
				sprintf(cmd,"select name from document where name='%s' and mode=1 and author='%s'",usr.data,usr.name);
		else if(usr.cmd==DDO_DDO)
				sprintf(cmd,"select name from document where name='%s' and mode=0",usr.data);
		if(sqlite3_get_table(db,cmd,&result,&nrow,&ncolumn,&errmsg)!=SQLITE_OK)
		{
				printf("查找下载文档 操作失败\n");
				printf("%s\n",errmsg);
				ser.sig=SER_ERR;
				write(sockfd,&ser,sizeof(ser));
				FD_SET(sockfd,&commonset);
				kill(getpid(),SIGUSR2);
				pthread_exit(NULL);
		}
		if(nrow==0)
		{
				ser.sig=DDO_ERR;
				write(sockfd,&ser,sizeof(ser));
				FD_SET(sockfd,&commonset);
				kill(getpid(),SIGUSR2);
				pthread_exit(NULL);
		}
		if(usr.cmd==DDO_PRI)
				sprintf(path,"%s%s-%s",doc_path,usr.data,usr.name);
		else if(usr.cmd==DDO_DDO)
				sprintf(path,"%s%s",doc_path,usr.data);
		fp=fopen(path,"r");
		if(fp==NULL)
		{
				ser.sig=DDO_ERR;
				write(sockfd,&ser,sizeof(ser));
				FD_SET(sockfd,&commonset);
				kill(getpid(),SIGUSR2);
				pthread_exit(NULL);
		}
		ser.sig=DDO_OK;
		write(sockfd,&ser,sizeof(ser));
		if(read(sockfd,&usr,sizeof(usr))<=0)
		{
				printf("客户端主动断开连接\n");
				close(sockfd);
				pthread_exit(NULL);
		}
		if(usr.cmd==DDO_CEL)
		{
				write(sockfd,&ser,sizeof(ser));
				FD_SET(sockfd,&commonset);
				kill(getpid(),SIGUSR2);
				pthread_exit(NULL);
		}
		if(read(sockfd,&usr,sizeof(usr))<=0)
		{
				printf("客户端主动断开连接\n");
				close(sockfd);
				pthread_exit(NULL);
		}
		fseek(fp,usr.cmd,SEEK_SET);
		while((ret=fread(ser.data,1,MAX_DATA,fp))==MAX_DATA)
		{
				if(flag==1)
						usleep(5000);
				write(sockfd,&ser,sizeof(ser));
				if(read(sockfd,&usr,sizeof(usr))<=0)
				{
						printf("客户端主动断开连接\n");
						close(sockfd);
						pthread_exit(NULL);
				}
				if(usr.cmd==DDO_CEL)
				{
						FD_SET(sockfd,&commonset);
						kill(getpid(),SIGUSR2);
						pthread_exit(NULL);
				}
				memset(ser.data,0,MAX_DATA);
		}
		ser.sig=DDO_END;
		write(sockfd,&ser,sizeof(ser));
		ser.sig=ret;
		write(sockfd,&ser,sizeof(ser));
		fclose(fp);
		FD_SET(sockfd,&commonset);
		kill(getpid(),SIGUSR2);
		if(usr.cmd==DDO_DDO)
		{
				ti=time(NULL);
				t=localtime(&ti);
				sprintf(downloadtime,"%d-%02d-%02d %02d:%02d:%02d",t->tm_year+1900,t->tm_mon+1,t->tm_mday,t->tm_hour,t->tm_min,t->tm_sec);
				sprintf(cmd,"insert into dochistory values('%s','download','%s','%s')",usr.name,usr.data,downloadtime);
				if(sqlite3_exec(db,cmd,NULL,NULL,&errmsg)!=SQLITE_OK)
				{
						printf("添加下载记录 操作失败");
						printf("%s\n",errmsg);
				}
		}
		pthread_exit(NULL);
}

void * Downloaddoc(void * arg)
{
		pthread_detach(pthread_self());
		int sockfd;
		char cmd[MAX_CMD];
		SER ser;
		USR usr;
		int i,j;
		char ** result;
		int nrow;
		int ncolumn;
		char * errmsg;
		struct tm * t;
		time_t ti;
		char downloadtime[MAX_TIME];
		FILE * fp;
		int ret=0;
		char path[MAX_PATH]={0};
		strcpy(path,doc_path);
		int flag=0;
		int index;

		sockfd=*(int *)arg;
		free(arg);
		ser.sig=GET_DDO;
		write(sockfd,&ser,sizeof(ser));
		while(1)
		{
				if(read(sockfd,&usr,sizeof(usr))<=0)
				{
						printf("客户端主动断开连接\n");
						close(sockfd);
						pthread_exit(NULL);
				}
				if(usr.cmd==DDO_CEL)
				{
						FD_SET(sockfd,&commonset);
						kill(getpid(),SIGUSR2);
						pthread_exit(NULL);
				}
				if(usr.cmd==DDO_PRI||usr.cmd==DDO_DDO)
				{
						sprintf(cmd,"select docmode from client where name='%s'",usr.name);
						if(sqlite3_get_table(db,cmd,&result,&nrow,&ncolumn,&errmsg)!=SQLITE_OK)
						{
								printf("确认用户下载特权 操作失败\n");
								printf("%s\n",errmsg);
						}
						if(atoi(result[1])==0)
								flag=1;
						if(usr.cmd==DDO_PRI)
						{
								sprintf(cmd,"select name,size from document where name='%s' and mode=1 and author='%s'",usr.data,usr.name);
						}
						else if(usr.cmd==DDO_DDO)
						{
								sprintf(cmd,"select name,size from document where name='%s' and mode=0",usr.data);
						}
						if(sqlite3_get_table(db,cmd,&result,&nrow,&ncolumn,&errmsg)!=SQLITE_OK)
						{
								printf("查找下载文档 操作失败\n");
								printf("%s\n",errmsg);
								ser.sig=SER_ERR;
								write(sockfd,&ser,sizeof(ser));
								FD_SET(sockfd,&commonset);
								kill(getpid(),SIGUSR2);
								pthread_exit(NULL);
						}
						if(nrow==0)
						{
								ser.sig=DDO_ERR;
								write(sockfd,&ser,sizeof(ser));
								continue;
						}
						strcat(path,result[2]);
						fp=fopen(path,"r");
						if(fp==NULL)
						{
								ser.sig=SER_ERR;
								write(sockfd,&ser,sizeof(ser));
								FD_SET(sockfd,&commonset);
								kill(getpid(),SIGUSR2);
								pthread_exit(NULL);
						}
						ser.sig=DDO_DOC;
						strcpy(ser.data,result[3]);
						write(sockfd,&ser,sizeof(ser));
						if(read(sockfd,&usr,sizeof(usr))<=0)
						{
								printf("客户端主动断开连接\n");
								close(sockfd);
								pthread_exit(NULL);
						}
						if(usr.cmd==DDO_CEL)
								continue;
						while((ret=fread(ser.data,1,MAX_DATA,fp))==MAX_DATA)
						{
								if(flag==1)
										usleep(5000);
								write(sockfd,&ser,sizeof(ser));
								memset(ser.data,0,MAX_DATA);
						}
						ser.sig=DDO_END;
						write(sockfd,&ser,sizeof(ser));
						ser.sig=ret;
						write(sockfd,&ser,sizeof(ser));
						fclose(fp);
						if(usr.cmd==DDO_DDO)
						{
								ti=time(NULL);
								t=localtime(&ti);
								sprintf(downloadtime,"%d-%02d-%02d %02d:%02d:%02d",t->tm_year+1900,t->tm_mon+1,t->tm_mday,t->tm_hour,t->tm_min,t->tm_sec);
								sprintf(cmd,"insert into dochistory values('%s','download','%s','%s')",usr.name,usr.data,downloadtime);
								if(sqlite3_exec(db,cmd,NULL,NULL,&errmsg)!=SQLITE_OK)
								{
										printf("添加下载记录 操作失败");
										printf("%s\n",errmsg);
								}
						}
				}
				else if(usr.cmd==DDO_HIS)
				{
						sprintf(cmd,"select * from dochistory where name='%s' and type='download'",usr.name);
						if(sqlite3_get_table(db,cmd,&result,&nrow,&ncolumn,&errmsg)!=SQLITE_OK)
						{
								printf("查找下载历史记录 操作失败\n");
								printf("%s\n",errmsg);
								ser.sig=SER_ERR;
								write(sockfd,&ser,sizeof(ser));
								FD_SET(sockfd,&commonset);
								kill(getpid(),SIGUSR2);
								pthread_exit(NULL);
						}
						if(nrow==0)
						{
								ser.sig=DDO_ERR;
								write(sockfd,&ser,sizeof(ser));
								FD_SET(sockfd,&commonset);
								kill(getpid(),SIGUSR2);
								pthread_exit(NULL);
						}
						ser.sig=DDO_OK;
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
						ser.sig=DDO_END;
						write(sockfd,&ser,sizeof(ser));
				}
		}
}

void * Uploaddoc_task(void * arg)
{
		pthread_detach(pthread_self());
		int sockfd;
		char cmd[MAX_CMD];
		SER ser;
		USR usr;
		int i,j;
		char ** result;
		int nrow;
		int ncolumn;
		char * errmsg;
		struct tm * t;
		time_t ti;
		char uploadtime[MAX_TIME];
		FILE * fp;
		int ret=0;
		char filename[MAX_FILENAME];
		char path[MAX_PATH]={0};
		strcpy(path,doc_path);
		int flag=0;
		int index;
		int temp_usrcmd;
		
		sockfd=*(int *)arg;
		free(arg);
		ser.sig=GET_UDO_TASK;
		write(sockfd,&ser,sizeof(ser));
		if(read(sockfd,&usr,sizeof(usr))<=0)
		{
				printf("客户端主动断开连接\n");
				close(sockfd);
				pthread_exit(NULL);
		}
		temp_usrcmd=usr.cmd;
		strcpy(filename,usr.data);
		strcat(path,usr.data);
		if(usr.cmd==UDO_PRI)
				sprintf(path,"%s%s-%s",doc_path,usr.data,usr.name);
		fp=fopen(path,"a+");
		if(fp==NULL)
		{
				ser.sig=SER_ERR;
				write(sockfd,&ser,sizeof(ser));
				FD_SET(sockfd,&commonset);
				kill(getpid(),SIGUSR2);
				pthread_exit(NULL);
		}
		ser.sig=UDO_OK;
		write(sockfd,&ser,sizeof(ser));
		if(read(sockfd,&usr,sizeof(usr))<=0)
		{
				printf("客户端主动断开连接\n");
				close(sockfd);
				pthread_exit(NULL);
		}
		if(usr.cmd==UDO_CEL) //用户取消了下载,或者失败重试次数超过限制
		{
				FD_SET(sockfd,&commonset);
				kill(getpid(),SIGUSR2);
				fclose(fp);
				remove(path);  //删除该文件,并且在文件表中删去
				if(temp_usrcmd==UDO_UDO)
						sprintf(cmd,"delete from document where name='%s'",filename);
				else if(temp_usrcmd==UDO_PRI)
						sprintf(cmd,"delete from document where name='%s' and author='%s' and mode=1",filename,usr.name);
				if(sqlite3_exec(db,cmd,NULL,NULL,&errmsg)!=SQLITE_OK)
				{
						printf("删除文件 操作失败");
						printf("%s\n",errmsg);
				}
				pthread_exit(NULL);
		}
		while(1)
		{
				if(read(sockfd,&usr,sizeof(usr))<=0)
				{
						printf("客户端主动断开连接\n");
						close(sockfd);
						fclose(fp);
						pthread_exit(NULL);
				}
				if(usr.cmd==UDO_CEL)
				{
						fclose(fp);
						FD_SET(sockfd,&commonset);
						kill(getpid(),SIGUSR2);
						pthread_exit(NULL);
				}
				if(flag==1)
						usleep(5000);
				if(usr.cmd==UDO_DOC)
						fwrite(usr.data,1,MAX_DATA,fp);
				else if(usr.cmd==UDO_END)
						break;
		}
		if(read(sockfd,&usr,sizeof(usr))<=0)
		{
				printf("客户端主动断开连接\n");
				close(sockfd);
				fclose(fp);
				pthread_exit(NULL);
		}
		fwrite(usr.data,1,usr.cmd,fp);
		if(read(sockfd,&usr,sizeof(usr))<=0)
		{
				printf("客户端主动断开连接\n");
				close(sockfd);
				fclose(fp);
				pthread_exit(NULL);
		}
		char size[6]={0};
		sprintf(size,"%d%c",usr.cmd,usr.data[0]);
		fclose(fp);
		ti=time(NULL);
		t=localtime(&ti);
		sprintf(uploadtime,"%d-%02d-%02d %02d:%02d:%02d",t->tm_year+1900,t->tm_mon+1,t->tm_mday,t->tm_hour,t->tm_min,t->tm_sec);
		if(temp_usrcmd==UDO_UDO)
		{
				sprintf(cmd,"insert into dochistory values('%s','upload','%s','%s')",usr.name,filename,uploadtime);
				if(sqlite3_exec(db,cmd,NULL,NULL,&errmsg)!=SQLITE_OK)
				{
						printf("添加上传记录 操作失败");
						printf("%s\n",errmsg);
				}
				sprintf(cmd,"update document set stat=1,atime='%s' where name='%s'",uploadtime,filename);
				if(sqlite3_exec(db,cmd,NULL,NULL,&errmsg)!=SQLITE_OK)
				{
						printf("上传文档 操作失败");
						printf("%s\n",errmsg);
						ser.sig=UDO_ERR;
				}
				else
						ser.sig=UDO_OK;
				write(sockfd,&ser,sizeof(ser));
		}
		else if(temp_usrcmd==UDO_PRI)
		{
				sprintf(cmd,"update document set stat=1,atime='%s' where name='%s' and author='%s' and mode=1",uploadtime,filename,usr.name);
				if(sqlite3_exec(db,cmd,NULL,NULL,&errmsg)!=SQLITE_OK)
				{
						printf("添加上传文档 操作失败");
						printf("%s\n",errmsg);
						ser.sig=UDO_ERR;
				}
				else
						ser.sig=UDO_OK;
				write(sockfd,&ser,sizeof(ser));
		}
}

void * Uploaddoc(void * arg)
{
		pthread_detach(pthread_self());
		int sockfd;
		char cmd[MAX_CMD];
		SER ser;
		USR usr;
		int i,j;
		char ** result;
		int nrow;
		int ncolumn;
		char * errmsg;
		struct tm * t;
		time_t ti;
		char uploadtime[MAX_TIME];
		FILE * fp;
		int ret=0;
		char filename[MAX_FILENAME];
		char path[MAX_PATH]={0};
		strcpy(path,doc_path);
		int flag=0;
		int index;
		int temp_usrcmd;

		sockfd=*(int *)arg;
		free(arg);
		ser.sig=GET_UDO;
		write(sockfd,&ser,sizeof(ser));
		while(1)
		{
				if(read(sockfd,&usr,sizeof(usr))<=0)
				{
						printf("客户端主动断开连接\n");
						close(sockfd);
						pthread_exit(NULL);
				}
				if(usr.cmd==UDO_CEL)
				{
						FD_SET(sockfd,&commonset);
						kill(getpid(),SIGUSR2);
						pthread_exit(NULL);
				}
				if(usr.cmd==UDO_AGA)
						continue;
				if(usr.cmd==UDO_PRI||usr.cmd==UDO_UDO)
				{
						temp_usrcmd=usr.cmd;
						sprintf(cmd,"select docmode from client where name='%s'",usr.name);
						if(sqlite3_get_table(db,cmd,&result,&nrow,&ncolumn,&errmsg)!=SQLITE_OK)
						{
								printf("确认用户上传特权 操作失败\n");
								printf("%s\n",errmsg);
								flag=1;
						}
						else if(atoi(result[1])==0)
								flag=1;
						if(usr.cmd==UDO_PRI)
								sprintf(cmd,"select name from document where name='%s' and mode=1 and author='%s'",usr.data,usr.name);
						else if(usr.cmd==UDO_UDO)
								sprintf(cmd,"select name from document where name='%s' and mode=0",usr.data);
						if(sqlite3_get_table(db,cmd,&result,&nrow,&ncolumn,&errmsg)!=SQLITE_OK)
						{
								printf("确认上传文档名有无重复 操作失败\n");
								printf("%s\n",errmsg);
								ser.sig=SER_ERR;
								write(sockfd,&ser,sizeof(ser));
								FD_SET(sockfd,&commonset);
								kill(getpid(),SIGUSR2);
								pthread_exit(NULL);
						}
						if(nrow==0)
						{
								strcpy(filename,usr.data);
								strcat(path,usr.data);
								if(usr.cmd==UDO_PRI)
										sprintf(path,"%s%s-%s",doc_path,usr.data,usr.name);
								fp=fopen(path,"w");
								if(fp==NULL)
								{
										ser.sig=SER_ERR;
										write(sockfd,&ser,sizeof(ser));
										FD_SET(sockfd,&commonset);
										kill(getpid(),SIGUSR2);
										pthread_exit(NULL);
								}
								ser.sig=UDO_OK;
								write(sockfd,&ser,sizeof(ser));
								if(read(sockfd,&usr,sizeof(usr))<=0)
								{
										printf("客户端主动断开连接\n");
										close(sockfd);
										fclose(fp);
										pthread_exit(NULL);
								}
								if(usr.cmd==UDO_TASK) //如果用户选择了task上传模式，则先将该文件添加进文件表，并将标志位设置为待完成
								{
										fclose(fp);
										FD_SET(sockfd,&commonset);
										kill(getpid(),SIGUSR2);
										if(temp_usrcmd==UDO_UDO)
												sprintf(cmd,"insert into document(name,author,size,mode,stat) values('%s','%s','%s',0,0)",filename,usr.name,usr.data);
										else if(temp_usrcmd==UDO_PRI)
												sprintf(cmd,"insert into document(name,author,size,mode,stat) values('%s','%s','%s',1,0)",filename,usr.name,usr.data);
										if(sqlite3_exec(db,cmd,NULL,NULL,&errmsg)!=SQLITE_OK)
										{
												printf("添加上传文档 操作失败");
												printf("%s\n",errmsg);
												ser.sig=UDO_ERR;
										}
										else 
												ser.sig=UDO_OK;
										write(sockfd,&ser,sizeof(ser));
										FD_SET(sockfd,&commonset);
										kill(getpid(),SIGUSR2);
										pthread_exit(NULL);
								}
								if(usr.cmd==UDO_CEL)
								{
										fclose(fp);
										FD_SET(sockfd,&commonset);
										kill(getpid(),SIGUSR2);
										pthread_exit(NULL);
								}
								while(1)
								{
										if(read(sockfd,&usr,sizeof(usr))<=0)
										{
												printf("客户端主动断开连接\n");
												close(sockfd);
												fclose(fp);
												pthread_exit(NULL);
										}
										printf("%d\n",usr.cmd);
										if(flag==1)
												usleep(5000);
										if(usr.cmd==UDO_DOC)
												fwrite(usr.data,1,MAX_DATA,fp);
										else if(usr.cmd==UDO_END)
												break;
								}
										printf("test\n");
								if(read(sockfd,&usr,sizeof(usr))<=0)
								{
										printf("客户端主动断开连接\n");
										close(sockfd);
										fclose(fp);
										pthread_exit(NULL);
								}
								fwrite(usr.data,1,usr.cmd,fp);
								if(read(sockfd,&usr,sizeof(usr))<=0)
								{
										printf("客户端主动断开连接\n");
										close(sockfd);
										fclose(fp);
										pthread_exit(NULL);
								}
								char size[6]={0};
								sprintf(size,"%d%c",usr.cmd,usr.data[0]);
								fclose(fp);
								ti=time(NULL);
								t=localtime(&ti);
								sprintf(uploadtime,"%d-%02d-%02d %02d:%02d:%02d",t->tm_year+1900,t->tm_mon+1,t->tm_mday,t->tm_hour,t->tm_min,t->tm_sec);
								if(temp_usrcmd==UDO_UDO)
								{
										sprintf(cmd,"insert into dochistory values('%s','upload','%s','%s')",usr.name,filename,uploadtime);
										if(sqlite3_exec(db,cmd,NULL,NULL,&errmsg)!=SQLITE_OK)
										{
												printf("添加上传记录 操作失败");
												printf("%s\n",errmsg);
										}
										sprintf(cmd,"insert into document values('%s','%s','%s','%s',0,0,1)",filename,usr.name,uploadtime,size);
										if(sqlite3_exec(db,cmd,NULL,NULL,&errmsg)!=SQLITE_OK)
										{
												printf("添加上传文档 操作失败");
												printf("%s\n",errmsg);
												ser.sig=UDO_ERR;
										}
										else
												ser.sig=UDO_OK;
										write(sockfd,&ser,sizeof(ser));
								}
								else if(temp_usrcmd==UDO_PRI)
								{
										sprintf(cmd,"insert into document values('%s','%s','%s','%s',0,1,1)",filename,usr.name,uploadtime,size);
										if(sqlite3_exec(db,cmd,NULL,NULL,&errmsg)!=SQLITE_OK)
										{
												printf("添加上传文档 操作失败");
												printf("%s\n",errmsg);
												ser.sig=UDO_ERR;
										}
										else
												ser.sig=UDO_OK;
										write(sockfd,&ser,sizeof(ser));
								}
						}
						else
						{
								ser.sig=UDO_ERR;
								write(sockfd,&ser,sizeof(ser));
						}
				}
				else if(usr.cmd==UDO_HIS)
				{
						sprintf(cmd,"select * from dochistory where name='%s' and type='upload'",usr.name);
						if(sqlite3_get_table(db,cmd,&result,&nrow,&ncolumn,&errmsg)!=SQLITE_OK)
						{
								printf("查找上传历史记录 操作失败\n");
								printf("%s\n",errmsg);
								ser.sig=SER_ERR;
								write(sockfd,&ser,sizeof(ser));
								FD_SET(sockfd,&commonset);
								kill(getpid(),SIGUSR2);
								pthread_exit(NULL);
						}
						if(nrow==0)
						{
								ser.sig=UDO_ERR;
								write(sockfd,&ser,sizeof(ser));
								FD_SET(sockfd,&commonset);
								kill(getpid(),SIGUSR2);
								pthread_exit(NULL);
						}
						ser.sig=UDO_OK;
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
						ser.sig=UDO_END;
						write(sockfd,&ser,sizeof(ser));
				}
		}
}

void * Canceldoc(void * arg)
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
		char path[MAX_PATH]={0};
		strcpy(path,doc_path);
		int index;
		int i,j;

		sockfd=*(int *)arg;
		free(arg);
		ser.sig=GET_CDO;
		write(sockfd,&ser,sizeof(ser));
		if(read(sockfd,&usr,sizeof(usr))<=0)
		{
				printf("客户端主动断开连接\n");
				close(sockfd);
				pthread_exit(NULL);
		}
		sprintf(cmd,"select * from document where author='%s'",usr.name);
		if(sqlite3_get_table(db,cmd,&result,&nrow,&ncolumn,&errmsg)!=SQLITE_OK)
		{
				printf("获取文档一览 操作失败\n");
				printf("%s\n",errmsg);
				ser.sig=SER_ERR;
				write(sockfd,&ser,sizeof(ser));
				FD_SET(sockfd,&commonset);
				kill(getpid(),SIGUSR2);
				pthread_exit(NULL);
		}
		if(nrow==0)
		{
				ser.sig=CDO_ERR;
				write(sockfd,&ser,sizeof(ser));
				FD_SET(sockfd,&commonset);
				kill(getpid(),SIGUSR2);
				pthread_exit(NULL);
		}
		else
		{
				ser.sig=CDO_LIST;
				index=ncolumn;
				for(i=0;i<nrow;i++)
				{
						for(j=0;j<ncolumn;j++)
						{
								sprintf(ser.data,"%s: %s",result[j],result[index++]);
								write(sockfd,&ser,sizeof(ser));
						}
						strcpy(ser.data,"----------------------");
						write(sockfd,&ser,sizeof(ser));
				}
				ser.sig=CDO_END;
				write(sockfd,&ser,sizeof(ser));
		}
		while(1)
		{
				if(read(sockfd,&usr,sizeof(usr))<=0)
				{
						printf("客户端主动断开连接\n");
						close(sockfd);
						pthread_exit(NULL);
				}
				if(usr.cmd==CDO_CEL)
				{
						FD_SET(sockfd,&commonset);
						kill(getpid(),SIGUSR2);
						pthread_exit(NULL);
				}
				sprintf(cmd,"select * from document where name='%s' and author='%s'",usr.data,usr.name);
				if(sqlite3_get_table(db,cmd,&result,&nrow,&ncolumn,&errmsg)!=SQLITE_OK)
				{
						printf("查找需要删除的文档 操作失败\n");
						printf("%s\n",errmsg);
						ser.sig=SER_ERR;
						write(sockfd,&ser,sizeof(ser));
						FD_SET(sockfd,&commonset);
						kill(getpid(),SIGUSR2);
						pthread_exit(NULL);
				}
				if(nrow==0)
				{
						ser.sig=CDO_ERR;
						write(sockfd,&ser,sizeof(ser));
						continue;
				}
				sprintf(cmd,"delete * from document where name='%s' and author='%s'",usr.data,usr.name);
				if(sqlite3_exec(db,cmd,NULL,NULL,&errmsg)!=SQLITE_OK)
				{
						printf("修改用户聊天标记 操作失败\n");
						printf("%s\n",errmsg);
						ser.sig=SER_ERR;
						write(sockfd,&ser,sizeof(ser));
						FD_SET(sockfd,&commonset);
						kill(getpid(),SIGUSR2);
						pthread_exit(NULL);
				}
				strcat(path,usr.data);
				remove(path);
				ser.sig=CDO_OK;
				write(sockfd,&ser,sizeof(ser));
		}
}
