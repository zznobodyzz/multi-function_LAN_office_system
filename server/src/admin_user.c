#include"server_head.h"

extern sqlite3 * db;
extern int udpfd;
extern char highestpass[20];


int Get_table(char * cmd)
{
		char ** result;
		char * errmsg;
		int nrow;
		int ncolumn;
		int index;
		int i,j;
		if(sqlite3_get_table(db,cmd,&result,&nrow,&ncolumn,&errmsg)!=SQLITE_OK)
		{
				printf("打印用户列表失败\n");
				printf("%s\n",errmsg);
				return;
		}
		if(nrow==0)
				return nrow;
		index=ncolumn;
		for(i=0;i<nrow;i++)
		{
				for(j=0;j<ncolumn;j++)
						printf("%s: %s\n",result[j],result[index++]);
				printf("---------------------\n");
		}
		return nrow;
}

void Print_online_client(int ch)
{
		char cmd[MAX_CMD];
		int flag=0;
		int choose;
		int ret;
		if(ch==1)//整体用户列表
		{
				while(1)
				{
						Print_client_next();
						scanf("%d",&choose);
						getchar();
						Print_client_next();
						switch(choose)
						{
								case 1:	while(1)
										{
												Print_client_next_next();
												scanf("%d",&choose);
												getchar();
												Print_client_next_next();
												switch(choose)
												{
														case 1: strcpy(cmd,"select name from client");
																ret=Get_table(cmd);
																printf("共有%d位已注册用户\n",ret);
																break;
														case 2: strcpy(cmd,"select * from client");
																ret=Get_table(cmd);
																printf("共有%d位已注册用户\n",ret);
																break;
														case 0: flag=1;
																break;
														default:printf("请输入有效数字\n");
																break;
												}
												if(flag==1)
												{
														flag=0;
														break;
												}
												printf("请按回车继续\n");
												getchar();
										}
										break;
								case 2:	while(1)
										{
												Print_client_next_next();
												scanf("%d",&choose);
												getchar();
												Print_client_next_next();
												switch(choose)
												{
														case 1: strcpy(cmd,"select name from client where stat=1");
																ret=Get_table(cmd);
																printf("共有%d位在线用户\n",ret);
																break;
														case 2: strcpy(cmd,"select * from client where stat=1");
																ret=Get_table(cmd);
																printf("共有%d位在线用户\n",ret);
																break;
														case 0: flag=1;
																break;
														default:printf("请输入有效数字\n");
																break;
												}
												if(flag==1)
												{
														flag=0;
														break;
												}
												printf("请按回车继续\n");
												getchar();
										}
										break;
								case 0: flag=1;
										break;
								default:printf("请输入有效数字\n");
										break;
						}
						if(flag==1)
								return;
				}
		}//聊天室用户列表
		else if(ch==2)
		{
				Print_client_next_next();
				scanf("%d",&choose);
				getchar();
				Print_client_next_next();
				switch(choose)
				{
						case 1: strcpy(cmd,"select name from client where chat=1");
								ret=Get_table(cmd);
								printf("聊天室有%d位用户\n",ret);
								break;
						case 2: strcpy(cmd,"select * from client where chat=1");
								ret=Get_table(cmd);
								printf("聊天室有%d位用户\n",ret);
								break;
						case 0: flag=1;
								break;
						default:printf("请输入有效数字\n");
								break;
				}
				if(flag==1)
						return;
		}
}

void Kick_chat(void)
{
		char name[MAX_NAME];
		SER ser;
		char cmd[MAX_CMD];
		int nrow;
		int ncolumn;
		char * errmsg;
		char ** result;
		struct sockaddr_in caddr;
		int len;
		len=sizeof(struct sockaddr_in);
		caddr.sin_port=htons(SERVER_UDP_PORT);
		caddr.sin_family=PF_INET;
		while(1)
		{
				printf("输入用户名:(输入q退出)\n");
				fgets(name,MAX_NAME,stdin);
				name[strlen(name)-1]='\0';
				if(name[0]=='q'&&name[1]=='\0')
				{
						printf("取消了本次操作\n");
						return;
				}
				sprintf(cmd,"select chat,addr from client where name='%s' and stat=1",name);
				if(sqlite3_get_table(db,cmd,&result,&nrow,&ncolumn,&errmsg)!=SQLITE_OK)
				{
						printf("查询用户聊天状态 操作失败\n");
						printf("%s\n",errmsg);
				}
				if(nrow==0)
				{
						printf("无法踢出该用户,用户名不存在或该用户可能已下线\n");
						continue;
				}
				else 
						break;
		}
		if(atoi(result[2])==0)
		{
				printf("该用户尚未登陆聊天室或已登出聊天室\n");
				return;
		}
		sprintf(cmd,"update client set chat=0 where name='%s'",name);
		if(sqlite3_exec(db,cmd,NULL,NULL,&errmsg)!=SQLITE_OK)
		{
				printf("操作失败\n");
				printf("%s\n",errmsg);
				return;
		}	
		printf("已将该用户提出聊天室\n");
		strcpy(ser.data,"/system warning96/ : 你已被服务器管理员踢出聊天室\n");
		ser.sig=SYS_INFO;
		caddr.sin_addr.s_addr=inet_addr(result[3]);
		//向被踢用户发送提醒
		if(sendto(udpfd,&ser,sizeof(ser),0,(struct sockaddr *)&caddr,len)<0)
		{
				printf("向用户发送提醒失败,用户可能已下线\n");
		}
		return;
}

void Change_client_mod(void)
{
		char cmd[MAX_CMD];
		int nrow;
		int ncolumn;
		char * errmsg;
		char ** result;
		struct sockaddr_in caddr;
		caddr.sin_port=htons(SERVER_UDP_PORT);
		caddr.sin_family=PF_INET;
		int len;
		int i;
		int choose;
		int mode=0;
		SER ser;
		ser.sig=SYS_INFO;
		char name[MAX_NAME];
		int flag=0;
		len=sizeof(struct sockaddr_in);
		while(1)
		{
				Print_server();
				printf("输入用户名:(输入q退出)\n");
				fgets(name,32,stdin);
				name[strlen(name)-1]='\0';
				if(strncmp(name,"q",1)==0)
				{
						return;
				}
				sprintf(cmd,"select mode,addr from client where name='%s'",name);
				if(sqlite3_get_table(db,cmd,&result,&nrow,&ncolumn,&errmsg)!=SQLITE_OK)
				{
						printf("操作失败\n");
						printf("%s\n",errmsg);
						continue;
				}
				if(nrow==0)
				{
						printf("无效用户名\n");
						continue;
				}
				else 
						break;
		}
		mode=atoi(result[2]);
		caddr.sin_addr.s_addr=inet_addr(result[3]);
		if(mode==0)
		{
				while(1)
				{
						printf("该用户为普通用户\n");
						printf("1.升级为高级用户\n");
						printf("2.升级为管理员\n");
						printf("0.取消\n");
						scanf("%d",&choose);
						switch(choose)
						{
								case 1: sprintf(cmd,"update client set mode=1 where name='%s'",name);
										flag=1;
										break;
								case 2: sprintf(cmd,"update client set mode=2 where name='%s'",name);
										flag=1;
										break;
								case 0: printf("取消了本次操作\n");
										return;
								default:printf("请输入有效数字\n");
										break;
						}
						if(flag==1)
								break;
						printf("请按回车键继续\n");
						getchar();
				}
		}
		else if(mode==1)
		{
				while(1)
				{
						printf("该用户为高级用户\n");
						printf("1.升级为管理员\n");
						printf("2.降低为普通用户\n");
						printf("0.取消\n");
						scanf("%d",&choose);
						switch(choose)
						{
								case 1: sprintf(cmd,"update client set mode=2 where name='%s'",name);
										flag=1;
										break;
								case 2: sprintf(cmd,"update client set mode=0 where name='%s'",name);
										flag=1;
										break;
								case 0: printf("取消了本次操作\n");
										return;
								default:printf("请输入有效数字\n");
										break;
						}
						if(flag==1)
								break;
						printf("请按回车重新选择\n");
						getchar();
				}
		}
		else if(mode==2)
		{
				while(1)
				{
						printf("该用户为管理员\n");
						printf("1.降低为高级用户\n");
						printf("2.降低为普通用户\n");
						printf("0.取消\n");
						scanf("%d",&choose);
						switch(choose)
						{
								case 1: sprintf(cmd,"update client set mode=1 where name='%s'",name);
										flag=1;
										break;
								case 2: sprintf(cmd,"update client set mode=0 where name='%s'",name);
										flag=1;
										break;
								case 0: printf("取消了本次操作\n");
										return;
								default:printf("请输入有效数字\n");
										break;
						}
						if(flag==1)
								break;
						printf("请按回车重新选择\n");
						getchar();
				}
		}
		else
		{
				printf("权限未知\n");
				printf("操作无法进行\n");
				return;
		}
		caddr.sin_addr.s_addr=inet_addr(result[3]);
		if(sqlite3_exec(db,cmd,NULL,NULL,&errmsg)!=SQLITE_OK)
		{
				printf("操作失败\n");
				printf("%s\n",errmsg);
				return;
		}	
		if(mode==0)
		{
				if(choose==1)
				{
						printf("该用户已升级为高级用户\n");
						strcpy(ser.data,"/system warning97/ : 您已被管理员升级为高级用户\n");
				}
				else if(choose==2)
				{
						printf("该用户已升级为管理员\n");
						strcpy(ser.data,"/system warning97/ : 您已被管理员升级为管理员\n");
				}	
		}
		else if(mode==1)
		{
				if(choose==1)
				{
						printf("该用户已升级为管理员\n");
						strcpy(ser.data,"/system warning97/ : 您已被管理员升级为管理员\n");
				}
				else if(choose==2)
				{
						printf("该用户已降低为普通用户\n");
						strcpy(ser.data,"/system warning97/ : 您已被管理员降低为普通用户\n");
				}
		}
		else if(mode==2)
		{
				if(choose==1)
				{
						printf("该用户已降低为高级用户\n");
						strcpy(ser.data,"/system warning97/ : 您已被管理员降低为高级用户\n");
				}
				else if(choose==2)
				{
						printf("该用户已降低为普通用户\n");
						strcpy(ser.data,"/system warning97/ : 您已被管理员降低为普通用户\n");
				}
		}
		if(sendto(udpfd,&ser,sizeof(ser),0,(struct sockaddr *)&caddr,len)<0)
		{
				printf("发送升级通知 操作失败,用户可能已下线\n");
		}
		return;
}

void View_search_history(void)
{
		int choose;
		char cmd[MAX_CMD];
		char buf[MAX_WORD];
		int nrow;
		int index;
		int i,j;
		int ncolumn;
		char * errmsg;
		char ** result;
		char time1[MAX_TIME];
		char time2[MAX_TIME];
		int flag=0;
		while(1)
		{
				flag=0;
				Print_client_historySS();
				scanf("%d",&choose);
				getchar();
				Print_client_historySS();
				switch(choose)
				{
						case 1: printf("请输入用户名(输入q退出):\n");
								fgets(buf,32,stdin);
								buf[strlen(buf)-1]='\0';
								if(strncmp(buf,"q",1)==0)
								{
										flag=1;
										break;
								}
								sprintf(cmd,"select * from history where name='%s'",buf);
								break;
						case 2: printf("请输入一个单词(输入q退出):");
								fgets(buf,32,stdin);
								buf[strlen(buf)-1]='\0';
								if(strncmp(buf,"q",1)==0)
								{
										flag=1;
										break;
								}
								sprintf(cmd,"select * from history where word='%s'",buf);
								break;
						case 3: printf("时间格式:yyyy-MM-dd hh:mm:ss\n");
								printf("请输入起始时间(输入q退出):");
								fgets(time1,32,stdin);
								time1[strlen(time1)-1]='\0';
								if(strncmp(time1,"q",1)==0)
								{
										flag=1;
										break;
								}
								printf("请输入截至时间(输入q退出):");
								fgets(time2,32,stdin);
								time2[strlen(time2)-1]='\0';
								if(strncmp(time2,"q",1)==0)
								{
										flag=1;
										break;
								}
								sprintf(cmd,"select * from history where dictime between '%s' and '%s'",time1,time2);
								break;
						case 4: strcpy(cmd,"select * from history");
								break;
						case 0: return;
						default:printf("请输入有效数字\n");
								break;
				}
				if(flag==1)
						continue;
				if(sqlite3_get_table(db,cmd,&result,&nrow,&ncolumn,&errmsg)!=SQLITE_OK)
				{
						printf("操作失败\n");
						printf("%s\n",errmsg);
						continue;
				}
				if(nrow==0)
				{
						printf("未查询到有关信息\n");
				}
				else 
				{
						index=ncolumn;
						for(i=0;i<nrow;i++)
						{
								for(j=0;j<ncolumn;j++)
								{
										printf("%s: %s\n",result[j],result[index++]);
								}
								printf("----------------------\n");
						}
				}
				printf("显示完毕,按回车继续\n");
				getchar();
		}
}

void View_history(int ch)
{
		int choose;
		char cmd[MAX_CMD];
		char buf[128];
		char time1[32];
		char time2[32];
		int nrow;
		int index;
		int i,j;
		int ncolumn;
		char * errmsg;
		char ** result;
		int flag=0;
		char passwd[20];
		while(1)
		{
				flag=0;
				if(ch==1)
				{
						printf("查看用户聊天记录需要输入最高权限密码\n");
						printf("请输入最高权限密码:\n");
						fgets(passwd,20,stdin);
						if(strcmp(passwd,highestpass)!=0)
						{
								printf("密码错误\n");
								return;
						}
						else
						{
								printf("密码正确\n");
								printf("请勿泄露用户隐私\n");
								printf("按回车键继续\n");
								getchar();		
						}
				}
				Print_client_history();
				scanf("%d",&choose);
				getchar();
				Print_client_history();
				switch(choose)
				{
						case 1: printf("请输入用户名(输入q退出):\n");
								fgets(buf,32,stdin);
								buf[strlen(buf)-1]='\0';
								if(strncmp(buf,"q",1)==0)
								{
										flag=1;
										break;
								}
								if(ch==1)
										sprintf(cmd,"select chattime,string from chat where name='%s'",buf);
								if(ch==2)
										sprintf(cmd,"select logintime,logouttime from client where name='%s'",buf);
								break;
						case 2: if(ch==2)
								{
										while(1)
										{
												printf("1.查看登录时间段\n");
												printf("2.查看登出时间段\n");
												printf("3.返回\n");
												scanf("%d",&choose);
												getchar();
												Print_server();
												if(choose==0)
												{
														flag=1;
														break;
												}
												if(choose!=1&&choose!=2)
														printf("请输入有效数字\n");
												else 
														break;
										}
								}
								printf("时间格式:yyyy-MM-dd hh:mm:ss\n");
								printf("请输入起始时间(输入q退出):");
								fgets(time1,32,stdin);
								time1[strlen(time1)-1]='\0';
								if(strncmp(time1,"q",1)==0)
								{
										flag=1;
										break;
								}
								printf("请输入截至时间(输入q退出):");
								fgets(time2,32,stdin);
								time2[strlen(time2)-1]='\0';
								if(strncmp(time2,"q",1)==0)
								{
										flag=1;
										break;
								}
								if(ch==2)
								{
										switch(choose)
										{
												case 1: sprintf(cmd,"select name from client where logintime between '%s' and '%s'",time1,time2);
														break;
												case 2: sprintf(cmd,"select name from client where logouttime between '%s' and '%s'",time1,time2);
														break;
										}
								}
								if(ch==1)
										sprintf(cmd,"select name,string from chat where chattime between '%s' and '%s'",time1,time2);
								break;
						case 3: if(ch==1)
										strcpy(cmd,"select * from chat");
								if(ch==2)
										strcpy(cmd,"select name,logintime,logouttime from client");
								break;
						case 0: return;
						default:printf("请输入有效数字\n");
								break;
				}
				if(flag==1)
						continue;
				if(sqlite3_get_table(db,cmd,&result,&nrow,&ncolumn,&errmsg)!=SQLITE_OK)
				{
						printf("操作失败\n");
						printf("%s\n",errmsg);
						continue;
				}
				if(nrow==0)
				{
						printf("未查询到有关信息\n");
				}
				else 
				{
						index=ncolumn;
						for(i=0;i<nrow;i++)
						{
								for(j=0;j<ncolumn;j++)
								{
										printf("%s: %s\n",result[j],result[index++]);
								}
								printf("----------------------------\n");
						}
				}
				printf("请按回车键继续\n");
				getchar();
		}
}
