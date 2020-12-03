#include"server_head.h"

extern sqlite3 * db;
extern char data_base[MAX_FILENAME];
extern print_log_flag;

void Init_db(void)
{
		char cmd[MAX_CMD];
		char word[MAX_WORD];
		char explain[MAX_EXPL];
		char buf[MAX_DATA];
		char * errmsg;
		char **result;
		int nrow;
		int ncolumn;
		FILE * fp;
		int i;
		int flag=0;
		char flag_client=0;
		char flag_dictionary=0;
		char flag_history=0;
		char flag_dochistory=0;
		char flag_document=0;
		char flag_chat=0;
		char flag_game_rank=0;
		char flag_client_bake=0;
		char flag_dictionary_bake=0;
		char flag_history_bake=0;
		char flag_dochistory_bake=0;
		char flag_document_bake=0;
		char flag_chat_bake=0;
		char flag_game_rank_bake=0;
		int line_flag=0;
		int progress_for_dic=0;
		char * bufp;
		if(sqlite3_open(data_base,&db)!=SQLITE_OK)
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
				if(strcmp(result[i],"client")==0)
				{
						if(print_log_flag==1)
								printf("用户表已存在\n");
						flag_client=1;
				}
				if(strcmp(result[i],"dictionary")==0)
				{
						if(print_log_flag==1)
								printf("字典数据已存在\n");
						flag_dictionary=1;
				}
				if(strcmp(result[i],"history")==0)
				{
						if(print_log_flag==1)
								printf("单词历史记录表已存在\n");
						flag_history=1;
				}
				if(strcmp(result[i],"dochistory")==0)
				{
						if(print_log_flag==1)
								printf("文档历史记录表已存在\n");
						flag_dochistory=1;
				}
				if(strcmp(result[i],"chat")==0)
				{
						if(print_log_flag==1)
								printf("聊天记录表已存在\n");
						flag_chat=1;
				}
				if(strcmp(result[i],"document")==0)
				{
						if(print_log_flag==1)
								printf("文档库已存在\n");
						flag_document=1;
				}
				if(strcmp(result[i],"game_rank")==0)
				{
						if(print_log_flag==1)
								printf("游戏天梯表已存在\n");
						flag_game_rank=1;
				}
				if(strcmp(result[i],"client_bake")==0)
				{
						if(print_log_flag==1)
								printf("用户表备份已存在\n");
						flag_client_bake=1;
				}
				if(strcmp(result[i],"dictionary_bake")==0)
				{
						if(print_log_flag==1)
								printf("字典数据备份已存在\n");
						flag_dictionary_bake=1;
				}
				if(strcmp(result[i],"history_bake")==0)
				{
						if(print_log_flag==1)
								printf("单词历史记录表备份已存在\n");
						flag_history_bake=1;
				}
				if(strcmp(result[i],"dochistory_bake")==0)
				{
						if(print_log_flag==1)
								printf("文档历史记录表备份已存在\n");
						flag_dochistory_bake=1;
				}
				if(strcmp(result[i],"chat_bake")==0)
				{
						if(print_log_flag==1)
								printf("聊天记录表备份已存在\n");
						flag_chat_bake=1;
				}
				if(strcmp(result[i],"document_bake")==0)
				{
						if(print_log_flag==1)
								printf("文档库备份已存在\n");
						flag_document_bake=1;
				}
				if(strcmp(result[i],"game_rank_bake")==0)
				{
						if(print_log_flag==1)
								printf("游戏天梯表备份已存在\n");
						flag_game_rank_bake=1;
				}
		}
		if(flag_client==0)
		{
				if(flag_client_bake==1)
				{
						printf("未找到用户表,正在导入备份...\n");
						strcpy(cmd,"create table client as select * from client_bake");
						if(sqlite3_exec(db,cmd,NULL,NULL,&errmsg)!=SQLITE_OK)
						{
								printf("备份导入失败,正在重新生成用户表...\n");
								printf("%s\n",errmsg);
						}
						else
						{
								printf("备份导入成功\n");
								flag=1;
						}
				}
				else if(flag_client_bake==0)
				{
						printf("未找到用户表和备份,正在生成用户表\n");
				}
				if(flag==0)
				{
						strcpy(cmd,"create table client(no integer primary key,name text not null,addr text,port text,pwd text default '123321',stat integer default 1,mode integer default 1,logintime text,logouttime text,adv integer default 1,sco integer default 0,chat integer default 0,docmode integer default 0,care integer default 0,cared text default null,game integer default 0)");
						if(sqlite3_exec(db,cmd,NULL,NULL,&errmsg)!=SQLITE_OK)
						{
								printf("数据库遇到致命错误\n");
								printf("%s\n",errmsg);
								exit(-1);
						}
				}
				printf("用户表初始化完毕\n");
				flag=0;
		}
		if(flag_dictionary==0)
		{
				if(flag_dictionary_bake==1)
				{
						printf("未找到字典数据,正在导入备份...\n");
						strcpy(cmd,"create table dictionary as select * from dictionary_bake");
						if(sqlite3_exec(db,cmd,NULL,NULL,&errmsg)!=SQLITE_OK)
						{
								printf("备份导入失败,正在重新生成字典...\n");
								printf("%s\n",errmsg);
						}
						else
						{
								printf("备份导入成功\n");
								flag=1;
						}
				}
				else if(flag_dictionary_bake==0)
				{
						printf("未找到字典数据和备份,正在尝试重新导入数据...\n");
				}
				if(flag==0)
				{
						fp=fopen("dict.txt","r");
						if(fp==NULL)
						{
								printf("字典源数据打开失败，服务器即将关闭\n");
								exit(0);
						}
						strcpy(cmd,"create table dictionary(word text not null,explain text)");
						if(sqlite3_exec(db,cmd,NULL,NULL,&errmsg)!=SQLITE_OK)
						{
								printf("数据库遇到致命错误\n");
								printf("%s\n",errmsg);
								exit(-1);
						}
						printf("生成字典表成功,正在导入数据...\n");
						while(fgets(buf,MAX_DATA,fp))
						{
								i=0;
								bufp=buf;
								while(*bufp!=' ')
								{
										word[i++]=*(bufp++);
								}
								word[i++]=' ';
								word[i]='\0';
								i=0;
								while(*bufp==' ')
										bufp++;
								while(*bufp!='\n')
								{
										explain[i++]=*(bufp++);
								}
								explain[i]='\0';
								sprintf(cmd,"insert into dictionary values('%s','%s')",word,explain);
								if(sqlite3_exec(db,cmd,NULL,NULL,&errmsg)!=SQLITE_OK)
								{
										printf("数据库遇到致命错误\n");
										printf("%s\n",errmsg);
										exit(-1);
								}
								line_flag++;
								if(line_flag==196)
								{
										printf("\b\b\b\b\b\b\b\b进度:%%%02d",++progress_for_dic);
										line_flag=0;
								}
						}
						printf("\n");
						fclose(fp);
				}
				printf("字典数据导入完成,字典表初始化完毕\n");
				flag=0;
		}
		if(flag_history==0)
		{
				if(flag_history_bake==1)
				{
						printf("未找到单词历史记录表,正在导入备份...\n");
						strcpy(cmd,"create table history as select * from history_bake");
						if(sqlite3_exec(db,cmd,NULL,NULL,&errmsg)!=SQLITE_OK)
						{
								printf("备份导入失败,正在重新生成单词历史记录表...\n");
								printf("%s\n",errmsg);
						}
						else
						{
								printf("备份导入成功\n");
								flag=1;
						}
				}
				else if(flag_history_bake==0)
				{
						printf("未找到单词历史记录表和备份,正在生成单词历史记录表...\n");
				}
				if(flag==0)
				{
						strcpy(cmd,"create table history(name text not null,word text,dictime text)");
						if(sqlite3_exec(db,cmd,NULL,NULL,&errmsg)!=SQLITE_OK)
						{
								printf("数据库遇到致命错误\n");
								printf("%s\n",errmsg);
								exit(-1);
						}
				}
				printf("单词历史记录表初始化完毕\n");
				flag=0;
		}
		if(flag_dochistory==0)
		{
				if(flag_dochistory_bake==1)
				{
						printf("未找到文档历史记录表,正在导入备份...\n");
						strcpy(cmd,"create table dochistory as select * from dochistory_bake");
						if(sqlite3_exec(db,cmd,NULL,NULL,&errmsg)!=SQLITE_OK)
						{
								printf("备份导入失败,正在重新生成文档历史记录表...\n");
								printf("%s\n",errmsg);
						}
						else
						{
								printf("备份导入成功\n");
								flag=1;
						}
				}
				else if(flag_history_bake==0)
				{
						printf("未找到文档历史记录表和备份,正在生成文档历史记录表...\n");
				}
				if(flag==0)
				{
						strcpy(cmd,"create table dochistory(name text not null,type text,docname text,time text)");
						if(sqlite3_exec(db,cmd,NULL,NULL,&errmsg)!=SQLITE_OK)
						{
								printf("数据库遇到致命错误\n");
								printf("%s\n",errmsg);
								exit(-1);
						}
				}
				printf("文档历史记录表初始化完毕\n");
				flag=0;
		}
		if(flag_chat==0)
		{
				if(flag_chat_bake==1)
				{
						printf("未找到聊天记录表,正在导入备份...\n");
						strcpy(cmd,"create table chat as select * from chat_bake");
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
				else if(flag_chat_bake==0)
				{
						printf("聊天记录表未创建，正在生成聊天记录表...\n");
				}
				if(flag==0)
				{
						strcpy(cmd,"create table chat(srcname text not null,dstname text not null,string text,chattime text)");
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
		if(flag_document==0)
		{
				if(flag_document_bake==1)
				{
						printf("未找到文档列表,正在导入备份...\n");
						strcpy(cmd,"create table document as select * from document_bake");
						if(sqlite3_exec(db,cmd,NULL,NULL,&errmsg)!=SQLITE_OK)
						{
								printf("备份导入失败,正在重新生成文档列表...\n");
								printf("%s\n",errmsg);
						}
						else
						{
								printf("备份导入成功\n");
								flag=1;
						}
				}
				else if(flag_document_bake==0)
				{
						printf("文档列表未创建，正在生成文档库...\n");
				}
				if(flag==0)
				{
						strcpy(cmd,"create table document(name text not null,author text,atime text,size text,dtimes integer default 0,mode integer default 0,stat integer)");
						if(sqlite3_exec(db,cmd,NULL,NULL,&errmsg)!=SQLITE_OK)
						{
								printf("数据库遇到致命错误\n");
								printf("%s\n",errmsg);
								exit(-1);
						}
				}
				printf("文档列表初始化完毕\n");
				flag=0;
		}
		if(flag_game_rank==0)
		{
				if(flag_game_rank_bake==1)
				{
						printf("未找到游戏天梯表,正在导入备份...\n");
						strcpy(cmd,"create table game_rank as select * from game_rank_bake");
						if(sqlite3_exec(db,cmd,NULL,NULL,&errmsg)!=SQLITE_OK)
						{
								printf("备份导入失败,正在重新生成游戏天梯表表...\n");
								printf("%s\n",errmsg);
						}
						else
						{
								printf("备份导入成功\n");
								flag=1;
						}
				}
				else if(flag_history_bake==0)
				{
						printf("未找到游戏天梯表和备份,正在生成游戏天梯表...\n");
				}
				if(flag==0)
				{
						strcpy(cmd,"create table game_rank(name text not null,gamename text,score integer,createtime text)");
						if(sqlite3_exec(db,cmd,NULL,NULL,&errmsg)!=SQLITE_OK)
						{
								printf("数据库遇到致命错误\n");
								printf("%s\n",errmsg);
								exit(-1);
						}
				}
		}
		printf("数据库初始化完毕\n");
}

void Bake_sqlite(void)
{
		char cmd[MAX_CMD];
		printf("正在准备备份数据\n");
		printf("正在检测备份更新\n");
		char date[20];
		char baketime[20];
		int i;
		int flag=0;
		FILE * fp;
		char * errmsg;
		time_t ti;
		ti=time(NULL);
		struct tm * tt;
		tt=localtime(&ti);
		fp=fopen("./date.conf","r");
		if(fp==NULL)
		{
				perror("打开日期文件出错\n");
				return;
		}
		if(fgets(date,20,fp))
		{
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
		fp=NULL;
		if(flag==1)
		{
				printf("正在备份数据库...\n");
				strcpy(cmd,"drop table if exists client_bake");
				if(sqlite3_exec(db,cmd,NULL,NULL,&errmsg)!=SQLITE_OK)
				{
						printf("数据库遇到致命错误(drop bake client)\n");
						printf("%s\n",errmsg);
						exit(-1);
				}
				strcpy(cmd,"drop table if exists history_bake");
				if(sqlite3_exec(db,cmd,NULL,NULL,&errmsg)!=SQLITE_OK)
				{
						printf("数据库遇到致命错误(drop bake history)\n");
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
				strcpy(cmd,"drop table if exists document_bake");
				if(sqlite3_exec(db,cmd,NULL,NULL,&errmsg)!=SQLITE_OK)
				{
						printf("数据库遇到致命错误(drop bake document)\n");
						printf("%s\n",errmsg);
						exit(-1);
				}
				strcpy(cmd,"drop table if exists chat_bake");
				if(sqlite3_exec(db,cmd,NULL,NULL,&errmsg)!=SQLITE_OK)
				{
						printf("数据库遇到致命错误(drop bake chat)\n");
						printf("%s\n",errmsg);
						exit(-1);
				}
				strcpy(cmd,"drop table if exists game_rank_bake");
				if(sqlite3_exec(db,cmd,NULL,NULL,&errmsg)!=SQLITE_OK)
				{
						printf("数据库遇到致命错误(drop bake game_rank)\n");
						printf("%s\n",errmsg);
						exit(-1);
				}
				strcpy(cmd,"create table client_bake as select * from client");
				if(sqlite3_exec(db,cmd,NULL,NULL,&errmsg)!=SQLITE_OK)
				{
						printf("数据库遇到致命错误(bake client)\n");
						printf("%s\n",errmsg);
						exit(-1);
				}
				strcpy(cmd,"create table if not exists dictionary_bake as select * from dictionary");
				if(sqlite3_exec(db,cmd,NULL,NULL,&errmsg)!=SQLITE_OK)
				{
						printf("数据库遇到致命错误(bake dictionary)\n");
						printf("%s\n",errmsg);
						exit(-1);
				}
				strcpy(cmd,"create table history_bake as select * from history");
				if(sqlite3_exec(db,cmd,NULL,NULL,&errmsg)!=SQLITE_OK)
				{
						printf("数据库遇到致命错误(bake history)\n");
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
				strcpy(cmd,"create table chat_bake as select * from chat");
				if(sqlite3_exec(db,cmd,NULL,NULL,&errmsg)!=SQLITE_OK)
				{
						printf("数据库遇到致命错误(bake chat)\n");
						printf("%s\n",errmsg);
						exit(-1);
				}
				strcpy(cmd,"create table document_bake as select * from document");
				if(sqlite3_exec(db,cmd,NULL,NULL,&errmsg)!=SQLITE_OK)
				{
						printf("数据库遇到致命错误\n");
						printf("%s\n",errmsg);
						exit(-1);
				}
				strcpy(cmd,"create table game_rank_bake as select * from game_rank");
				if(sqlite3_exec(db,cmd,NULL,NULL,&errmsg)!=SQLITE_OK)
				{
						printf("数据库遇到致命错误\n");
						printf("%s\n",errmsg);
						exit(-1);
				}
				printf("数据库备份完毕\n");
				sprintf(baketime,"%d-%02d-%02d\n",tt->tm_year+1900,tt->tm_mon+1,tt->tm_mday);
				fp=fopen("date.conf","w");
				if(fp==NULL)
				{
						printf("更新备份时间失败\n");
						return;
				}
				fputs(baketime,fp);
				fclose(fp);
		}
		else
				printf("备份已至最新，无需再次备份\n");
}
