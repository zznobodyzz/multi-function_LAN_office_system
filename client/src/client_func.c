#include"client_head.h"
extern char name[MAX_NAME];
extern int sync_err_flag;
extern int free_flag;
extern pthread_mutex_t free_lock;

void Passwd(int sockfd)
{
		USR usr;
		SER ser;
		usr.cmd=PWD;
		strcpy(usr.name,name);
		char buf[MAX_DATA];
		write(sockfd,&usr,sizeof(usr));
		if(read(sockfd,&ser,sizeof(ser))<=0)
		{
				printf("服务器未响应,请稍后再试\n");
				return;
		}
		if(ser.sig!=GET_PWD)
		{
				printf("与服务器同步发生错误,请稍后再试\n");
				sync_err_flag=1;
				return;
		}
		printf("请输入旧密码:\n");
		fgets(usr.data,MAX_DATA,stdin);
		usr.data[strlen(usr.data)-1]='\0';
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
		if(ser.sig==PWD_ERR)
		{
				printf("旧密码不正确\n");
				return;
		}
		if(ser.sig==PWD_NEW)
		{
				while(1)
				{
						printf("请输入新密码(14位以内):\n");
						fgets(usr.data,MAX_DATA,stdin);
						usr.data[strlen(usr.data)-1]='\0';
						printf("请再次输入新密码:\n");
						fgets(buf,MAX_DATA,stdin);
						buf[strlen(buf)-1]='\0';
						if(strcmp(usr.data,buf)==0)
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
						printf("服务器异常,操作失败\n");
				}
				else
				{
						printf("修改密码成功\n");
				}
				return;
		}
}

void Onlinelist_all(int sockfd)
{
		USR usr;
		SER ser;
		int choose;
		int flag=0;
		usr.cmd=ONLINELIST;
		strcpy(usr.name,name);
		write(sockfd,&usr,sizeof(usr));
		if(read(sockfd,&ser,sizeof(ser))<=0)
		{
				printf("服务器未响应,请稍后再试\n");
				return;
		}
		if(ser.sig!=GET_ONL)
		{
				printf("与服务器同步发生错误,请稍后再试\n");
				sync_err_flag=1;
				return;
		}
		else
		{
				while(1)
				{
						Client_menu();
						printf("1.查看在线用户\n");
						printf("2.查看所有用户(需要管理员权限)\n");
						printf("0.返回\n");
						scanf("%d",&choose);
						getchar();
						Client_menu();
						switch(choose)
						{
								case 1: while(1)
										{
												printf("1.查看所有信息(需要管理员权限)\n");
												printf("2.仅查看姓名\n");
												printf("0.返回\n");
												scanf("%d",&choose);
												getchar();
												Menu();
												switch(choose)
												{
														case 1: usr.cmd=ONL_ALL;
																flag=1;
																break;
														case 2: usr.cmd=ONL_NAME;
																flag=1;
																break;
														case 0: flag=2;
																break;
														default:printf("请输入有效数字\n");
																break;
												}
												if(flag!=0)
														break;
										}
										break;
								case 2: while(1)
										{
												printf("1.查看所有信息(需要管理员权限)\n");
												printf("2.仅查看姓名(需要管理员权限)\n");
												printf("0.返回\n");
												scanf("%d",&choose);
												getchar();
												Menu();
												switch(choose)
												{
														case 1: usr.cmd=ALL_ALL;
																flag=1;
																break;
														case 2: usr.cmd=ALL_NAME;
																flag=1;
																break;
														case 0: flag=2;
																break;
														default:printf("请输入有效数字\n");
																break;
												}
												if(flag!=0)
														break;
										}
								case 0: usr.cmd=ONL_CEL;
										write(sockfd,&usr,sizeof(usr));
										return;
								default:printf("请输入有效数字\n");
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
						printf("服务器异常,请稍后再试\n");
						return;
				}
				if(ser.sig==ONL_ERR)
				{
						printf("对不起,权限不足\n");
						return;
				}
				if(ser.sig==ONL_OK)
				{
						printf("正在读取信息\n");
						while(1)
						{
								printf("%s\n",ser.data);
								if(read(sockfd,&ser,sizeof(ser))<=0)
								{
										printf("服务器失去响应\n");
										return;
								}
								if(ser.sig==ONL_END)
										break;
						}
						printf("信息读取完毕\n");
				}
				return;
		}
}

void Adv(int sockfd)
{
		USR usr;
		SER ser;
		usr.cmd=ADVCEL;
		strcpy(usr.name,name);
		write(sockfd,&usr,sizeof(usr));
		write(sockfd,&usr,sizeof(usr));
		if(read(sockfd,&ser,sizeof(ser))<=0)
		{
				printf("服务器未响应,请稍后再试\n");
				return;
		}
		if(ser.sig!=GET_DVC)
		{
				printf("与服务器同步发生错误,请稍后再试\n");
				sync_err_flag=1;
				return;
		}
		else
		{
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
				if(ser.sig==DVC_UNA)
				{
						printf("对不起,权限不足,请向管理员申请提高权限\n");
						return;
				}
				else if(ser.sig==DVC_ABL)
				{
						if(read(sockfd,&ser,sizeof(ser))<=0)
						{
								printf("服务器未响应,请稍后再试\n");
						}
						if(ser.sig==SER_ERR)
						{
								printf("服务器异常,请稍后再试\n");
						}
						else if(ser.sig==DVC_OFF)
						{
								printf("关闭推送服务成功\n");
						}
						else if(ser.sig==DVC_ON)
						{
								printf("开启推送服务成功\n");
						}
				}
		}
		return;
}

void Mod(int sockfd)
{
		USR usr;
		SER ser;
		usr.cmd=MOD;
		strcpy(usr.name,name);
		write(sockfd,&usr,sizeof(usr));
		if(read(sockfd,&ser,sizeof(ser))<=0)
		{
				printf("服务器未响应,请稍后再试\n");
				return;
		}
		if(ser.sig!=GET_MOD)
		{
				printf("与服务器同步发生错误,请稍后再试\n");
				sync_err_flag=1;
				return;
		}
		printf("注意:用户无法主动升级为管理员,如有需要请与服务器管理员联系\n");
		printf("修改权限需要权限密钥,请输入密钥:\n");
		fgets(usr.data,MAX_DATA,stdin);
		usr.data[strlen(usr.data)-1]='\0';
		write(sockfd,&usr,sizeof(usr));
		if(read(sockfd,&ser,sizeof(ser))<=0)
		{
				printf("服务器未响应,请稍后再试\n");
				return;
		}
		if(ser.sig==MOD_ABL)
		{
				printf("密钥认证通过\n");
				printf("正在确认当前权限\n");
				if(read(sockfd,&ser,sizeof(ser))<=0)
				{
						printf("服务器未响应,请稍后再试\n");
						return;
				}
				if(ser.sig==MOD_LOW)
				{
						printf("当前为普通用户,是否升级？\n");
				}
				else if(ser.sig==MOD_HIG)
				{
						printf("当前为高级用户,是否降级？\n");
				}
				else if(ser.sig==MOD_ADM)
				{
						printf("当前为管理员,是否降低为高级用户？\n");
				}
				if(getchar()=='y')
				{
						getchar();
						usr.cmd=MOD_YES;
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
								printf("操作完成,权限已更改\n");
								return;
						}
				}
				else
				{
						getchar();
						printf("取消了本次操作\n");
						usr.cmd=MOD_NO;
						write(sockfd,&usr,sizeof(usr));
						return;
				}
		}
}

void Client_func(int sockfd)
{
		int choose;
		int retu;
		while(1)
		{
				Client_menu();
				scanf("%d",&choose);
				getchar();
				switch(choose)
				{
						//改密码
						case 1:	free_flag=1;
								pthread_mutex_lock(&free_lock);
								Passwd(sockfd);
								pthread_mutex_unlock(&free_lock);
								break;
								//查看用户表
						case 2: free_flag=1;
								pthread_mutex_lock(&free_lock);
								Onlinelist_all(sockfd);
								pthread_mutex_unlock(&free_lock);
								break;
								//开关自动推送
						case 3: free_flag=1;
								pthread_mutex_lock(&free_lock);
								Adv(sockfd);
								pthread_mutex_unlock(&free_lock);
								break;
								//升/降用户权限
						case 4: free_flag=1;
								pthread_mutex_lock(&free_lock);
								Mod(sockfd);
								pthread_mutex_unlock(&free_lock);
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
