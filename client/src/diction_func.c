#include"client_head.h"
extern char name[MAX_NAME];
extern int sync_err_flag;
extern int free_flag;
extern pthread_mutex_t free_lock;

void History_dic(int sockfd)
{
		USR usr;
		SER ser;
		memset(ser.data,0,MAX_DATA);
		int choose;
		int flag=0;
		strcpy(usr.name,name);
		usr.cmd=HIS;
		write(sockfd,&usr,sizeof(usr));
		if(read(sockfd,&ser,sizeof(ser))<=0)
		{
				printf("服务器未响应,请稍后再试\n");
				return;
		}
		if(ser.sig!=GET_HIS)
		{
				printf("与服务器同步发生错误,请稍后再试\n");
				sync_err_flag=1;
				return;
		}
		while(1)
		{
				Dic_menu();
				printf("1.查看本机查询记录\n");
				printf("2.查询所有用户记录(需要高级权限)\n");
				printf("0.返回\n");
				scanf("%d",&choose);
				getchar();
				Dic_menu();
				switch(choose)
				{
						case 1: usr.cmd=HIS_SEF;
								flag=1;
								break;
						case 2: usr.cmd=HIS_ALL;
								flag=1;
								break;
						case 0: usr.cmd=HIS_CEL;
								write(sockfd,&usr,sizeof(usr));
								return;
						default:printf("请输入1或2\n");
								break;
				}
				if(flag==1)
						break;
		}
		write(sockfd,&usr,sizeof(usr));
		if(read(sockfd,&ser,sizeof(ser))<=0)
		{
				printf("服务器未响应,请稍后再试\n");
				return;
		}
		if(ser.sig==SER_ERR)
		{
				printf("服务器异常，请稍后再试\n");
				return;
		}
		else if(ser.sig==HIS_ERR)
		{
				printf("未查询到相关记录\n");
				return;
		}
		else if(ser.sig==HIS_UNA)
		{
				printf("权限不足,无法查看\n");
				return;
		}
		else if(ser.sig==HIS_OK)
		{
				printf("权限认证通过,正在查找记录\n");
				printf("%s\n",ser.data);
				while(1)
				{
						if(read(sockfd,&ser,sizeof(ser))<=0)
						{
								printf("服务器失去响应\n");
								return;
						}
						if(ser.sig==HIS_OK)
								printf("%s\n",ser.data);
						if(ser.sig==HIS_END)
								break;
				}
				printf("记录显示完毕\n");
				return;
		}
}

void Diction(int sockfd)
{
		USR usr;
		SER ser;
		strcpy(usr.name,name);
		usr.cmd=DIC;
		write(sockfd,&usr,sizeof(usr));
		if(read(sockfd,&ser,sizeof(ser))<=0)
		{
				printf("服务器未响应,请稍后再试\n");
				return;
		}
		if(ser.sig!=GET_DIC)
		{
				printf("与服务器同步发生错误,查询失败\n");
				sync_err_flag=1;
				return;
		}
		else
		{
				while(1)
				{
						Dic_menu();
						printf("请输入要查询的单词:\n");
						printf("输入0返回\n");
						fgets(usr.data,MAX_DATA,stdin);
						Dic_menu();
						usr.data[strlen(usr.data)-1]='\0';
						if(strncmp(usr.data,"0",1)==0)
						{
								usr.cmd=DIC_END;
								write(sockfd,&usr,sizeof(usr));
								return;
						}
						write(sockfd,&usr,sizeof(usr));
						if(read(sockfd,&ser,sizeof(ser))<=0)
						{
								printf("服务器未响应,请稍后再试\n");
								return;
						}
						if(ser.sig==SER_ERR)
						{
								printf("服务器异常，请稍后再试\n");
								return;
						}
						if(ser.sig==DIC_ERR)
						{
								printf("未查询到相关解释\n");
						}
						if(ser.sig==DIC_OK)
						{
								printf("%s:  %s\n",usr.data,ser.data);
								memset(usr.data,0,MAX_DATA);
						}
						printf("请按回车键继续\n");
						getchar();
				}
		}
}

void Diction_func(int sockfd)
{
		int choose;
		int retu=0;
		while(1)
		{
				Dic_menu();
				scanf("%d",&choose);
				getchar();
				switch(choose)
				{
						case 1: free_flag=1;
								pthread_mutex_lock(&free_lock);
								Diction(sockfd);
								pthread_mutex_unlock(&free_lock);
								break;
						case 2: free_flag=1;
								pthread_mutex_lock(&free_lock);
								History_dic(sockfd);
								pthread_mutex_unlock(&free_lock);
								printf("请按回车继续\n");
								getchar();
								break;
						case 0: retu=1;
								break;
						default:printf("请输入有效数字\n");
								break;
				}
				if(retu==1)
						break;
				if(sync_err_flag==1)
						return;
				free_flag=0;
		}
		return;
}
