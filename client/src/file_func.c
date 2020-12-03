#include"client_head.h"
extern char name[MAX_NAME];
extern int sync_err_flag;
extern char doc_path[MAX_PATH];
extern int free_flag;
extern pthread_mutex_t free_lock;
extern sqlite3 * db;
extern int work_queue_state;
extern int work_queue_for_load;

void View_doc(int sockfd,int mode)
{
		USR usr;
		SER ser;
		usr.cmd=VDO;
		strcpy(usr.name,name);
		write(sockfd,&usr,sizeof(usr));
		int ret;
		if(read(sockfd,&ser,sizeof(ser))<=0)
		{
				printf("服务器未响应,请稍后再试\n");
				return;
		}
		if(ser.sig!=GET_VDO)
		{
				printf("与服务器同步发生错误,请稍后再试\n");
				sync_err_flag=1;
				return;
		}
		if(mode==0)
				usr.cmd=VDO;
		else if(mode==1)
				usr.cmd=VDO_PRI;
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
		if(ser.sig==VDO_ERR)
		{
				printf("服务器内无文档\n");
				return;
		}
		else if(ser.sig==VDO_OK)
		{
				printf("%s\n",ser.data);
				while((ret=read(sockfd,&ser,sizeof(ser)))>0)
				{
						if(ser.sig=VDO_END)
								break;
				}
				if(ret<=0)
				{
						printf("服务器未响应,请稍后再试\n");
				}
				else 
				{
						printf("文档列表传输完毕\n");
				}
		}
		return;
}

void Search_doc(int sockfd)
{
		USR usr;
		SER ser;
		usr.cmd=SDO;
		int choose;
		int flag=0;
		int ret;
		strcpy(usr.name,name);
		write(sockfd,&usr,sizeof(usr));
		if(read(sockfd,&ser,sizeof(ser))<=0)
		{
				printf("服务器未响应,请稍后再试\n");
				return;
		}
		if(ser.sig!=GET_SDO)
		{
				printf("与服务器同步发生错误,请稍后再试\n");
				sync_err_flag=1;
				return;
		}
		while(1)
		{
				while(1)
				{
						Searchdoc_menu();
						scanf("%d",&choose);
						getchar();
						Searchdoc_menu();
						switch(choose)
						{
								case 1: while(1)
										{
												flag=0;
												Print_searchdoc();
												scanf("%d",&choose);
												getchar();
												Print_searchdoc();
												switch(choose)
												{
														case 1: usr.cmd=SDO_NAME_C;
																break;
														case 2: usr.cmd=SDO_NAME_U;
																break;
														case 3: flag=2;
																break;
														default:printf("请输入有效数字\n");
																flag=1;
																break;
												}
												if(flag==1)
												{
														printf("请按回车键继续\n");
														getchar();
														continue;
												}
												printf("请输入文档名(输入q返回):\n");
												break;
										}
										break;
								case 2: while(1)
										{
												Print_searchdoc();
												scanf("%d",&choose);
												getchar();
												Print_searchdoc();
												switch(choose)
												{
														case 1: usr.cmd=SDO_AUTH_C;
																break;
														case 2: usr.cmd=SDO_AUTH_U;
																break;
														case 3: flag=2;
																break;
														default:printf("请输入有效数字\n");
																flag=1;
																break;
												}
												if(flag==1)
												{
														printf("请按回车键继续\n");
														getchar();
														continue;
												}
												printf("请输入作者名(输入q返回):\n");
												break;
										}
										break;
								case 3: usr.cmd=SDO_TIME;
										printf("请输入日期(输入q返回)\n");
										break;
								case 0: usr.cmd=SDO_CEL;
										flag=3;
										break;
								default:printf("请输入有效数字\n");
										flag=2;
										break;
						}
						if(flag==2)
								continue;
						if(flag==3)
						{
								write(sockfd,&usr,sizeof(usr));
								return;
						}
						else
								break;
				}
				fgets(usr.data,MAX_DATA,stdin);
				usr.data[strlen(usr.data)-1]='\0';
				if(usr.data[0]=='q'&&usr.data[1]=='\0')
						continue;
				write(sockfd,&usr,sizeof(usr));
				if(read(sockfd,&ser,sizeof(ser))<=0)
				{
						printf("服务器失去响应,请稍后再试\n");
						return;
				}
				if(ser.sig==SER_ERR)
				{
						printf("服务器异常,请稍后再试\n");
						return;
				}
				else if(ser.sig==SDO_ERR)
				{
						printf("对不起,未搜索到相关文档信息\n");
				}
				else if(ser.sig==SDO_OK)
				{
						printf("已搜索到相关文档,正在接收信息\n");
						printf("%s\n",ser.data);
						while(ret=read(sockfd,&ser,sizeof(ser))>0)
						{
								if(ser.sig==SDO_END)
										break;
								printf("%s\n",ser.data);
						}
						if(ret<=0)
						{
								printf("服务器失去响应,请稍后再试\n");
								return;
						}
						printf("文档信息显示完毕\n");
				}
				printf("请按回车键继续\n");
				getchar();
		}
}

void History_doc(int sockfd)
{
		USR usr;
		SER ser;
		usr.cmd=HDO;
		strcpy(usr.name,name);
		int ret;
		write(sockfd,&usr,sizeof(usr));
		if(read(sockfd,&ser,sizeof(ser))<=0)
		{
				printf("服务器未响应,请稍后再试\n");
				return;
		}
		if(ser.sig!=GET_HDO)
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
				printf("服务器异常，请稍后再试\n");
				return;
		}
		else if(ser.sig==HDO_ERR)
		{
				printf("未查询到相关记录\n");
				return;
		}
		else if(ser.sig==HDO_OK)
		{
				printf("已查询到相关信息,正在接收信息\n");
				printf("%s\n",ser.data);
				while(ret=read(sockfd,&ser,sizeof(ser))>0)
				{
						if(ser.sig==HDO_END)
								break;
						printf("%s\n",ser.data);
				}
				if(ret<=0)
				{
						printf("服务器失去响应,请稍后再试\n");
						return;
				}
				printf("文档信息显示完毕\n");
		}
		return;
}

unsigned long Download_doc_task(int sockfd,char * filename,char * downloadname,unsigned long offset,int * times,int mode)
{
		USR usr;
		SER ser;
		char path[MAX_PATH];
		strcpy(path,doc_path);
		strcat(path,downloadname);
		usr.cmd=DDO_TASK;
		int choose;
		int ret;
		strcpy(usr.name,name);
		FILE * fp;
		int flag=0;
		write(sockfd,&usr,sizeof(usr));
		if(read(sockfd,&ser,sizeof(ser))<=0)
		{
				*times++;
				return -1;
		}
		if(ser.sig!=GET_DDO_TASK)
		{
				*times++;
				sync_err_flag=1;
				return;
		}
		if(mode==0)
				usr.cmd=DDO_DDO;
		else if(mode==1)
				usr.cmd=DDO_PRI;
		else	
		{
				*times++;
				return -1;
		}
		strcpy(usr.data,filename);
		write(sockfd,&usr,sizeof(usr));
		if(read(sockfd,&ser,sizeof(ser))<=0)
		{
				*times++;
				return -1;
		}
		if(ser.sig!=DDO_OK)
		{
				*times++;
				return -1;
		}
		if(*times==MAX_RETRY_TIMES)  //超过最大尝试次数，取消上传
		{
				*times++;
				usr.cmd=DDO_CEL;
				write(sockfd,&usr,sizeof(usr));
				return -1;
		}
		fp=fopen(path,"a+");
		if(fp==NULL)  
		{
				usr.cmd=DDO_CEL;
				write(sockfd,&usr,sizeof(usr));
				return -1;
		}
		usr.cmd=DDO_OK;
		write(sockfd,&usr,sizeof(usr));
		usr.cmd=offset;
		write(sockfd,&usr,sizeof(usr));
		while((ret=read(sockfd,&ser,sizeof(ser)))>0)
		{
				if(ser.sig==DDO_END)
						break;
				fwrite(ser.data,1,MAX_DATA,fp);
				offset+=MAX_DATA;
				if(free_flag==1)
				{
						usr.cmd=DDO_CEL;
						write(sockfd,&usr,sizeof(usr));
						return offset;
				}
				usr.cmd=DDO_DDO;
				write(sockfd,&usr,sizeof(usr));
		}
		if(ret<=0)
		{
				*times++;
				return -1;
		}
		read(sockfd,&ser,sizeof(ser));
		fwrite(ser.data,1,ser.sig,fp);
		fclose(fp);
		char cmd[MAX_CMD];
		char * errmsg;
		if(mode==0)
				sprintf(cmd,"insert into dochistory values('%s',0)",downloadname);
		else if(mode==1)
				sprintf(cmd,"insert into dochistory values('%s',1)",downloadname);
		sqlite3_exec(db,cmd,NULL,NULL,&errmsg);
		printf("文件 %s 下载完毕\n",filename);
		return 0;
}

void Download_doc(int sockfd,int mode)
{
		USR usr;
		SER ser;
		usr.cmd=DDO;
		int choose;
		int ret;
		char downloadname[MAX_FILENAME];
		char temp[MAX_FILENAME];
		strcpy(usr.name,name);
		FILE * fp;
		int flag=0;
		write(sockfd,&usr,sizeof(usr));
		if(read(sockfd,&ser,sizeof(ser))<=0)
		{
				printf("服务器未响应,请稍后再试\n");
				return;
		}
		if(ser.sig!=GET_DDO)
		{
				printf("与服务器同步发生错误,请稍后再试\n");
				sync_err_flag=1;
				return;
		}
		while(1)
		{
				if(mode==0)
				{
						Downloaddoc_menu();
						scanf("%d",&choose);
						getchar();
						Downloaddoc_menu();
						switch(choose)
						{
								case 1: flag=1;
										break;
								case 2: usr.cmd=DDO_HIS;
										write(sockfd,&usr,sizeof(usr));
										if(read(sockfd,&ser,sizeof(ser))<=0)
										{
												printf("服务器失去响应,请稍后再试\n");
												return;
										}
										if(ser.sig==SER_ERR)
										{
												printf("服务器异常,请稍后再试\n");
												return;
										}
										else if(ser.sig==DDO_ERR)
										{
												printf("未查询到相关记录\n");
												continue;
										}
										else if(ser.sig==DDO_OK)
										{
												printf("正在接受数据...\n");
												printf("%s\n",ser.data);
												while((ret=read(sockfd,&ser,sizeof(ser)))>0)
												{
														if(ser.sig==DDO_END)
																break;
														printf("%s\n",ser.data);
												}
												if(ret<=0)
												{
														printf("服务器失去响应,请稍候再试\n");
														return;
												}
												else 
														printf("数据接收完毕\n");
												printf("请按回车键继续\n");
												getchar();
										}
										break;
								case 0: usr.cmd=DDO_CEL;
										write(sockfd,&usr,sizeof(usr));
										return;
								default:printf("请输入有效数字\n");
										printf("请按回车键继续\n");
										getchar();
										break;
						}
						if(flag==0)
								continue;
				}
				if(mode==1||flag==1)
				{
						printf("请输入文档名(输入q退出):\n");
						fgets(usr.data,MAX_DATA,stdin);
						usr.data[strlen(usr.data)-1]='\0';
						if(usr.data[0]=='q'&&usr.data[1]=='\0')
								usr.cmd=DDO_CEL;
						else if(mode==0)
								usr.cmd=DDO_DDO;
						else if(mode==1)
								usr.cmd=DDO_PRI;
						write(sockfd,&usr,sizeof(usr));
						if(usr.cmd==DDO_CEL)
								return;
						else 
						{
								if(read(sockfd,&ser,sizeof(ser))<=0)
								{
										printf("服务器失去响应,请稍后再试\n");
										return;
								}
								if(ser.sig==SER_ERR)
								{
										printf("服务器异常,请稍后再试\n");
										return;
								}
								else if(ser.sig==DDO_ERR)
								{
										printf("未找到相关文档\n");
										printf("请按回车键继续\n");
										getchar();
										continue;
								}
								else if(ser.sig==DDO_DOC)
								{
										int c;
										int at_work_flag=0;
										printf("该文件大小为 %s\n",ser.data);
										if(work_queue_state==1)
										{
												if(usr.data[strlen(ser.data)-1]=='M')
												{
														ser.data[strlen(ser.data)-1]='\0';
														if(atoi(ser.data)>=200)
														{
																printf("该文件较大,是否加入工作队列？(y/n)\n(工作队列可在空闲时自动下载)\n");
																c=getchar();
																if(c=='y')
																{
																		getchar();
																		at_work_flag=1;
																}
																else if(c!='n')
																		printf("未知选择,默认取消\n");
														}
												}
										}
										while(1)
										{
												printf("另存为为(输入q取消):\n");
												fgets(temp,MAX_FILENAME,stdin);
												temp[strlen(temp)-1]='\0';
												if(temp[0]=='q'&&temp[1]=='\0')
												{		
														usr.cmd=DDO_CEL;
														write(sockfd,&usr,sizeof(usr));
														break;
												}
												sprintf(downloadname,"%s%s",doc_path,temp);
												if(!access(downloadname,F_OK))
														printf("文件已存在,请重新输入\n");
												else 
												{
														fp=fopen(downloadname,"w+");
														if(fp==NULL)
														{
																perror("create downloadfile failed\n");
																usr.cmd=DDO_CEL;
																write(sockfd,&usr,sizeof(usr));
														}
														break;
												}
										}
										if(usr.cmd==DDO_CEL)
												continue;
										if(at_work_flag==1)
										{
												getchar();
												struct at_work{
														char filename[MAX_FILENAME];
														char downloadname[MAX_FILENAME];
														unsigned long offset;
														int sockfd;
														int mode; //私人空间模式还是公共模式
												}download_at_work;
												download_at_work.mode=mode;
												strcpy(download_at_work.filename,usr.data);
												strcpy(download_at_work.downloadname,temp);
												download_at_work.offset=0;
												download_at_work.sockfd=sockfd;
												if(Set_load_file_at_work(&download_at_work,2))
												{
														printf("添加进工作队列失败,是否立即开始下载？(y/n)");
														c=getchar();
														if(c=='n')
														{
																printf("取消了本次操作\n");
																usr.cmd==DDO_CEL;
																write(sockfd,&usr,sizeof(usr));
																continue;
														}
														else if(c!='y')
														{
																printf("未知选项,默认取消\n");
																usr.cmd==DDO_CEL;
																write(sockfd,&usr,sizeof(usr));
																continue;
														}
												}
												else
												{
														printf("系统即将在某一空闲时刻下载,下载完毕后会发送通知\n");
														usr.cmd=DDO_CEL;
														write(sockfd,&usr,sizeof(usr));
														continue;
												}
										}
										else	
												printf("请耐心等待下载完毕,下载期间请勿操作\n");
										usr.cmd=DDO_DDO;
										write(sockfd,&usr,sizeof(usr));
										printf("正在下载,请稍后...\n");
										while((ret=read(sockfd,&ser,sizeof(ser)))>0)
										{
												if(ser.sig==DDO_END)
														break;
												fwrite(ser.data,1,MAX_DATA,fp);
										}
										if(ret<=0)
										{
												printf("服务器失去响应,请稍后再试\n");
												fclose(fp);
												return;
										}
										if(read(sockfd,&ser,sizeof(ser))<=0)
										{
												printf("服务器失去响应,请稍后再试\n");
												fclose(fp);
												return;
										}
										fwrite(ser.data,1,ser.sig,fp);
										fclose(fp);
										printf("下载完毕\n");
										char cmd[MAX_CMD];
										char * errmsg;
										if(mode==0)
												sprintf(cmd,"insert into dochistory values('%s',0)",downloadname);
										else if(mode==1)
												sprintf(cmd,"insert into dochistory values('%s',1)",downloadname);
										if(sqlite3_exec(db,cmd,NULL,NULL,&errmsg)!=SQLITE_OK)
										{
												printf("加入下载文档表失败\n");
												printf("%s\n",errmsg);
										}
										printf("请按回车键继续\n");
										getchar();
								}
						}
				}
		}
}

unsigned long Get_file_size(char * name,char * unit) //址传递返回计量单位K/M/B
{
		struct stat p;
		lstat(name,&p);
		if(p.st_size<1048576&&p.st_size>=1024)
		{
				*unit='K';
				return p.st_size/1024;
		}
		if(p.st_size>1048576)
		{
				*unit='M';
				return p.st_size/1048576;
		}
		if(p.st_size<1024)
		{
				*unit='B';
				return p.st_size;
		}
		return -1;
}

unsigned long Upload_doc_task(int sockfd,char * filename,unsigned long offset,int * times,int mode)
{
		USR usr;
		SER ser;
		usr.cmd=UDO_TASK;
		char path[MAX_PATH];
		strcpy(path,doc_path);
		strcat(path,filename);
		write(sockfd,&usr,sizeof(usr));
		if(read(sockfd,&ser,sizeof(ser)<=0))
		{
				*times++;
				return -1;
		}
		if(ser.sig!=GET_UDO_TASK)
		{
				sync_err_flag=1;
				*times++;
				return -1;
		}
		if(mode==0)
				usr.cmd=UDO_UDO;
		else if(mode==1)
				usr.cmd=UDO_PRI;
		else	
		{
				*times++;
				return -1;
		}
		strcpy(usr.name,name);
		strcpy(usr.data,filename);
		write(sockfd,&usr,sizeof(usr));
		if(read(sockfd,&ser,sizeof(ser)<=0))
		{
				*times++;
				return -1;
		}
		if(ser.sig!=UDO_OK)
		{
				*times++;
				return -1;
		}
		if(*times==MAX_RETRY_TIMES)  //超过最大尝试次数，取消上传
		{
				*times++;
				usr.cmd=UDO_CEL;
				write(sockfd,&usr,sizeof(usr));
				return -1;
		}
		FILE * fp;
		fp=fopen(path,"r");
		if(fp==NULL)  //打不开说明文件已不存在，再次尝试也已没有意义，直接取消
		{
				usr.cmd=UDO_CEL;
				write(sockfd,&usr,sizeof(usr));
				return -1;
		}
		usr.cmd=UDO_OK;
		write(sockfd,&usr,sizeof(usr));
		fseek(fp,offset,SEEK_SET);
		usr.cmd=UDO_DOC;
		int ret;
		while((ret=fread(usr.data,1,MAX_DATA,fp))>0)
		{
				if(ret<MAX_DATA)
						break;
				write(sockfd,&usr,sizeof(usr));
				offset+=MAX_DATA;
				if(free_flag==1)
				{
						usr.cmd=UDO_CEL;
						write(sockfd,&usr,sizeof(usr));
						return offset;
				}
				memset(usr.data,0,MAX_DATA);
		}
		if(ret<0)
		{
				*times++;
				return -1;
		}
		usr.cmd=UDO_END;
		write(sockfd,&usr,sizeof(usr));
		usr.cmd=ret;
		write(sockfd,&usr,sizeof(usr));
		char unit;
		usr.cmd=Get_file_size(path,&unit);
		usr.data[0]=unit;
		write(sockfd,&usr,sizeof(usr));
		fclose(fp);
		read(sockfd,&ser,sizeof(ser));
		printf("文件 %s 上传完毕\n",filename);
		return 0;
}

void Upload_doc(int sockfd,int mode)
{
		USR usr;
		SER ser;
		DIR * di;
		struct dirent * d;
		usr.cmd=UDO;
		int choose;
		int ret;
		char uploadname[MAX_FILENAME];
		char temp[MAX_PATH];
		char filename[MAX_FILENAME];
		strcpy(usr.name,name);
		FILE * fp;
		int flag=0;
		unsigned long size=0;
		char unit=0;
		write(sockfd,&usr,sizeof(usr));
		if(read(sockfd,&ser,sizeof(ser))<=0)
		{
				printf("服务器未响应,请稍后再试\n");
				return;
		}
		if(ser.sig!=GET_UDO)
		{
				printf("与服务器同步发生错误,请稍后再试\n");
				sync_err_flag=1;
				return;
		}
		while(1)
		{
				flag=0;
				if(mode==0)
				{
						Uploaddoc_menu();
						scanf("%d",&choose);
						getchar();
						Uploaddoc_menu();
						switch(choose)
						{
								case 1: flag=1;
										break;
								case 2: usr.cmd=UDO_HIS;
										write(sockfd,&usr,sizeof(usr));
										if(read(sockfd,&ser,sizeof(ser))<=0)
										{
												printf("服务器失去响应,请稍后再试\n");
												return;
										}
										if(ser.sig==SER_ERR)
										{
												printf("服务器异常,请稍后再试\n");
												return;
										}
										else if(ser.sig==UDO_ERR)
										{
												printf("未查询到相关记录\n");
												continue;
										}
										else if(ser.sig==UDO_OK)
										{
												printf("正在接受数据...\n");
												printf("%s\n",ser.data);
												while((ret=read(sockfd,&ser,sizeof(ser)))>0)
												{
														if(ser.sig==UDO_END)
																break;
														printf("%s\n",ser.data);
												}
												if(ret<=0)
												{
														printf("服务器失去响应,请稍候再试\n");
														return;
												}
												else 
														printf("数据接收完毕\n");
												printf("请按回车键继续\n");
												getchar();
										}
										break;
								case 0: usr.cmd=UDO_CEL;
										write(sockfd,&usr,sizeof(usr));
										return;
								default:printf("请输入有效数字\n");
										printf("请按回车键继续\n");
										getchar();
										break;
						}
						if(flag==0)
								continue;
				}
				if(mode==1||flag==1)
				{
						strcpy(temp,doc_path);
						di=opendir(temp);
						if(di==NULL)
						{
								printf("打开本地默认文档目录失败\n");
								usr.cmd=UDO_CEL;
								write(sockfd,&usr,sizeof(usr));
								return;
						}
						printf("本地文件一览:\n");
						while(d=readdir(di))
						{
								if(d->d_name[0]=='.')  //忽略隐藏文件
										continue;
								printf("%8s\t",d->d_name);
						}
						printf("\n");
						while(1)
						{
								printf("请输入文档名(输入quit退出):\n");
								fgets(usr.data,MAX_DATA,stdin);
								usr.data[strlen(usr.data)-1]='\0';
								if((strncmp(usr.data,"quit",4)==0)&&usr.data[4]=='\0')
								{
										usr.cmd=UDO_AGA;
										break;
								}
								if(mode==0)
								{
										usr.cmd=UDO_UDO;
								}
								else if(mode==1)
								{
										usr.cmd=UDO_PRI;
								}
								sprintf(temp,"%s/%s",doc_path,usr.data);
								fp=fopen(temp,"r");
								if(fp==NULL)
								{
										printf("文件不存在\n");	
										continue;
								}
								size=Get_file_size(temp,&unit);
								if(size==-1)
								{
										printf("对不起,文件过大\n");
										continue;
								}
								else 
										break;
						}
						write(sockfd,&usr,sizeof(usr));
						if(usr.cmd==UDO_AGA)
								continue;
						else 
						{
								strcpy(filename,usr.data);
								if(read(sockfd,&ser,sizeof(ser))<=0)
								{
										printf("服务器失去响应,请稍后再试\n");
										return;
								}
								if(ser.sig==SER_ERR)
								{
										printf("服务器异常,请稍后再试\n");
										return;
								}
								else if(ser.sig==UDO_ERR)
								{
										printf("同名文档已存在\n");
								}
								else if(ser.sig==UDO_OK)
								{
										int c;
										printf("此文件大小为: %ld%c\n",size,unit);
										if(work_queue_state==1)
										{
												if(unit=='M'&&size>100)
												{
														printf("该文件较大,是否加入工作队列？(y/n)\n(工作队列可在空闲时自动上传)\n");
														c=getchar();
														if(c=='y')
														{
																getchar();
																struct at_work{
																		char filename[MAX_FILENAME];
																		unsigned long offset;
																		int sockfd;
																		int mode;
																}upload_at_work;
																upload_at_work.mode=mode;
																strcpy(upload_at_work.filename,filename);
																upload_at_work.offset=0;
																upload_at_work.sockfd=sockfd;
																int id=-1;
																if((id=Set_load_file_at_work(&upload_at_work,1))==-1)
																{
																		printf("添加进工作队列失败,是否立即开始上传？(y/n)");
																		c=getchar();
																		if(c=='n')
																		{
																				printf("取消了本次操作\n");
																				usr.cmd==UDO_CEL;
																				write(sockfd,&usr,sizeof(usr));
																				return;
																		}
																		else if(c!='y')
																		{
																				printf("未知选项,默认取消\n");
																				usr.cmd==UDO_CEL;
																				write(sockfd,&usr,sizeof(usr));
																				return;
																		}
																}
																else
																{
																		usr.cmd==UDO_DO_TASK;
																		write(sockfd,&usr,sizeof(usr));
																		if(read(sockfd,&ser,sizeof(ser))<=0)
																		{
																				printf("服务器失去响应,请稍后再试\n");
																				return;
																		}
																		if(ser.sig==UDO_ERR)
																		{
																				printf("服务器异常,上传取消\n");
																				Destroy_task(NULL,NULL,id);
																				return;
																		}
																		printf("系统即将在某一空闲时刻上传,上传完毕后会发送通知\n");
																		return;
																}
														}
														else
																printf("请耐心等待上传完毕,上传期间请勿操作\n");
												}
										}
										printf("正在上传文档...\n");
										usr.cmd=UDO_DOC;
										write(sockfd,&usr,sizeof(usr));
										while((ret=fread(usr.data,1,MAX_DATA,fp))>0)
										{
												if(ret<MAX_DATA)
														break;
												write(sockfd,&usr,sizeof(usr));
												memset(usr.data,0,MAX_DATA);
										}
										usr.cmd=UDO_END;
										write(sockfd,&usr,sizeof(usr));
										usr.cmd=ret;
										write(sockfd,&usr,sizeof(usr));
										usr.cmd=size;
										usr.data[0]=unit;
										write(sockfd,&usr,sizeof(usr));
										fclose(fp);
										if(read(sockfd,&ser,sizeof(ser))<=0)
										{
												printf("服务器失去响应,请稍后再试\n");
												return;
										}
										if(ser.sig==UDO_ERR)
										{
												printf("服务器异常,上传失败\n");
												return;
										}
										printf("上传完毕\n");
								}
								printf("请按回车键继续\n");
								getchar();
						}
				}
		}
}

void Delete_doc(void)
{
		char ** result;
		char * errmsg;
		int nrow;
		int ncolumn;
		int index;
		int i,j;
		char docname[MAX_FILENAME];
		char cmd[MAX_CMD]="select * from dochistory";
		int ret;
		if(sqlite3_get_table(db,cmd,&result,&nrow,&ncolumn,&errmsg)!=SQLITE_OK)
		{
				printf("读取下载记录时出错\n");
				printf("%s\n",errmsg);
				return;
		}
		if(nrow==0)
		{
				printf("未下载文档\n");
				return;
		}
		index=ncolumn;
		for(i=0;i<nrow;i++)
		{
				for(j=0;j<ncolumn;j++)
				{
						printf("%s: %s\n",result[j],result[index++]);
				}
				printf("----------------------\n");
		}
		printf("记录显示完毕\n");
		printf("请输入想要删除的文档名(输入q取消):\n");
		fgets(docname,MAX_FILENAME,stdin);
		docname[strlen(docname)-1]='\0';
		if(docname[0]=='q'&&docname[1]=='\0')
		{
				printf("取消了本次操作\n");
				return;
		}
		char docpathname[MAX_PATH];
		sprintf(docpathname,"%s%s",doc_path,docname);
		sprintf(cmd,"delect from dochistory where name='%s'",docpathname);
		if(sqlite3_exec(db,cmd,NULL,NULL,&errmsg)!=SQLITE_OK)
		{
				printf("%s\n",errmsg);
				printf("删除文档失败\n");
				return;
		}
		remove(docpathname);
		printf("删除文档成功\n");
		return;
}

void Cancel_doc(int sockfd)
{
		USR usr;
		SER ser;
		usr.cmd=CDO;
		strcpy(usr.name,name);
		int ret;
		write(sockfd,&usr,sizeof(usr));
		if(read(sockfd,&ser,sizeof(ser))<=0)
		{
				printf("服务器失去响应,请稍后再试\n");
				return;
		}
		if(ser.sig!=GET_CDO)
		{
				printf("与服务器同步发生错误,请稍后再试\n");
				sync_err_flag=1;
				return;
		}
		write(sockfd,&usr,sizeof(usr));
		printf("正在获取分享列表...\n");
		if(read(sockfd,&ser,sizeof(ser))<=0)
		{
				printf("服务器失去响应,请稍后再试\n");
				return;
		}
		if(ser.sig==SER_ERR)
		{
				printf("服务器异常,请稍候再试\n");
				return;
		}
		if(ser.sig==CDO_ERR)
		{
				printf("未上传任何文档\n");
				return;
		}
		if(ser.sig==CDO_LIST)
		{
				printf("%s\n",ser.data);
				while(ret=read(sockfd,&ser,sizeof(ser)))
				{
						if(ser.sig==CDO_END)
								break;
						printf("%s\n",ser.data);
				}
				if(ret==0)
				{
						printf("服务器失去响应,请稍后再试\n");
						return;
				}
				printf("列表接受完毕\n");
				while(1)
				{
						printf("请输入想要取消分享的文档名(输入q取消):\n");
						fgets(usr.data,MAX_FILENAME,stdin);
						usr.data[strlen(usr.data)-1]=='\0';
						if(usr.data[0]=='q'&&usr.data[1]=='\0')
						{
								printf("取消了本次操作\n");
								usr.cmd=CDO_CEL;
								write(sockfd,&usr,sizeof(usr));
								return;
						}
						write(sockfd,&usr,sizeof(usr));
						if(read(sockfd,&ser,sizeof(ser))<=0)
						{
								printf("服务器失去响应,请稍后再试\n");
								return;
						}
						if(ser.sig==CDO_ERR)
						{
								printf("文档不存在,请检查是否输错\n");
								continue;
						}
						if(ser.sig==SER_ERR)
						{
								printf("服务器异常,请稍后再试\n");
								return;
						}
						if(ser.sig==CDO_OK)
						{
								printf("取消分享成功\n");
						}
				}
		}
}

void Change_path(void)
{
		printf("current docpath=%s\n",doc_path);
		if(work_queue_for_load)
				printf("当前有%d个下载/上传任务正在进行,请等待完成后再修改\n",work_queue_for_load);
		FILE * fp1,* fp2;
		char buf[128]={0};
		char temp[128]="docpath=";
		char doc_path_temp[MAX_PATH]={0};
		printf("请输入新下载路径(绝对路径)(输入q取消):\n");
		fgets(doc_path_temp,MAX_PATH,stdin);
		doc_path_temp[strlen(doc_path_temp)-1]='\0';
		if(doc_path_temp[0]=='q'&&doc_path_temp[1]=='\0')
		{
				printf("取消了本次操作\n");
				return;
		}
		struct stat st;
		lstat(doc_path_temp,&st);
		if((st.st_mode&S_IFMT)!=S_IFDIR)
		{
				printf("error:不是一个(合法)目录\n");
				return;
		}
		strcpy(doc_path,doc_path_temp);
		if(doc_path[strlen(doc_path)-1]!='/')
				strcat(doc_path,"/");
		fp1=fopen("./client_config","r+");
		if(fp1==NULL)
		{
				perror("配置文件打开失败\n");
				printf("文档默认目录修改失败\n");
				return;
		}
		fp2=fopen("./config_temp","w");
		if(fp2==NULL)
		{
				perror("配置文件temp打开失败\n");
				printf("文档默认目录修改失败\n");
				return;
		}
		while(fgets(buf,128,fp1))
		{
				if(strncmp(buf,"docpath=",8)==0)
				{
						strcat(temp,doc_path);
						temp[strlen(temp)]='\n';
						temp[strlen(temp)]='\0';
						fputs(temp,fp2);
				}
				else
						fputs(buf,fp2);
		}
		remove("./client_config");
		rename("config_temp","client_config");
		printf("已更新配置文件\n");
		fclose(fp1);
		fclose(fp2);
		printf("修改文档默认目录成功\n");
		return;
}

void File_func(int sockfd)
{
		int choose;
		int retu=0;
		while(1)
		{
				File_menu();
				scanf("%d",&choose);
				getchar();
				free_flag=1;
				File_menu();
				switch(choose)
				{
						//文档一览
						case 1: free_flag=1;
								pthread_mutex_lock(&free_lock);
								View_doc(sockfd,0);
								pthread_mutex_unlock(&free_lock);
								break;
								//查找
						case 2: free_flag=1;
								pthread_mutex_lock(&free_lock);
								Search_doc(sockfd);
								pthread_mutex_unlock(&free_lock);
								break;
								//查找的历史记录
						case 3: free_flag=1;
								pthread_mutex_lock(&free_lock);
								History_doc(sockfd);
								pthread_mutex_unlock(&free_lock);
								break;
								//下载和下载记录
						case 4: free_flag=1;
								pthread_mutex_lock(&free_lock);
								Download_doc(sockfd,0);
								pthread_mutex_unlock(&free_lock);
								break;
								//上传和上传记录
						case 5: free_flag=1;
								pthread_mutex_lock(&free_lock);
								Upload_doc(sockfd,0);
								pthread_mutex_unlock(&free_lock);
								break;
								//删除已下载文档
						case 6: Delete_doc();
								break;
								//取消分享
						case 7: free_flag=1;
								pthread_mutex_lock(&free_lock);
								Cancel_doc(sockfd);
								pthread_mutex_unlock(&free_lock);
								break;
								//改文档下载目录
						case 8: Change_path();
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
