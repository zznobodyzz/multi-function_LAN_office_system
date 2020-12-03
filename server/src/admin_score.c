#include"server_head.h"

extern sqlite3 * db;

void Sco_count(void)
{
		char cmd[MAX_CMD];
		int index;
		int i,j;
		int nrow;
		int ncolumn;
		char **result;
		char * errmsg;
		strcpy(cmd,"select name,sco from client where sco>0");
		if(sqlite3_get_table(db,cmd,&result,&nrow,&ncolumn,&errmsg)!=SQLITE_OK)
		{
				printf("操作失败\n");
				printf("%s\n",errmsg);
		}
		if(nrow==0)
		{
				printf("还未有用户评分\n");
				return;
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
		strcpy(cmd,"select avg(sco) from client where sco>0");
		if(sqlite3_get_table(db,cmd,&result,&nrow,&ncolumn,&errmsg)!=SQLITE_OK)
		{
				printf("操作失败\n");
				printf("%s\n",errmsg);
		}
		printf("平均打分(满分5分): %s\n",result[1]);
}
