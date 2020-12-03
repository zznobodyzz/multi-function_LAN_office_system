#include"client_head.h"

extern char name[MAX_NAME];
extern int sync_err_flag;
extern int free_flag;
extern pthread_mutex_t free_lock;

int Super_check(int sockfd)
{
		USR usr;
		SER ser;
		usr.cmd=SCK;
		strcpy(usr.name,name);
		write(sockfd,&usr,sizeof(usr));
		if(read(sockfd,&ser,sizeof(ser))<=0)
		{
				printf("服务器未响应,请稍后再试\n");
				return 1;
		}
		if(ser.sig!=GET_SCK)
		{
				printf("与服务器同步发生错误,请稍后再试\n");
				sync_err_flag=1;
				return 1;
		}
		write(sockfd,&usr,sizeof(usr));
		if(read(sockfd,&ser,sizeof(ser))<=0)
		{
				printf("服务器未响应,请稍后再试\n");
				return 1;
		}
		if(ser.sig==SER_ERR)
		{
				printf("服务器异常,请稍后再试\n");
				return 1;
		}
		else if(ser.sig==SCK_OK)
		{
				printf("权限确认通过,请输入登录密码:\n");
				fgets(usr.data,MAX_PASSWD,stdin);
				usr.data[strlen(usr.data)-1]='\0';
				write(sockfd,&usr,sizeof(usr));
				if(read(sockfd,&ser,sizeof(ser))<=0)
				{
						printf("服务器未响应,请稍后再试\n");
						return 1;
				}
				if(ser.sig==PWD_OK)
				{
						printf("验证通过\n");
						return 0;
				}
				else if(ser.sig==PWD_ERR)
				{
						printf("密码错误\n");
						return 1;
				}
		}
		else if(ser.sig==SCK_NO)
		{
				printf("权限不足\n");
				return 1;
		}
}

void Private_space(int sockfd)
{
		int choose;
		while(1)
		{
				Private_menu();
				scanf("%d",&choose);
				getchar();
				Private_menu();
				switch(choose)
				{
						case 1: free_flag=1;
								pthread_mutex_lock(&free_lock);
								View_doc(sockfd,1);
								pthread_mutex_unlock(&free_lock);
								break;
						case 2: free_flag=1;
								pthread_mutex_lock(&free_lock);
								Download_doc(sockfd,1);
								pthread_mutex_unlock(&free_lock);
								break;
						case 3: free_flag=1;
								pthread_mutex_lock(&free_lock);
								Upload_doc(sockfd,1);
								pthread_mutex_unlock(&free_lock);
								break;
						case 4: free_flag=1;
								pthread_mutex_lock(&free_lock);
								Cancel_doc(sockfd);
								pthread_mutex_unlock(&free_lock);
								break;
						case 0: return;
						default:printf("请输入有效数字\n");
								break;
				}
				free_flag=0;
				printf("请按回车键继续\n");
				getchar();
		}
}

void Super_load(int sockfd)
{
		USR usr;
		SER ser;
		strcpy(usr.name,name);
		usr.cmd=DMD;
		char flag;
		write(sockfd,&usr,sizeof(usr));
		if(read(sockfd,&ser,sizeof(ser))<=0)
		{
				printf("服务器未响应,请稍后再试\n");
				return;
		}
		if(ser.sig!=GET_DMD)
		{
				printf("与服务器同步发生错误,请稍候再试\n");
				sync_err_flag=1;
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
				printf("服务器异常,请稍后再试\n");
				return;
		}
		if(ser.sig==MOD_LOW)
		{
				flag='l';
				printf("当前未开启加速,是否开启加速？(y/n)\n");
				if(getchar()=='y')
				{
						getchar();
						usr.cmd=MOD_YES;
				}
				else
				{
						printf("取消了本次操作\n");
						usr.cmd=MOD_NO;
						write(sockfd,&usr,sizeof(usr));
						return;
				}
		}
		if(ser.sig==MOD_HIG)
		{
				flag='h';
				printf("当前已开启加速,是否关闭加速？(y/n)\n");
				if(getchar()=='y')
				{
						getchar();
						usr.cmd=MOD_YES;
				}
				else
				{
						printf("取消了本次操作\n");
						usr.cmd=MOD_NO;
						write(sockfd,&usr,sizeof(usr));
						return;
				}
		}
		write(sockfd,&usr,sizeof(usr));
		if(read(sockfd,&ser,sizeof(ser))<=0)
		{
				printf("服务器未响应,请稍后再试\n");
				return;
		}
		if(ser.sig==SER_ERR)
		{
				printf("服务器异常,请稍后再试\n");
				return;
		}
		if(ser.sig==MOD_OK)
		{
				if(flag=='l')
						printf("开启加速成功\n");
				else if(flag=='h')
						printf("关闭加速成功\n");
		}
		return;
}

void Focus_set(int sockfd)
{
		USR usr;
		SER ser;
		strcpy(usr.name,name);
		usr.cmd=SCR;
		write(sockfd,&usr,sizeof(usr));
		if(read(sockfd,&ser,sizeof(ser))<=0)
		{
				printf("服务器未响应,请稍后再试\n");
				return;
		}
		if(ser.sig!=GET_SCR)
		{
				printf("与服务器同步发生错误,请稍候再试\n");
				sync_err_flag=1;
				return;
		}
		while(1)
		{
				printf("请输入需要特别关心的用户名(输入q取消):\n");
				fgets(usr.data,MAX_NAME,stdin);
				usr.data[strlen(usr.data)-1]='\0';
				if(usr.data[0]=='q'&&usr.data[1]=='\0')
				{
						printf("取消了本次操作\n");
						usr.cmd=SCR_CEL;
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
						printf("服务器异常,请稍后再试\n");
						return;
				}
				if(ser.sig==SCR_ERR)
				{
						printf("无效用户名\n");
						continue;
				}
				if(ser.sig==SCR_OK)
				{
						printf("设置成功\n");
						return;
				}
		}
}

void Focus_switch(int sockfd)
{
		USR usr;
		SER ser;
		strcpy(usr.name,name);
		usr.cmd=CMD;
		char flag;
		write(sockfd,&usr,sizeof(usr));
		if(read(sockfd,&ser,sizeof(ser))<=0)
		{
				printf("服务器未响应,请稍后再试\n");
				return;
		}
		if(ser.sig!=GET_CMD)
		{
				printf("与服务器同步发生错误,请稍候再试\n");
				sync_err_flag=1;
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
				printf("服务器异常,请稍后再试\n");
				return;
		}
		if(ser.sig==MOD_LOW)
		{
				flag='l';
				printf("当前未开启特别关心,是否开启？(y/n)\n");
				if(getchar()=='y')
				{
						getchar();
						usr.cmd=MOD_YES;
				}
				else
				{
						printf("取消了本次操作\n");
						usr.cmd=MOD_NO;
						write(sockfd,&usr,sizeof(usr));
						return;
				}
		}
		if(ser.sig==MOD_HIG)
		{
				flag='h';
				printf("当前已开启特别关心,是否关闭？(y/n)\n");
				if(getchar()=='y')
				{
						getchar();
						usr.cmd=MOD_YES;
				}
				else
				{
						printf("取消了本次操作\n");
						usr.cmd=MOD_NO;
						write(sockfd,&usr,sizeof(usr));
						return;
				}
		}
		write(sockfd,&usr,sizeof(usr));
		if(read(sockfd,&ser,sizeof(ser))<=0)
		{
				printf("服务器未响应,请稍后再试\n");
				return;
		}
		if(ser.sig==SER_ERR)
		{
				printf("服务器异常,请稍后再试\n");
				return;
		}
		if(ser.sig==MOD_OK)
		{
				if(flag=='l')
						printf("开启加速成功\n");
				else if(flag=='h')
						printf("关闭加速成功\n");
		}
		return;
}

void VIP_func(int sockfd)
{
		int choose;
		int retu;
		//如果未有高级权限，返回
		system("clear");
		printf("正在进行权限认证...\n");
		if(Super_check(sockfd)==1)
				return;
		while(1)
		{
				retu=0;
				Super_menu();
				scanf("%d",&choose);
				getchar();
				switch(choose)
				{
						//私人空间
						case 1: Private_space(sockfd);
								break;
								//开关下载加速
						case 2: free_flag=1;
								pthread_mutex_lock(&free_lock);
								Super_load(sockfd);
								pthread_mutex_unlock(&free_lock);
								break;
								//设置上线下线提醒
						case 3: while(1)
								{
										Focus_menu();
										scanf("%d",&choose);
										getchar();
										Focus_menu();
										switch(choose)
										{
												//设置名单
												case 1: free_flag=1;
														pthread_mutex_lock(&free_lock);
														Focus_set(sockfd);
														pthread_mutex_unlock(&free_lock);
														break;
														//开关
												case 2:	free_flag=1;
														pthread_mutex_lock(&free_lock);
														Focus_switch(sockfd);
														pthread_mutex_unlock(&free_lock);
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
										printf("请按回车键继续\n");
										getchar();
								}
								break;
						case 0: return;
						default:printf("请输入有效数字\n");
								break;
				}
				if(sync_err_flag==1)
						return;
				free_flag=0;
				printf("请按回车继续\n");
				getchar();
		}
}
