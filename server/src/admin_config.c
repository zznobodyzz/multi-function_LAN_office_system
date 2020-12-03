#include"server_head.h"

extern char doc_path[MAX_PATH];
extern sqlite3 * db;

void Change_docpath(void)
{
		char cmd[MAX_CMD];
		int nrow;
		int ncolumn;
		char **result;
		char * errmsg;
		printf("current docpath=%s\n",doc_path);
		strcpy(cmd,"select * from client where stat=1");
		if(sqlite3_get_table(db,cmd,&result,&nrow,&ncolumn,&errmsg)!=SQLITE_OK)
		{
				printf("检测在线用户失败,安全起见禁止更改\n");
				return;
		}
		if(nrow!=0)
		{
				printf("当前有用户在线,禁止更改文档目录\n");
				return;
		}
		FILE * fp1,* fp2;
		char buf[128]={0};
		char temp[128]="docpath=";
		char doc_path_temp[MAX_PATH]={0};
		printf("请输入新文档目录路径(绝对路径)(输入q取消):\n");
		fgets(doc_path_temp,MAX_PATH,stdin);
		doc_path_temp[strlen(doc_path_temp)-1]='\0';
		if(doc_path_temp[0]=='q'&&doc_path_temp[1]=='\0')
		{
				printf("取消了本次操作\n");
				return;
		}
		struct stat st;
		lstat(doc_path_temp,&st);
		if((st.st_mode&S_IFMT!=S_IFDIR))
		{
				printf("error:不是一个(合法)目录\n");
				return;
		}
		strcpy(doc_path,doc_path_temp);
		if(doc_path[strlen(doc_path)-1]!='/')
				strcat(doc_path,"/");
		fp1=fopen("./server_config","r+");
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
		remove("./server_config");
		rename("config_temp","server_config");
		printf("已更新配置文件\n");
		fclose(fp1);
		fclose(fp2);
		printf("修改文档默认目录成功\n");
		return;
}
