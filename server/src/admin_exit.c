#include"server_head.h"

extern sqlite3 * db;
extern int udpfd;
extern pthread_mutex_t game_queue_lock;
extern pthread_t ttellip;
extern pthread_t tadv;

void Exit_server(void)
{
		char cmd[MAX_CMD];
		int nrow;
		int index;
		int i;
		int ncolumn;
		SER ser;
		ser.sig=SYS_INFO;
		char * errmsg;
		char ** result;
		struct sockaddr_in caddr;
		caddr.sin_family=PF_INET;
		caddr.sin_port=htons(SERVER_UDP_PORT);
		int len=sizeof(struct sockaddr_in);
		printf("关闭服务器会导致所有在线用户无法正常访问，确认要继续吗？\n");
		if(getchar()=='y')
		{
				getchar();
				Bake_sqlite();
				printf("服务器即将关闭\n");
				strcpy(ser.data,"/system warning99/ : 服务器即将关闭,造成不便敬请谅解\n");
				strcpy(cmd,"select addr from client where stat=1");
				if(sqlite3_get_table(db,cmd,&result,&nrow,&ncolumn,&errmsg)!=SQLITE_OK)
				{
						printf("%s\n",errmsg);
				}
				else if(nrow!=0)
				{
						index=ncolumn;
						for(i=0;i<nrow;i++)
						{
								caddr.sin_addr.s_addr=inet_addr(result[index++]);
								if(sendto(udpfd,&ser,sizeof(ser),0,(struct sockaddr *)&caddr,len)<0)
								{
										printf("向用户发送服务器关闭信号 操作失败,用户可能已下线\n");
								}	
						}
				}
				strcpy(cmd,"update client set stat=0,chat=0,game=0 where stat=1");
				if(sqlite3_exec(db,cmd,NULL,NULL,&errmsg)!=SQLITE_OK)
				{
						printf("强制所有用户下线失败,服务器正常关闭\n");
						printf("%s\n",errmsg);
				}
				pthread_cancel(ttellip);
				pthread_cancel(tadv);
				sqlite3_close(db);
				Release_game_queue();
				pthread_mutex_destroy(&game_queue_lock);
				for(i=0;i<getdtablesize();i++)
						close(i);
				printf("感谢使用\n");
				exit(0);
		}
		else
				printf("取消了本次操作\n");
}
