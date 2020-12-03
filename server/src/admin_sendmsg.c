#include"server_head.h"

extern char advbuf[MAX_DATA];
extern char adv_flag;
extern sqlite3 * db;
extern int udpfd;

void Change_adv(void)
{
		SER ser;
		printf("想要修改成什么内容:\n");
		fgets(advbuf,MAX_DATA,stdin);
		advbuf[strlen(advbuf)-1]='\0';
		Print_server();
		printf("修改成功\n");
		printf("内容将每隔60秒向所有在线用户发送\n");
		return;
}

void Broadcast(void)
{
		if(adv_flag==0)
		{		
				printf("广播服务尚未正常启动\n");
				return;
		}
		char cmd[MAX_CMD];
		int index;
		int i,j;
		int nrow;
		int ncolumn;
		SER ser;
		ser.sig=ADM_INFO;
		struct sockaddr_in caddr;
		caddr.sin_family=PF_INET;
		caddr.sin_port=htons(SERVER_UDP_PORT);
		int len;
		len=sizeof(struct sockaddr_in);
		char **result;
		char * errmsg;
		printf("广播内容:(少于512字节)\n");
		fgets(ser.data,MAX_DATA,stdin);
		ser.data[strlen(ser.data)-1]='\0';
		Print_server();
		strcpy(cmd,"select addr from client where stat=1 and adv=1");
		if(sqlite3_get_table(db,cmd,&result,&nrow,&ncolumn,&errmsg)!=SQLITE_OK)
		{
				printf("获取在线用户地址失败\n");
				printf("%s\n",errmsg);
				return;
		}
		else if(nrow!=0)
		{
				index=ncolumn;
				for(i=0;i<nrow;i++)
				{
						caddr.sin_addr.s_addr=inet_addr(result[index++]);
						sendto(udpfd,&ser,sizeof(ser),0,(struct sockaddr *)&caddr,len);	
				}
				printf("发送完毕\n");
				return;
		}
		else if(nrow==0)
		{
				printf("当前无在线用户,发送失败\n");
				return;
		}
}
