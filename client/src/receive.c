#include"client_head.h"
#include"client_game.h"
extern void * Count(void * arg);
extern int udpfd;
extern int countchat;
extern char call[MAX_NAME];
extern char name[MAX_NAME];
extern sqlite3 * db;
extern int gaming_flag;
extern char game_pk_name[MAX_NAME];

void * Advertise(void * arg)
{
		pthread_detach(pthread_self());
		SER ser;
		struct sockaddr_in saddr;
		int len;
		int i=5;
		int count=0;
		char * q;
		time_t ti;
		char chattime[MAX_TIME];
		struct tm * t;
		pthread_t tcount;
		char cmd[MAX_CMD];
		char * errmsg;
		char broadname[MAX_NAME];
		len=sizeof(struct sockaddr_in);
		while(1)
		{
				if(recvfrom(udpfd,&ser,sizeof(ser),0,(struct sockaddr *)&saddr,&len)<0)
				{
						printf("推送接收服务异常,已自动关闭\n");
						perror("recvfrom failed");
						pthread_exit(NULL);
				}
				switch(ser.sig)
				{
						case SYS_INFO:
								printf("收到系统广播:  %s\n",ser.data);
								if(strncmp(ser.data,"/system warning99/",18)==0)
								{
										printf("探测到服务器关闭信号,程序即将在5秒后关闭\n");
										while(i>0)
										{
												printf("%d",i);
												fflush(stdout);
												i--;
												printf("\b");
												fflush(stdout);
												sleep(1);
										}
										Bake_data();
										for(i=3;i<getdtablesize();i++)
										{
												close(i);
										}
										printf("感谢使用,请等待管理员重启服务器\n");
										exit(0);
								}
								if(strncmp(ser.data,"/system warning96/",18)==0)
								{
										printf("5分钟内无法再次进入聊天室\n");
										countchat=1;
										pthread_create(&tcount,NULL,Count,NULL);
								}
								break;
						case ADM_INFO:
								printf("收到管理员广播:  %s\n",ser.data);
								break;
						case PRIV_INFO:
								printf("收到私聊消息:\n%s\n",ser.data);
								q=ser.data;
								while(*q!='>')
								{
										q++;
										count++;
								}
								q+=4;
								memset(call,0,MAX_NAME);
								strncpy(call,ser.data,count);
								count=0;
								ti=time(NULL);
								t=localtime(&ti);
								sprintf(chattime,"%d-%02d-%02d %02d:%02d:%02d",t->tm_year+1900,t->tm_mon+1,t->tm_mday,t->tm_hour,t->tm_min,t->tm_sec);
								sprintf(cmd,"insert into chathistory values('%s','%s','%s','%s')",call,name,q,chattime);
								if(sqlite3_exec(db,cmd,NULL,NULL,&errmsg)!=SQLITE_OK)
								{
										printf("添加私聊记录 操作失败\n");
										printf("%s\n",errmsg);
								}
								break;
						case BROA_INFO:
								printf("收到群聊消息:\n%s\n",ser.data);
								q=ser.data;
								while(*q!='-')
								{
										q++;
										count++;
								}
								q+=3;
								memset(broadname,0,MAX_NAME);
								strncpy(broadname,ser.data,count);
								count=0;
								ti=time(NULL);
								t=localtime(&ti);
								sprintf(chattime,"%d-%02d-%02d %02d:%02d:%02d",t->tm_year+1900,t->tm_mon+1,t->tm_mday,t->tm_hour,t->tm_min,t->tm_sec);
								sprintf(cmd,"insert into chathistory values('%s','broadcast','%s','%s')",broadname,q,chattime);
								if(sqlite3_exec(db,cmd,NULL,NULL,&errmsg)!=SQLITE_OK)
								{
										printf("添加群聊记录 操作失败\n");
										printf("%s\n",errmsg);
								}
								break;
						case WUZIQI_PK:
								if(gaming_flag==1)
										break;
								strcpy(game_pk_name,ser.data);
								kill(getpid(),SIGUSR1);
								break;
						default:break;
				}
		}
}
