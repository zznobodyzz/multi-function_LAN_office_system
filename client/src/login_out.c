#include"client_head.h"
extern void * Advertise(void * arg);
extern int sync_err_flag;
extern char name[MAX_NAME];
extern char passwd[MAX_PASSWD];
extern int free_flag;
extern pthread_mutex_t free_lock;
extern int work_queue_for_load;
extern pthread_mutex_t work_queue_lock;
extern int chatflag;
extern pthread_t tadvertise;
extern login_flag;
extern logout;

int Login(int sockfd)
{
		USR usr;
		SER ser;
		usr.cmd=LOGIN;
		write(sockfd,&usr,sizeof(usr));
		if(read(sockfd,&ser,sizeof(ser))<=0)
		{
				printf("服务器未响应,请稍后再试\n");
				return 0;
		}
		if(ser.sig!=GET_LOG)
		{
				printf("与服务器同步发生错误,登录失败\n");
				return 0;
		}
		if(sync_err_flag==1)
				strcpy(usr.name,name);
		else
		{
				printf("请输入用户名:\n");
				fgets(usr.name,MAX_NAME,stdin);
				usr.name[strlen(usr.name)-1]='\0';
				strcpy(name,usr.name);
		}
		write(sockfd,&usr,sizeof(usr));
		if(read(sockfd,&ser,sizeof(ser))<=0)
		{
				printf("服务未响应,请稍后再试\n");
				return 0;
		}
		if(ser.sig==SER_ERR)
		{
				printf("服务器操作异常,请稍后再试\n");
				return 0;
		}
		else if(ser.sig==LOG_ERR)
		{
				printf("用户名不存在\n");
				return 0;
		}
		else if(ser.sig==LOG_PWD)
		{
				if(sync_err_flag==1)
						strcpy(usr.data,passwd);
				else
				{
					printf("请输入密码:\n");
					fgets(usr.data,MAX_PASSWD,stdin);
					usr.data[strlen(usr.data)-1]='\0';
				}
				write(sockfd,&usr,sizeof(usr));
				if(read(sockfd,&ser,sizeof(ser))<=0)
				{
						printf("服务器未响应,请稍后再试\n");
						return 0;
				}
				if(ser.sig==SER_ERR)
				{
						printf("服务器操作异常,请稍后再试\n");
						return 0;
				}
				else if(ser.sig==PWD_ERR)
				{
						printf("密码错误\n");
						return 0;
				}
				else if(ser.sig==PWD_OK)
				{
						if(sync_err_flag==0)
						{
								strcpy(passwd,usr.data);
								printf("密码验证成功,等待服务器确认\n");
						}
						if(read(sockfd,&ser,sizeof(ser))<=0)
						{
								printf("服务器未响应,请稍后再试\n");
								return 0;
						}
						if(ser.sig==SER_ERR)
						{
								printf("服务器操作异常,请稍后再试\n");
								return 0;
						}
						else if(ser.sig==LOG_OK)
						{
								if(sync_err_flag==1)
								{
										printf("重连成功\n");
										sync_err_flag=0;
								}
								else
										printf("收到服务器验证,登录成功\n");
								return 1;
						}
				}
		}
}

int  Regist(int sockfd)
{
		USR usr;
		SER ser;
		usr.cmd=REGISTER;
		write(sockfd,&usr,sizeof(usr));
		if(read(sockfd,&ser,sizeof(ser))<=0)
		{
				printf("服务器异常,请稍后再试\n");
				return 0;
		}
		if(ser.sig!=GET_REG)
		{
				printf("与服务器同步发生错误,注册失败\n");
				sync_err_flag=1;
				return 0;
		}
		else
		{
				while(1)
				{
						printf("请输入用户名:(30字节以内)\n");
						fgets(usr.name,MAX_NAME,stdin);
						usr.name[strlen(usr.name)-1]='\0';
						write(sockfd,&usr,sizeof(usr));
						if(read(sockfd,&ser,sizeof(ser))<=0)
						{
								printf("服务器未响应,请稍后再试\n");
								return 0;
						}
						if(ser.sig==SER_ERR)
						{
								printf("服务器异常,请稍后再试\n");
								return 0;
						}
						else if(ser.sig==NAME_ERROR)
						{
								printf("该用户名已被注册,请重新输入\n");
						}
						else if(ser.sig==NAME_OK)
								break;
				}
				printf("用户名可用\n");
				strcpy(name,usr.name);
				printf("请输入密码:(14位以内)\n");
				fgets(usr.data,MAX_PASSWD,stdin);
				usr.data[strlen(usr.data)-1]='\0';
				write(sockfd,&usr,sizeof(usr));
				if(read(sockfd,&ser,sizeof(ser))<=0)
				{
						printf("服务器未响应,请稍后再试\n");
						return 0;
				}
				if(ser.sig==SER_ERR)
				{
						printf("服务器异常，请稍后再试\n");
						return 0;
				}
				else if(ser.sig==REG_OK)
				{
						strcpy(passwd,usr.data);
						printf("注册成功! 已自动登录\n");
						return 1;
				}
		}
}

void Logout(int sockfd)
{
		USR usr;
		SER ser;
		usr.cmd=LOGOUT;
		strcpy(usr.name,name);
		write(sockfd,&usr,sizeof(usr));
		write(sockfd,&usr,sizeof(usr));
		if(read(sockfd,&ser,sizeof(ser))<=0)
		{
				printf("服务器未响应,请稍后再试\n");
				return;
		}
		if(ser.sig!=GET_LOU)
		{
				printf("与服务器同步发生错误,请稍后再试\n");
				sync_err_flag=1;
				return;
		}
		if(read(sockfd,&ser,sizeof(ser))<=0)
		{
				printf("服务器未响应,请稍后再试\n");
				return;
		}
		if(ser.sig==SER_ERR)
		{
				printf("服务器异常,注销失败,请稍后再试\n");
				return;
		}
		else if(ser.sig==LOU_OK)
		{
				printf("注销成功\n");
				return;
		}
}

void Logout_func(int sockfd)
{
		pthread_mutex_lock(&free_lock);
		if(work_queue_for_load)
		{	
				printf("当前还有%d个下载/上传任务正在进行,确定登出？(y/n)",work_queue_for_load);
				if(getchar()!='y')
				{
						printf("取消了本次操作\n");
						getchar();
						return;
				}
		}
		pthread_mutex_lock(&work_queue_lock);
		Destroy_work_queue();
		pthread_mutex_unlock(&work_queue_lock);
		pthread_mutex_unlock(&free_lock);
		if(chatflag==1)
		{
				Exit_chat(sockfd);
				chatflag=0;
		}
		Logout(sockfd);
		pthread_cancel(tadvertise);
		login_flag=0;
		logout=1;
		close(sockfd);
}


void Init(int sockfd)
{
		int choose;
		pthread_t tadvertise;
		int ret=0;
		printf("请先登录/注册\n");
		while(1)
		{
				printf("1.登录\n");
				printf("2.注册\n");
				printf("0.退出\n");
				scanf("%d",&choose);
				getchar();
				switch(choose)
				{
						case 1: ret=Login(sockfd);
								break;
						case 2: ret=Regist(sockfd);
								break;
						case 0: Exit(sockfd);
								break;
						default:printf("请选择1、2或0");
								break;
				}
				if(ret==1)
				{
						pthread_create(&tadvertise,NULL,Advertise,NULL);
						return;
				}
				printf("请按回车键继续\n");
				getchar();
				system("clear");
		}
}
