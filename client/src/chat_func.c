#include"client_head.h"
extern int countchat_second;
extern int countchat;
extern char name[MAX_NAME];
extern int sync_err_flag;
extern int chatflag;
extern sqlite3 * db;
extern int free_flag;
extern pthread_mutex_t free_lock;
extern char call[MAX_NAME];

void * Count(void * arg)
{
		pthread_detach(pthread_self());
		countchat_second=301;
		while(--countchat_second)
				sleep(1);
		countchat=0;
		pthread_exit(NULL);
}

int Join_chat(int sockfd)
{
		USR usr;
		SER ser;
		usr.cmd=JCH;
		strcpy(usr.name,name);
		write(sockfd,&usr,sizeof(usr));
		if(read(sockfd,&ser,sizeof(ser))<=0)
		{
				printf("服务器未响应,请稍后再试\n");
				return -1;
		}
		if(ser.sig!=GET_JCH)
		{
				printf("与服务器同步发生错误,请稍后再试\n");
				sync_err_flag=1;
				return -1;
		}
		write(sockfd,&usr,sizeof(usr));
		if(read(sockfd,&ser,sizeof(ser))<=0)
		{
				printf("服务器未响应,请稍后再试\n");
				return -1;
		}
		if(ser.sig==SER_ERR)
		{
				printf("服务器异常,请稍后再试\n");
				return -1;
		}
		else if(ser.sig==JCH_OK)
		{
				printf("登录聊天室成功\n");
				printf("欢迎来到聊天室\n");
		}
		chatflag=1;
		return 0;
}

void Onlinelist_chat(int sockfd)
{
		USR usr;
		SER ser;
		usr.cmd=CHL;
		int ret;
		write(sockfd,&usr,sizeof(usr));
		if(read(sockfd,&ser,sizeof(ser))<=0)
		{
				printf("服务器未响应,请稍后再试\n");
				return;
		}
		if(ser.sig!=GET_CHL)
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
				printf("服务器异常,请稍候再试\n");
				return;
		}
		if(ser.sig==CHL_ERR)
		{
				printf("当前聊天室只有你一人\n");
				return;
		}
		if(ser.sig==CHL_OK)
		{
				printf("聊天室成员如下:\n");
				while((ret=read(sockfd,&ser,sizeof(ser)))>0)
				{
						if(ser.sig==CHL_END)
								break;
						printf("%s\n",ser.data);
				}
				if(ret<=0)
				{
						printf("服务器失去响应,请稍候再试\n");
						return;
				}
				memset(&ser,0,sizeof(ser));
				if(read(sockfd,&ser,sizeof(ser))<=0)
				{
						printf("服务器失去响应,请稍候再试\n");
						return;
				}
				else 
						printf("聊天室里共有%d位用户\n",ser.sig);
		}
		return;
}

void Group_chat(int sockfd)
{
		USR usr;
		SER ser;
		usr.cmd=CFA;
		strcpy(usr.name,name);
		write(sockfd,&usr,sizeof(usr));
		if(read(sockfd,&ser,sizeof(ser))<=0)
		{
				printf("服务器未响应,请稍后再试\n");
				return;
		}
		if(ser.sig!=GET_CFA)
		{
				printf("与服务器同步发生错误,请稍后再试\n");
				sync_err_flag=1;
				return;
		}
		printf("输入q退出\n");
		while(1)
		{
				printf("say:");
				fflush(stdout);
				fgets(usr.data,MAX_DATA,stdin);
				usr.data[strlen(usr.data)-1]='\0';
				if(usr.data[0]=='q'&&usr.data[1]=='\0')
				{
						printf("退出了群聊\n");
						usr.cmd=CFA_CEL;
						write(sockfd,&usr,sizeof(usr));
						return;
				}
				//如果已经被管理员踢出聊天室，自动退出聊天
				if(countchat==1)
				{
						usr.cmd=CFA_CEL;
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
				if(ser.sig==CFA_ERR)
				{
						printf("当前聊天室只有你一人,发送失败\n");
						continue;
				}
				if(ser.sig==CFA_OK)
				{
						time_t ti;
						ti=time(NULL);
						struct tm * t;
						t=localtime(&ti);
						char chattime[MAX_TIME];
						char cmd[MAX_CMD];
						char * errmsg;
						sprintf(chattime,"%d-%02d-%02d %02d:%02d:%02d",t->tm_year+1900,t->tm_mon+1,t->tm_mday,t->tm_hour,t->tm_min,t->tm_sec);
						sprintf(cmd,"insert into chathistory values('%s','broadcast','%s','%s')",usr.name,usr.data,chattime);
						if(sqlite3_exec(db,cmd,NULL,NULL,&errmsg)!=SQLITE_OK)
						{
								printf("添加群聊记录 操作失败\n");
								printf("%s\n",errmsg);
						}
						continue;
				}
		}
}	

void Private_chat(int sockfd,char * call)
{
		USR usr;
		SER ser;
		char dstname[MAX_NAME];
		usr.cmd=CFO;
		strcpy(usr.name,name);
		write(sockfd,&usr,sizeof(usr));
		if(read(sockfd,&ser,sizeof(ser))<=0)
		{
				printf("服务器未响应,请稍后再试\n");
				return;
		}
		if(ser.sig!=GET_CFO)
		{
				printf("与服务器同步发生错误,请稍后再试\n");
				sync_err_flag=1;
				return;
		}
		if(call!=NULL)
		{
				strcpy(usr.data,call);
				strcpy(dstname,call);
		}
		else if(call==NULL)
		{
				printf("请输入目标用户名(输入q返回)\n");
				fgets(usr.data,MAX_NAME,stdin);
				usr.data[strlen(usr.data)-1]='\0';
				if(usr.data[0]=='q'&&usr.data[1]=='\0')
				{
						printf("退出了私聊\n");
						usr.cmd=CFO_CEL;
						write(sockfd,&usr,sizeof(usr));
						return;
				}
				strcpy(dstname,usr.data);
				//如果已经被管理员踢出聊天室，自动退出聊天
				if(countchat==1)
				{
						usr.cmd=CFO_CEL;
						write(sockfd,&usr,sizeof(usr));
						return;
				}
		}
		while(1)
		{
				//each time you send a message,copy dest name to usrdata,cause usrdata include message now
				strcpy(usr.data,dstname);
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
				if(ser.sig==CFO_ERR)
				{
						printf("用户不在线,发送失败\n");
						return;
				}
				if(ser.sig==CFO_DATA)
				{
						printf("输入q退出\n");
						printf("say:");
						fflush(stdout);
						fgets(usr.data,MAX_DATA,stdin);
						usr.data[strlen(usr.data)-1]='\0';
						if(usr.data[0]=='q'&&usr.data[1]=='\0')
						{
								printf("退出了私聊\n");
								usr.cmd=CFO_CEL;
								write(sockfd,&usr,sizeof(usr));
								return;
						}
						//如果已经被管理员踢出聊天室，自动退出聊天
						if(countchat==1)
						{
								usr.cmd=CFO_CEL;
								write(sockfd,&usr,sizeof(usr));
								return;
						}
						write(sockfd,&usr,sizeof(usr));
						if(read(sockfd,&ser,sizeof(ser))<=0)
						{
								printf("服务器未响应,请稍后再试\n");
								return;
						}
						if(ser.sig==CFO_ERR)
						{
								printf("发送失败,对方可能已离线\n");
						}
						else if(ser.sig==CFO_OK)
						{
								time_t ti;
								ti=time(NULL);
								struct tm * t;
								t=localtime(&ti);
								char chattime[MAX_TIME];
								char cmd[MAX_CMD];
								char * errmsg;
								sprintf(chattime,"%d-%02d-%02d %02d:%02d:%02d",t->tm_year+1900,t->tm_mon+1,t->tm_mday,t->tm_hour,t->tm_min,t->tm_sec);
								sprintf(cmd,"insert into chathistory values('%s','%s','%s','%s')",usr.name,dstname,usr.data,chattime);
								if(sqlite3_exec(db,cmd,NULL,NULL,&errmsg)!=SQLITE_OK)
								{
										printf("添加私聊记录 操作失败\n");
										printf("%s\n",errmsg);
								}
						}
				}
		}
}

void History_chat(void)
{
		char cmd[MAX_CMD];
		int choose;	
		char ** result;
		char * errmsg;
		int nrow;
		int ncolumn;
		int index;
		int i,j;
		while(1)
		{	
				Chat_history_choose();
				scanf("%d",&choose);
				getchar();
				Chat_history_choose();
				switch(choose)
				{
						case 1: strcpy(cmd,"select * from chathistory where dstname='broadcast'");
								break;
						case 2: strcpy(cmd,"select * from chathistory where dstname<>'broadcast'");
								break;
						case 3: strcpy(cmd,"select * from chathistory");
								break;
						case 0: return;
						default:printf("请输入有效数字\n");
								continue;
				}
				if(sqlite3_get_table(db,cmd,&result,&nrow,&ncolumn,&errmsg)!=SQLITE_OK)
				{
						printf("查询数据库时出错\n");
						return;
				}
				if(nrow==0)
				{
						printf("未查询到相关记录\n");
						printf("请按回车键继续\n");
						getchar();
						continue;
				}
				index=0;
				for(i=0;i<=nrow;i++)
				{
						for(j=0;j<ncolumn;j++)
						{
								printf("%s\t",result[index++]);
						}
						printf("\n");
				}
				printf("请按回车键继续\n");
				getchar();
		}
}

void Delete_chat(void)
{
		char * errmsg;
		printf("确定要清空本地聊天记录?(y/n)");
		if(getchar()=='y')
		{
				getchar();
				char cmd[MAX_CMD]="drop table chathistory";
				if(sqlite3_exec(db,cmd,NULL,NULL,&errmsg)!=SQLITE_OK)
				{
						printf("数据库错误\n");
						printf("%s\n",errmsg);
						return;
				}
				strcpy(cmd,"create table chathistory(srcname text not null,dstname text not null,string text,chattime text)");
				if(sqlite3_exec(db,cmd,NULL,NULL,&errmsg)!=SQLITE_OK)
				{
						printf("数据库错误\n");
						printf("%s\n",errmsg);
						return;
				}
				printf("清空完成\n");
		}		
		else
		{
				printf("取消了本次操作\n");
				return;
		}
}

void Syn_chat(int sockfd)
{
		USR usr;
		SER ser;
		usr.cmd=CHR;
		char cmd[MAX_CMD];
		char srcname[MAX_NAME]={0};
		char dstname[MAX_NAME]={0};
		char string[MAX_DATA]={0};
		char chattime[MAX_TIME]={0};
		char * errmsg;
		strcpy(usr.name,name);
		write(sockfd,&usr,sizeof(usr));
		strcpy(cmd,"delete from chathistory");
		if(sqlite3_exec(db,cmd,NULL,NULL,&errmsg)!=SQLITE_OK)
		{
				printf("数据库错误\n");
				printf("%s\n",errmsg);
				return;
		}
		if(read(sockfd,&ser,sizeof(ser))<=0)
		{
				printf("服务器未响应,请稍后再试\n");
				return;
		}
		if(ser.sig!=GET_CHR)
		{
				printf("与服务器同步发生错误,请稍后再试\n");
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
		if(ser.sig==CHR_ERR)
		{
				printf("未查询到相关记录\n");
				return;
		}
		if(ser.sig==CHR_OK)
		{
				printf("正在接受数据...\n");
				while(1)
				{
						if(read(sockfd,&ser,sizeof(ser))<=0)
						{
								printf("服务器未响应,请稍后再试\n");
								return;
						}
						printf("test1\n");
						if(ser.sig==CHR_END)
						{
								printf("数据接收完毕\n");
								break;
						}
						printf("test2\n");
						strncpy(srcname,ser.data,strlen(ser.data));
						printf("test3\n");
						if(read(sockfd,&ser,sizeof(ser))<=0)
						{
								printf("服务器未响应,请稍后再试\n");
								return;
						}
						printf("test4\n");
						strncpy(dstname,ser.data,strlen(ser.data));
						printf("test5\n");
						if(read(sockfd,&ser,sizeof(ser))<=0)
						{
								printf("服务器未响应,请稍后再试\n");
								return;
						}
						printf("test6\n");
						strncpy(string,ser.data,strlen(ser.data));
						printf("test7\n");
						if(read(sockfd,&ser,sizeof(ser))<=0)
						{
								printf("服务器未响应,请稍后再试\n");
								return;
						}
						printf("test8\n");
						strncpy(chattime,ser.data,strlen(ser.data));
						printf("test9\n");
						sprintf(cmd,"insert into chathistory values('%s','%s','%s','%s')",srcname,dstname,string,chattime);
						if(sqlite3_exec(db,cmd,NULL,NULL,&errmsg)!=SQLITE_OK)
						{
								printf("数据库错误\n");
								printf("%s\n",errmsg);
								return;
						}
				}
				return;
		}
}

void Report_chat(int sockfd)
{
		USR usr;
		SER ser;
		usr.cmd=CRP;
		strcpy(usr.name,name);
		write(sockfd,&usr,sizeof(usr));
		if(read(sockfd,&ser,sizeof(ser))<=0)
		{
				printf("服务器未响应,请稍后再试\n");
				return;
		}
		if(ser.sig!=GET_CRP)
		{
				printf("与服务器同步发生错误,请稍后再试\n");
				sync_err_flag=1;
				return;
		}
		printf("输入要举报的用户名(输入q返回)\n");
		fgets(usr.data,MAX_NAME,stdin);
		usr.data[strlen(usr.data)-1]='\0';
		if(usr.data[0]=='q'&&usr.data[1]=='\0')
		{
				printf("取消了本次操作\n");
				usr.cmd=CRP_CEL;
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
		if(ser.sig==CRP_ERR)
		{
				printf("无效用户名\n");
		}
		if(ser.sig==CRP_DATA)
		{
				printf("请输入举报理由(输入q返回)\n");
				fgets(usr.data,MAX_NAME,stdin);
				usr.data[strlen(usr.data)-1]='\0';
				if(usr.data[0]=='q'&&usr.data[1]=='\0')
				{
						printf("取消了本次操作\n");
						usr.cmd=CRP_CEL;
						write(sockfd,&usr,sizeof(usr));
						return;
				}
				write(sockfd,&usr,sizeof(usr));
				if(read(sockfd,&ser,sizeof(ser))<=0)
				{
						printf("服务器未响应,请稍后再试\n");
						return;
				}
				if(ser.sig==CRP_OK)
				{
						printf("服务器已接收到举报信息,请耐心等待管理员处理\n");
						return;
				}
		}
}

void Exit_chat(int sockfd)
{
		USR usr;
		SER ser;
		usr.cmd=ECH;
		strcpy(usr.name,name);
		write(sockfd,&usr,sizeof(usr));
		if(read(sockfd,&ser,sizeof(ser))<=0)
		{
				printf("服务器未响应,请稍后再试\n");
				return;
		}
		if(ser.sig!=GET_ECH)
		{
				printf("与服务器同步发生错误,请稍后再试\n");
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
				printf("服务器异常,退出聊天室失败\n");
				return;
		}
		else if(ser.sig==ECH_OK)
		{
				printf("退出聊天室成功\n");
				chatflag=0;
		}
}

void * Flash_countchat(void * arg)
{
		pthread_detach(pthread_self());
		while(countchat==1)
		{
				sleep(10);
				kill(getpid(),SIGUSR2);
		}
}

void Chat_func(int sockfd)
{
		int choose;
		int retu;
		int c;
		while(1)
		{
				retu=0;
				Chat_enter();
				scanf("%d",&choose);
				getchar();
				switch(choose)
				{
						//若选择进入聊天室，则判断是否已进入
						case 1:	if(chatflag==0)
								{
										//若被举报，倒计时300秒
										if(countchat==1)
										{
												pthread_t tflash;
												//创建倒计时器,每秒发送信号给主进程,从getchar阻塞返回并刷新等待时间
												pthread_create(&tflash,NULL,Flash_countchat,NULL);
												//循环打印剩余时间
												while(1)
												{
														system("clear");
														if(countchat==1)
														{
																printf("您已被举报,%d秒内无法再次进入聊天室\n",countchat_second);
																printf("请按回车键返回\n");
																//若getchar没有超时，返回
																if((c=getchar())==-1)
																		continue;
																else if(c!=10)
																		getchar();
																return;
														}
														else
														{
																//循环判断，若时间结束，则可再次进入聊天室
																printf("时间结束,是否进入聊天室？(y/n)\n");
																c=getchar();
																if(c!=10)
																		getchar();
																if(c=='y')
																{
																		free_flag=1;
																		pthread_mutex_lock(&free_lock);
																		if(Join_chat(sockfd)<0)
																		{
																				pthread_mutex_unlock(&free_lock);
																				return;
																		}
																		pthread_mutex_lock(&free_lock);
																}
																else if(c=='n')
																		return;
																else
																{
																		printf("输入了y/n以外的字符,默认返回\n");	
																		break;
																}								
																break;
														}
												}
										}
										else 
										{
												free_flag=1;
												pthread_mutex_lock(&free_lock);
												if(Join_chat(sockfd)<0)
												{
														pthread_mutex_lock(&free_lock);
														return;
												}
												pthread_mutex_unlock(&free_lock);
										}
								}
								while(1)
								{
										retu=0;
										//如果之前收到私聊，则可以直接回复，无需输入用户名
										if(call[0]!='\0')
										{
												printf("%s私聊了您,需要立即回复吗？(y/n)\n",call);
												c=getchar();
												if(c=='y')
												{
														getchar();
														Print_chat_head();
														Private_chat(sockfd,call);
												}
												memset(call,0,MAX_NAME);
										}
										Chat_menu();
										free_flag=0;
										scanf("%d",&choose);
										getchar();
										//如果已经被管理员踢出聊天室，自动退出聊天
										if(countchat==1)
										{
												free_flag=1;
												pthread_mutex_lock(&free_lock);
												Exit_chat(sockfd);
												pthread_mutex_unlock(&free_lock);
												return;
										}
										switch(choose)
										{
												//查看聊天室用户表
												case 1: free_flag=1;
														pthread_mutex_lock(&free_lock);
														Onlinelist_chat(sockfd);
														pthread_mutex_unlock(&free_lock);
														break;
														//公聊
												case 2: Print_chat_head();
														free_flag=1;
														pthread_mutex_lock(&free_lock);
														Group_chat(sockfd);
														pthread_mutex_unlock(&free_lock);
														//如果已经被管理员踢出聊天室，自动退出聊天
														if(countchat==1)
														{
																free_flag=1;
																pthread_mutex_lock(&free_lock);
																Exit_chat(sockfd);
																pthread_mutex_unlock(&free_lock);
																return;
														}
														break;
														//私聊
												case 3: Print_chat_head();
														free_flag=1;
														pthread_mutex_lock(&free_lock);
														Private_chat(sockfd,NULL);
														pthread_mutex_unlock(&free_lock);
														//如果已经被管理员踢出聊天室，自动退出聊天
														if(countchat==1)
														{
																free_flag=1;
																pthread_mutex_lock(&free_lock);
																Exit_chat(sockfd);
																pthread_mutex_unlock(&free_lock);
																return;
														}
														break;
														//查看聊天记录(本地)
												case 4: History_chat();
														break;
														//删除聊天记录(本地)
												case 5: Delete_chat();
														break;
														//同步聊天记录(服务器)
												case 6: free_flag=1;
														pthread_mutex_lock(&free_lock);
														Syn_chat(sockfd);
														pthread_mutex_unlock(&free_lock);
														break;
														//举报
												case 7: free_flag=1;
														pthread_mutex_lock(&free_lock);
														Report_chat(sockfd);
														pthread_mutex_unlock(&free_lock);
														break;
														//仍能接受消息的退出
												case 8: retu=1;
														break;
														//退出聊天室
												case 0: free_flag=1;
														pthread_mutex_lock(&free_lock);
														Exit_chat(sockfd);
														pthread_mutex_unlock(&free_lock);
														return;
												default:printf("请输入有效数字\n");
														break;
										}
										if(sync_err_flag==1)
												return;
										free_flag=0;
										printf("请按回车继续\n");
										getchar();
								}
								break;
						case 2: if(chatflag==1)
								{
										free_flag=1;
										pthread_mutex_lock(&free_lock);
										Exit_chat(sockfd);
										pthread_mutex_unlock(&free_lock);
								}
								else
								{
										printf("还未登录聊天室\n默认返回\n");
										return;
								}
						case 0: return;
						default:printf("请输入有效数字\n");
								break;
				}
				free_flag=0;
				printf("请按回车继续\n");
				getchar();	
		}
}
