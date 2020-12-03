#include"server_head.h"

extern sqlite3 * db;
extern int udpfd;

void Delete_cache(void)
{
		char cmd[MAX_CMD];
		char * errmsg;
		char cachetime[MAX_TIME];
		int choose;
		int year,mon,mday;
		SER ser;
		char ** result;
		int nrow;
		int ncolumn;
		struct sockaddr_in caddr;
		int len;
		int i;
		int index;
		len=sizeof(struct sockaddr_in);
		caddr.sin_family=PF_INET;
		caddr.sin_port=htons(SERVER_UDP_PORT);
		time_t ti;
		ti=time(NULL);
		struct tm * t;
		t=localtime(&ti);
		if(t->tm_mday-1==0)
		{
				mon=t->tm_mon;
				if(mon==0)
				{
						year=t->tm_year+1900-1;
						mon=12;
						mday=31;
				}
				else
				{
						year=t->tm_year+1900;
						mday=t->tm_mday-1;
				}
		}
		else
		{
				year=t->tm_year+1900;
				mon=t->tm_mon+1;
				mday=t->tm_mday-1;
		}
		printf("只能清理一天以前的历史记录缓存,服务器自动保留前一天所有记录\n");
		printf("1.清理查找记录缓存\n");
		printf("2.清理聊天记录缓存\n");
		printf("3.清理文档查询上传下载记录缓存\n");
		printf("4.清理以上全部\n");
		printf("0.返回\n");
		scanf("%d",&choose);
		getchar();
		switch(choose)
		{
				case 1: printf("确认清理所有用户查找历史记录缓存？\n");
						if(getchar()=='y')
						{
								getchar();
								sprintf(cmd,"delete from history where dictime like '%d-%02d-%02d%%'",year,mon,mday);
						}
						else
						{
								printf("取消了本次操作\n");
						}
						break;
				case 2: printf("确认清理所有用户聊天记录缓存？\n");
						if(getchar()=='y')
						{
								getchar();
								sprintf(cmd,"delete from chat where chattime like '%d-%02d-%02d%%'",year,mon,mday);
						}
						else
						{
								printf("取消了本次操作\n");
						}
						break;
				case 3: printf("确认清理所有用户查询上传下载记录缓存？\n");
						if(getchar()=='y')
						{
								getchar();
								sprintf(cmd,"delete from dochistory where time like '%d-%02d-%02d%%'",year,mon,mday);
						}
						else
						{
								printf("取消了本次操作\n");
						}
						break;
				case 4: printf("确认清理所有缓存？(y/n)\n");
						if(getchar()=='y')
						{
								getchar();
								sprintf(cmd,"delete from history where dictime like '%d-%02d-%02d%%'",year,mon,mday);
								if(sqlite3_exec(db,cmd,NULL,NULL,&errmsg)!=SQLITE_OK)
								{
										printf("操作失败\n");
										printf("%s\n",errmsg);
								}
								sprintf(cmd,"delete from chat where chattime like '%d-%02d-%02d%%'",year,mon,mday);
								if(sqlite3_exec(db,cmd,NULL,NULL,&errmsg)!=SQLITE_OK)
								{
										printf("操作失败\n");
										printf("%s\n",errmsg);
								}
								sprintf(cmd,"delete from dochistory where time like '%d-%02d-%02d%%'",year,mon,mday);
								if(sqlite3_exec(db,cmd,NULL,NULL,&errmsg)!=SQLITE_OK)
								{
										printf("操作失败\n");
										printf("%s\n",errmsg);
								}
						}
						else
						{
								printf("取消了本次操作\n");
						}
						break;
				case 0: printf("取消了本次操作\n");
						return;
				default:printf("无效选项,默认取消本次操作\n");
						return;
		}
		if(choose==1||choose==2||choose==3)
		{
				if(sqlite3_exec(db,cmd,NULL,NULL,&errmsg)!=SQLITE_OK)
				{
						printf("操作失败\n");
						printf("%s\n",errmsg);
				}
		}
		switch(choose)
		{
				case 1: printf("查找记录缓存已清理\n");
						break;
				case 2: printf("聊天记录缓存已清理\n");
						break;
				case 3: printf("文档查询上传下载记录缓存已清理\n");
						break;
				case 4: printf("所有缓存已清理\n");
		}
		strcpy(cmd,"select addr from client where stat=1");
		switch(choose)
		{
				case 1: strcpy(ser.data,"/system warning98/ : 管理员清理了字典查询记录缓存,系统已自动保留前一天历史记录\n");
						break;
				case 2: strcpy(ser.data,"/system warning98/ : 管理员清理了聊天记录缓存,系统已自动保留前一天历史记录\n");
						break;
				case 3: strcpy(ser.data,"/system warning98/ : 管理员清理了文档查询上传下载记录缓存,系统已自动保留前一天历史记录\n");
						break;
				case 4: strcpy(ser.data,"/system warning98/ : 管理员清理了记录缓存,系统已自动保留前一天历史记录\n");
						break;
		}
		if(sqlite3_get_table(db,cmd,&result,&nrow,&ncolumn,&errmsg)!=SQLITE_OK)
		{
				printf("%s\n",errmsg);
		}
		else if(nrow!=0)
		{
				ser.sig=SYS_INFO;
				index=ncolumn;
				for(i=0;i<nrow;i++)
				{
						caddr.sin_addr.s_addr=inet_addr(result[index++]);
						if(sendto(udpfd,&ser,sizeof(ser),0,(struct sockaddr *)&caddr,len)<0)	
						{
								printf("向用户发送缓存清理提醒 操作失败,用户可能已下线\n");
						}
				}
		}
		printf("向用户发送缓存清理提醒成功\n");
}
