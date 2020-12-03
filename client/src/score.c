#include"client_head.h"
extern char name[MAX_NAME];
extern int sync_err_flag;

void Score(int sockfd)
{
		USR usr;
		SER ser;
		usr.cmd=SCORE;
		strcpy(usr.name,name);
		write(sockfd,&usr,sizeof(usr));
		if(read(sockfd,&ser,sizeof(ser))<=0)
		{
				printf("服务器未响应,请稍后再试\n");
				return;
		}
		if(ser.sig!=GET_SCO)
		{
				printf("与服务器同步发生错误,请稍后再试\n");
				sync_err_flag=1;
				return;
		}
		Menu();
		while(1)
		{
				printf("请给本软件打分\n");
				printf("(满分5分,请不要打0分,谢谢)\n");
				printf("1分.很不满意\n");
				printf("2分.不满意\n");
				printf("3分.基本满意\n");
				printf("4分.非常满意\n");
				printf("5分.超神(推荐)\n");
				fgets(usr.data,MAX_DATA,stdin);
				usr.data[1]='\0';
				if(usr.data[0]==48)
				{
						system("clear");
						printf("皮?\n");
						sleep(3);
						printf("重新打!\n");
						continue;
				}
				if(usr.data[0]<48||usr.data[0]>53)
						printf("请重新打分(输入q取消)\n");
				else 
				{
						Menu();
						printf("感谢评分\n");
						break;
				}
		}
		write(sockfd,&usr,sizeof(usr));
		if(read(sockfd,&ser,sizeof(ser))<=0)
		{
				return;
		}
		if(ser.sig==SCO_OK)
		{
				printf("服务器已收到您的评分\n");
		}
		return;
}
