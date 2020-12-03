#include"server_head.h"

extern char doc_path[MAX_PATH];
extern char data_base[MAX_FILENAME];
extern char adv_per_time;
extern char print_log_flag;

void Init_config(void)
{
		FILE * fp;
		char * p;
		int flag_docpath=0;
		int flag_database=0;
		int flag_adv_per_time=0;
		int flag_print_log_flag=0;
		char buf[128]="0";
		fp=fopen("./server_config","r");
		if(fp==NULL)
		{
				perror("配置文件打开失败\n");
				exit(-1);
		}
		while(fgets(buf,128,fp))
		{
				if(flag_docpath==0)
				{
						if(strncmp(buf,"docpath=",8)==0)
						{
								for(p=buf;*p!='\n';p++);
								*p='\0';
								p=&buf[8];
								strcpy(doc_path,p);
								printf("文档默认目录加载完毕\n");
								flag_docpath=1;
								memset(buf,0,sizeof(buf));
								continue;
						}
				}
				if(flag_database==0)
				{
						if(strncmp(buf,"database=",9)==0)
						{
								for(p=buf;*p!='\n';p++);
								*p='\0';
								p=&buf[9];
								strcpy(data_base,p);
								printf("数据库文件名已读取\n");
								flag_database=1;
								memset(buf,0,sizeof(buf));
								continue;
						}
				}
				if(flag_adv_per_time==0)
				{
						if(strncmp(buf,"advpertime=",11)==0)
						{
								for(p=buf;*p!='\n';p++);
								*p='\0';
								p=&buf[11];
								adv_per_time=atoi(p);
								printf("自动广播间隔时间已读取\n");
								flag_adv_per_time=1;
								memset(buf,0,sizeof(buf));
								continue;
						}
				}
				if(flag_print_log_flag==0)
				{
						if(strncmp(buf,"printlogflag=",13)==0)
						{
								for(p=buf;*p!='\n';p++);
								*p='\0';
								p=&buf[13];
								print_log_flag=atoi(p);
								printf("调试信息标志已读取\n");
								flag_print_log_flag=1;
								memset(buf,0,sizeof(buf));
								continue;
						}
				}
		}
		fclose(fp);
		printf("配置文件读取完毕\n");
}
