#include"server_head.h"

extern sqlite3 * db;
extern char doc_path[MAX_PATH];

void View_document(void)
{
		char cmd[MAX_CMD];
		char * errmsg;
		char ** result;
		int nrow;
		int ncolumn;
		int index;
		int i,j;
		char name[MAX_FILENAME];
		strcpy(cmd,"select name from document");
		if(sqlite3_get_table(db,cmd,&result,&nrow,&ncolumn,&errmsg)!=SQLITE_OK)
		{
				printf("打印文档列表失败\n");
				printf("%s\n",errmsg);
				return;
		}
		if(nrow==0)
		{
				printf("暂未有任何文档\n");
				return;
		}
		index=ncolumn;
		for(i=0;i<nrow;i++)
		{
				for(j=0;j<ncolumn;j++)
						printf("%s: %s\n",result[j],result[index++]);
		}
		printf("请输入想要浏览的文档名(输入q返回)\n");
		fgets(name,MAX_FILENAME,stdin);
		name[strlen(name)-1]='\0';
		if(name[0]=='q')
				return;
		sprintf(cmd,"select * from document where name='%s'",name);
		if(sqlite3_get_table(db,cmd,&result,&nrow,&ncolumn,&errmsg)!=SQLITE_OK)
		{
				printf("打印文档详细信息失败\n");
				printf("%s\n",errmsg);
				return;
		}
		if(nrow==0)
		{
				printf("文档名错误\n");
				return;
		}
		Print_document();
		printf("------------------------------\n");
		index=ncolumn;
		for(i=0;i<nrow;i++)
		{
				for(j=0;j<ncolumn;j++)
				{
						printf("%s: %s\n",result[j],result[index++]);
				}
				printf("------------------------------\n");
		}
		FILE * fp;
		char docname[MAX_FILENAME]={0};
		sprintf(docname,"%s%s",doc_path,name);
		fp=fopen(docname,"r");
		if(fp==NULL)
		{
				perror("打开文档失败\n");
				return;
		}
		char buf[128]={0};
		int ret;
		ret=fread(buf,1,128,fp);
		while(ret!=0)
		{
				fwrite(buf,1,ret,stdout);
				fflush(stdout);
				ret=fread(buf,1,128,fp);
		}
		printf("\n------------------------------\n");
		printf("文档显示完毕\n");
		printf("请按回车键继续\n");
		getchar();
		return;
}

int My_file_remove(char * file)
{
		FILE * fp,* fp_temp;
		char trash_file[MAX_PATH]={0};
		char delfile[MAX_PATH]={0};
		struct tm * t;
		time_t ti;
		char deltime[MAX_TIME]={0};
		int ret;
		char buf[MAX_DATA]={0};
		sprintf(delfile,"%s%s",doc_path,file);
		ti=time(NULL);
		t=localtime(&ti);
		sprintf(deltime,"%d-%02d-%02d-%02d:%02d",t->tm_year+1900,t->tm_mon+1,t->tm_mday,t->tm_hour,t->tm_min);
		sprintf(trash_file,"%strash/%s_%s",doc_path,file,deltime);
		fp=fopen(delfile,"r");
		if(fp==NULL)
		{
				perror("打开文件失败\n");
				return -1;
		}
		fp_temp=fopen(trash_file,"w");
		if(fp_temp==NULL)
		{
				perror("打开文件失败\n");
				return -1;
		}
		while((ret=fread(buf,1,MAX_DATA,fp)==MAX_DATA))
				fwrite(buf,1,MAX_DATA,fp_temp);
		fwrite(buf,1,ret,fp_temp);
		fclose(fp);
		fclose(fp_temp);
		remove(delfile);
		return 0;
}

void Search_document(int ch)
{
		char cmd[MAX_CMD];
		char * errmsg;
		char ** result;
		int nrow;
		int ncolumn;
		int index;
		int i,j;
		int choose;
		FILE * fp;
		int ret;
		char file[MAX_PATH];
		char buf[MAX_DATA];
		strcpy(file,doc_path);
		char name[MAX_FILENAME];
		while(1)
		{
				Print_document_search();
				scanf("%d",&choose);
				getchar();
				Print_document_search();
				switch(choose)
				{
						case 1: while(1)
								{
										Print_search_doc();
										scanf("%d",&choose);
										getchar();
										if(choose==0)
												break;
										else if(choose!=1&&choose!=2)
										{
												printf("请输入有效数字\n");
												printf("请按回车键继续\n");
												getchar();
												continue;
										}
										Print_document_search();
										printf("请输入文档名(输入q返回)\n");
										fgets(name,MAX_FILENAME,stdin);
										name[strlen(name)-1]='\0';
										Print_document_search();
										if(name[0]=='q')
										{
												break;
										}
										switch(choose)
										{
												case 1: sprintf(cmd,"select name,author,atime,dtimes from document where name='%s'",name);
														break;
												case 2: sprintf(cmd,"select name,author,atime,dtimes from document where name like '%%%s%%'",name);
														break;
										}
										if(sqlite3_get_table(db,cmd,&result,&nrow,&ncolumn,&errmsg)!=SQLITE_OK)
										{
												printf("搜索文档失败\n");
												printf("%s\n",errmsg);
												break;
										}
										if(nrow==0)
										{
												printf("未搜索到相关文档\n");
												break;
										}
										index=ncolumn;
										for(i=0;i<nrow;i++)
										{
												for(j=0;j<ncolumn;j++)
												{
														printf("%s: %s\n",result[j],result[index++]);
												}
												printf("------------------------------\n");
										}
										strcat(file,result[4]);
										if(ch==1)
										{
												printf("是否显示该文档内容(y/n)");
												if(getchar()=='y')
												{
														fp=fopen(file,"r");
														if(fp==NULL)
														{
																perror("打开文件失败\n");
																break;
														}
														while((ret=fread(buf,1,MAX_DATA,fp))==MAX_DATA)
																fwrite(buf,1,MAX_DATA,stdout);
														fwrite(buf,1,ret,stdout);
														printf("------------------------------\n");
														printf("文档显示完毕\n");
												}
												else
														printf("取消了操作\n");

										}
										else if(ch==2)
										{
												printf("删除后不可恢复,确定要删除该文档吗(y/n)\n");
												if(getchar()=='y')
												{
														getchar();
														if(My_file_remove(file))
														{
																printf("删除文档失败\n");
																break;
														}
														sprintf(cmd,"delete from document where name='%s'",name);
														if(sqlite3_exec(db,cmd,NULL,NULL,&errmsg)!=SQLITE_OK)
														{
																printf("删除文档失败\n");
																printf("%s\n",errmsg);
																break;
														}
														printf("文档已删除\n");
												}
												else
														printf("取消了删除操作\n");
										}
								}
								printf("请按回车键继续\n");
								getchar();
								break;
						case 2: while(1)
								{
										Print_search_doc();
										scanf("%d",&choose);
										getchar();
										if(choose==0)
												break;
										else if(choose!=1&&choose!=2)
										{
												printf("请输入有效数字\n");
												printf("请按回车键继续\n");
												getchar();
												continue;
										}
										Print_document_search();
										printf("请输入作者名(输入q返回)\n");
										fgets(name,MAX_FILENAME,stdin);
										name[strlen(name)-1]='\0';
										if(name[0]=='q')
										{
												break;
										}
										if(ch==1)
										{
												switch(choose)
												{
														case 1:	sprintf(cmd,"select author,name,atime,dtimes from document where author='%s'",name);
																break;
														case 2: sprintf(cmd,"select author,name,atime,dtimes from document where author like '%%%s%%'",name);
																break;
												}
										}
										if(ch==2)
										{
												switch(choose)
												{		
														case 1:	sprintf(cmd,"select author,name,atime,dtimes from document where author='%s'",name);
																break;
														case 2:	sprintf(cmd,"select author,name,atime,dtimes from document where author like '%%%s%%'",name);
																break;
												}
										}
										if(sqlite3_get_table(db,cmd,&result,&nrow,&ncolumn,&errmsg)!=SQLITE_OK)
										{
												printf("搜索作者失败\n");
												printf("%s\n",errmsg);
												break;
										}
										if(nrow==0)
										{
												printf("未搜索到相关文档\n");
												break;
										}
										index=ncolumn;
										for(i=0;i<nrow;i++)
										{
												for(j=0;j<ncolumn;j++)
												{
														printf("%s: %s\n",result[j],result[index++]);
												}
												printf("------------------------------\n");
										}
										if(ch==2)
										{
												printf("删除后不可恢复,确定要删除该文档吗(y/n)\n");
												if(getchar()=='y')
												{
														getchar();
														sprintf(cmd,"delete from document where author='%s'",name);
														if(sqlite3_exec(db,cmd,NULL,NULL,&errmsg)!=SQLITE_OK)
														{
																printf("删除文档失败\n");
																printf("%s\n",errmsg);
																break;
														}
														printf("文档已删除\n");
												}
												else
												{
														printf("取消了删除操作\n");
												}
										}
										printf("请按回车键继续\n");
										getchar();
								}
								break;
						case 3: while(1)
								{
										printf("请输入时间(日期)(输入q返回)\n");
										printf("日期格式:xxxx-xx-xx\n");
										fgets(name,MAX_FILENAME,stdin);
										name[strlen(name)-1]='\0';
										if(name[0]=='q')
										{
												break;
										}
										if(ch==1)
										{
												sprintf(cmd,"select author,name,atime,dtimes from document where atime like '%s%%'",name);
										}
										else if(ch==2)
										{
												sprintf(cmd,"select author,name,atime,dtimes from document where atime like '%s%%'",name);
										}
										if(sqlite3_get_table(db,cmd,&result,&nrow,&ncolumn,&errmsg)!=SQLITE_OK)
										{
												printf("搜索上传日期失败\n");
												printf("%s\n",errmsg);
												break;
										}
										if(nrow==0)
										{
												printf("未搜索到相关文档\n");
												break;
										}
										for(i=0;i<nrow;i++)
										{
												for(j=0;j<ncolumn;j++)
												{
														printf("%s: %s\n",result[j],result[index++]);
												}
												printf("------------------------------");
										}
										if(ch==2)
										{
												printf("1.删除当天所有文档\n");
												printf("2.输入文档名删除\n");
												printf("0.取消\n");
												if(getchar()=='1')
												{
														getchar();
														printf("删除后不可恢复,确定要删除吗？(y/n)\n");
														if(getchar()=='y')
														{
																getchar();
																sprintf(cmd,"delete from document where atime like '%s%%'",name);
														}
														else
														{
																printf("取消了删除操作\n");
																return;
														}
														if(sqlite3_exec(db,cmd,NULL,NULL,&errmsg)!=SQLITE_OK)
														{
																printf("删除文档失败\n");
																printf("%s\n",errmsg);
																break;
														}
														printf("文档已删除\n");
												}
												else if(getchar()=='2')
												{
														getchar();
														printf("请输入文档名(输入q返回)\n");
														fgets(name,MAX_FILENAME,stdin);
														name[strlen(name)-1]='\0';
														if(name[0]=='q')
																break;
														printf("删除后不可恢复,确定要删除该文档吗(y/n)\n");
														if(getchar()=='y')
														{
																getchar();
																sprintf(cmd,"delete from document where name='%s'",name);
																if(sqlite3_exec(db,cmd,NULL,NULL,&errmsg)!=SQLITE_OK)
																{
																		printf("删除文档失败\n");
																		printf("%s\n",errmsg);
																		break;
																}
																printf("文档已删除\n");
														}
														else
																printf("取消了删除操作\n");
												}
												else
														printf("取消了删除操作\n");
												printf("请按回车键继续\n");
												getchar();
										}
								}
								break;
						case 0: return;
						default:printf("请输入有效数字\n");
								break;
				}
		}
}
