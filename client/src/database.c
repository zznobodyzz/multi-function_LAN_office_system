#include"client_head.h"
extern sqlite3 * db;

void Init_data(void)
{
		char cmd[MAX_CMD];
		char * errmsg;
		char **result;
		int nrow;
		int ncolumn;
		int i;
		int flag=0;
		int flag_chathistory=0;
		int flag_doclist=0;
		int flag_chathistory_bake=0;
		int flag_doclist_bake=0;
		if(sqlite3_open("client.db",&db)!=SQLITE_OK)
		{
				printf("数据库打开失败\n");
				printf("%s\n",sqlite3_errmsg(db));
				exit(-1);
		}
		printf("正在检查数据是否存在...\n");
		strcpy(cmd,"select name from sqlite_master where type='table'");
		if(sqlite3_get_table(db,cmd,&result,&nrow,&ncolumn,&errmsg)!=SQLITE_OK)
		{
				printf("数据库遇到致命错误\n");
				printf("%s\n",errmsg);
				exit(-1);
		}
		for(i=1;i<=nrow;i++)
		{
				if(strcmp(result[i],"chathistory")==0)
				{
						printf("聊天记录已存在\n");
						flag_chathistory=1;
				}
				if(strcmp(result[i],"dochistory")==0)
				{
						printf("下载文档表已存在\n");
						flag_doclist=1;
				}
				if(strcmp(result[i],"chathistory_bake")==0)
				{
						printf("聊天记录备份已存在\n");
						flag_chathistory_bake=1;
				}
				if(strcmp(result[i],"dochistory_bake")==0)
				{
						printf("下载文档表备份已存在\n");
						flag_doclist_bake=1;
				}
		}
		if(flag_chathistory==0)
		{
				if(flag_chathistory_bake==1)
				{
						printf("未找到聊天记录表,正在导入备份...\n");
						strcpy(cmd,"create table chathistory as select * from chathistory_bake");
						if(sqlite3_exec(db,cmd,NULL,NULL,&errmsg)!=SQLITE_OK)
						{
								printf("备份导入失败,正在重新生成聊天记录表...\n");
								printf("%s\n",errmsg);
						}
						else
						{
								printf("备份导入成功\n");
								flag=1;
						}
				}
				else if(flag_chathistory_bake==0)
				{
						printf("未找到用户表和备份,正在生成用户表\n");
				}
				if(flag==0)
				{
						strcpy(cmd,"create table chathistory(srcname text not null,dstname text not null,string text,chattime text)");
						if(sqlite3_exec(db,cmd,NULL,NULL,&errmsg)!=SQLITE_OK)
						{
								printf("数据库遇到致命错误\n");
								printf("%s\n",errmsg);
								exit(-1);
						}
				}
				printf("聊天记录表初始化完毕\n");
				flag=0;
		}
		if(flag_doclist==0)
		{
				if(flag_doclist_bake==1)
				{
						printf("未找到下载文档表,正在导入备份...\n");
						strcpy(cmd,"create table dochistory as select * from dochistory_bake");
						if(sqlite3_exec(db,cmd,NULL,NULL,&errmsg)!=SQLITE_OK)
						{
								printf("备份导入失败,正在重新生成下载文档表...\n");
								printf("%s\n",errmsg);
						}
						else
						{
								printf("备份导入成功\n");
								flag=1;
						}
				}
				else if(flag_doclist_bake==0)
				{
						printf("未找到下载文档表和备份,正在生成下载文档表\n");
				}
				if(flag==0)
				{
						strcpy(cmd,"create table dochistory(name text not null,mode integer)");
						if(sqlite3_exec(db,cmd,NULL,NULL,&errmsg)!=SQLITE_OK)
						{
								printf("数据库遇到致命错误\n");
								printf("%s\n",errmsg);
								exit(-1);
						}
				}
				printf("下载文档表表初始化完毕\n");
		}
		printf("数据库初始化完毕\n");	
}

void Bake_data(void)
{
		printf("正在准备备份数据\n");
		printf("正在检测备份更新\n");
		char date[20];
		char baketime[20];
		time_t ti;
		int i;
		int flag=0;
		FILE * fp;
		char cmd[MAX_CMD];
		char * errmsg;
		struct tm * tt;
		fp=fopen("./date.conf","r");
		if(fp==NULL)
		{
				perror("打开日期文件出错\n");
				flag=1;
		}
		if(fgets(date,20,fp))
		{
				ti=time(NULL);
				tt=localtime(&ti);
				char * p;
				p=date;
				for(i=0;i<4;i++)
						baketime[i]=*(p++);
				baketime[i]='\0';
				if(atoi(baketime)<tt->tm_year+1900)
						flag=1;
				else
				{
						p++;
						for(i=0;i<2;i++)
								baketime[i]=*(p++);
						baketime[i]='\0';
						if(atoi(baketime)<tt->tm_mon+1)
								flag=1;
						else
						{
								p++;
								for(i=0;i<2;i++)
										baketime[i]=*(p++);
								baketime[i]='\0';
								if(atoi(baketime)<tt->tm_mday)
										flag=1;
						}
				}
		}
		else
				flag=1;
		fclose(fp);
		if(flag==1)
		{
				printf("正在备份数据库...\n");
				strcpy(cmd,"drop table if exists chathistory_bake");
				if(sqlite3_exec(db,cmd,NULL,NULL,&errmsg)!=SQLITE_OK)
				{
						printf("数据库遇到致命错误(drop bake chathistory)\n");
						printf("%s\n",errmsg);
						exit(-1);
				}
				strcpy(cmd,"create table chathistory_bake as select * from chathistory");
				if(sqlite3_exec(db,cmd,NULL,NULL,&errmsg)!=SQLITE_OK)
				{
						printf("数据库遇到致命错误(bake chathistory)\n");
						printf("%s\n",errmsg);
						exit(-1);
				}
				strcpy(cmd,"drop table if exists dochistory_bake");
				if(sqlite3_exec(db,cmd,NULL,NULL,&errmsg)!=SQLITE_OK)
				{
						printf("数据库遇到致命错误(drop bake dochistory)\n");
						printf("%s\n",errmsg);
						exit(-1);
				}
				strcpy(cmd,"create table dochistory_bake as select * from dochistory");
				if(sqlite3_exec(db,cmd,NULL,NULL,&errmsg)!=SQLITE_OK)
				{
						printf("数据库遇到致命错误(bake dochistory)\n");
						printf("%s\n",errmsg);
						exit(-1);
				}
				printf("数据库备份完毕\n");
				sprintf(baketime,"%d-%02d-%02d\n",tt->tm_year+1900,tt->tm_mon+1,tt->tm_mday);
				fp=fopen("./date.conf","w");
				if(fp==NULL)
				{		
						perror("写入备份日期出错\n");
						exit(-1);
				}
				fputs(baketime,fp);
				fclose(fp);
		}
		else
		{
				printf("备份已至最新，无需再次备份\n");
		}
}
