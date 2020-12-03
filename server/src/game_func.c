#include"server_head.h"
#include"server_game.h"
#include"server_wuziqi.h"

extern fd_set commonset;
extern sqlite3 * db;
extern char game_queue_stat;
extern GAME_LIST * game_queue_head;

void * Rank_view(void * arg)
{
		pthread_detach(pthread_self());
		int sockfd;
		char cmd[MAX_CMD];
		SER ser;
		USR usr;
		char ** result;
		int nrow;
		int ncolumn;
		char * errmsg;
		int i,j,index,amount=0;

		sockfd=*(int *)arg;
		free(arg);
		ser.sig=GET_RANK_VIEW;
		write(sockfd,&ser,sizeof(ser));
		if(read(sockfd,&usr,sizeof(usr))<=0)
		{
				printf("客户端主动断开连接\n");
				close(sockfd);
				pthread_exit(NULL);
		}
		switch(atoi(usr.data))
		{
				case WUZIQI: sprintf(cmd,"select name,score from game_rank where gamename='wuziqi' order by score asc");
							 break;
				case TETRIS: sprintf(cmd,"select name,score,createtime from game_rank where gamename='tetris' order by score asc");
							 break;
				case SNAKE:  sprintf(cmd,"select name,score,createtime from game_rank where gamename='snake' order by score asc");
							 break;
				default:     ser.sig=RANK_VIEW_ERR;
							 write(sockfd,&ser,sizeof(ser));
							 FD_SET(sockfd,&commonset);
							 kill(getpid(),SIGUSR2);
							 pthread_exit(NULL);
		}
		if(sqlite3_get_table(db,cmd,&result,&nrow,&ncolumn,&errmsg)!=SQLITE_OK)
		{
				printf("查询游戏天梯 操作失败\n");
				printf("%s\n",errmsg);
				ser.sig=SER_ERR;
				write(sockfd,&ser,sizeof(ser));
				FD_SET(sockfd,&commonset);
				kill(getpid(),SIGUSR2);
				pthread_exit(NULL);
		}
		if(nrow==0)
		{
				ser.sig=RANK_VIEW_YET;
				write(sockfd,&ser,sizeof(ser));
				FD_SET(sockfd,&commonset);
				kill(getpid(),SIGUSR2);
				pthread_exit(NULL);
		}
		memset(ser.data,0,MAX_DATA);
		ser.sig=RANK_VIEW_OK;
		//如果排行榜超过10人,取前10
		if(nrow>10)
				amount=10;
		else 
				amount=nrow;
		index=0;
		for(i=0;i<=amount;i++)
		{
				memset(ser.data,0,MAX_DATA);
				for(j=0;j<ncolumn;j++)
				{
						strcat(ser.data,result[index]);
						//保证每列间隔距离相等
						strncat(ser.data,"              ",10-strlen(result[index++]));
				}	
				write(sockfd,&ser,sizeof(ser));
		}
		ser.sig=RANK_VIEW_OVER;
		write(sockfd,&ser,sizeof(ser));
		FD_SET(sockfd,&commonset);
		kill(getpid(),SIGUSR2);
		pthread_exit(NULL);
}

int Set_game_queue(int sockfd,char * name,int game)
{
		GAME_LIST * new;
		new=(GAME_LIST *)malloc(sizeof(GAME_LIST));
		if(new==NULL)
		{
				game_queue_stat=0;
				return SET_GAME_QUEUE_FAILURE;
		}
		new->sockfd=sockfd;
		strcpy(new->name,name);
		new->game=game;
		new->next=game_queue_head->next;
		game_queue_head->next=new;
}

int Search_game_queue(char * name,int game)
{
		if(game_queue_head->next==NULL)
				return -1;
		GAME_LIST * work;
		work=game_queue_head->next;
		while(work!=NULL)
		{
				if(strcmp(work->name,name)==0&&work->game==game)
						return work->sockfd;
				work=work->next;
		}
		return -1;
}

void Clean_game_queue(int sockfd)
{
		if(game_queue_head->next==NULL)
				return;
		GAME_LIST * del;
		GAME_LIST * work;
		work=game_queue_head;
		while(work->next!=NULL)
		{
				if(work->sockfd==sockfd)
				{
						del=work->next;
						work->next=del->next;
						free(del);
						return;
				}
				work=work->next;
		}
}


void Release_game_queue(void)
{
		GAME_LIST * work;
		GAME_LIST * del=game_queue_head->next;
		while(del!=NULL)
		{
				work=del->next;
				free(del);
				del=work;
		}
		free(game_queue_head);
}

void Create_game_queue(void)
{
		game_queue_head=(GAME_LIST *)malloc(sizeof(GAME_LIST));
		if(game_queue_head==NULL)
		{
				printf("初始化游戏对战功能失败\n");
				game_queue_stat=0;
		}
		else
		{
				printf("初始化游戏对战功能成功\n");
				game_queue_head->next=NULL;
		}
		return;
}

int Add_game_score(char * user_name,int game,int score)
{
		char cmd[MAX_CMD];
		char game_name[10];
		char create_time[MAX_TIME];
		time_t ti;
		ti=time(NULL);
		struct tm * t;
		char ** result;
		int nrow;
		int ncolumn;
		char * errmsg;
		t=localtime(&ti);
		switch(game)
		{
				case WUZIQI:
						strcpy(game_name,"wuziqi");
						create_time[0]='\0';
						break;
				case TETRIS:
						strcpy(game_name,"tetris");
						sprintf(create_time,"%d-%02d-%02d %02d:%02d:%02d",t->tm_year+1900,t->tm_mon+1,t->tm_mday,t->tm_hour,t->tm_min,t->tm_sec);
						break;
				case SNAKE:
						strcpy(game_name,"snake");
						sprintf(create_time,"%d-%02d-%02d %02d:%02d:%02d",t->tm_year+1900,t->tm_mon+1,t->tm_mday,t->tm_hour,t->tm_min,t->tm_sec);
						break;
				default: return 1;
		}
		sprintf(cmd,"select * from game_rank where name='%s' and gamename='%s'",user_name,game_name);
		if(sqlite3_get_table(db,cmd,&result,&nrow,&ncolumn,&errmsg)!=SQLITE_OK)
		{
				printf("添加用户游戏分数前的检测 操作失败\n");
				printf("%s\n",errmsg);
				return 1;
		}
		if(nrow==0)
		{
				if(create_time[0]=='\0')
						sprintf(cmd,"insert into game_rank(name,gamename,score) values('%s','%s',10)",user_name,game_name);
				else
						sprintf(cmd,"insert into game_rank values('%s','%s',%d,'%s')",user_name,game_name,score,create_time);
		}
		else 
		{
				if(create_time[0]=='\0')
						sprintf(cmd,"insert into game_rank(name,gamename,score) values('%s','%s',%d)",user_name,game_name,atoi(result[6])+10);
				else
				{
						if(atoi(result[6])<score)
								sprintf(cmd,"insert into game_rank values('%s','%s',%d,'%s')",user_name,game_name,score,create_time);
						else
								return 0;
				}
		}
		if(sqlite3_exec(db,cmd,NULL,NULL,&errmsg)!=SQLITE_OK)
		{
				printf("添加用户游戏得分 操作失败\n");
				printf("%s\n",errmsg);
				return 1;
		}
}

void * Upload_game_score(void * arg)
{
		pthread_detach(pthread_self());
		int sockfd;
		char cmd[MAX_CMD];
		SER ser;
		USR usr;
		char ** result;
		int nrow;
		int ncolumn;
		char * errmsg;
		char user_name[MAX_NAME];
		int game;
		int score;
		int i,j,index,amount=0;

		sockfd=*(int *)arg;
		free(arg);
		ser.sig=GET_UPLOAD_GAME;
		write(sockfd,&ser,sizeof(ser));
		if(read(sockfd,&usr,sizeof(usr))<=0)
		{
				printf("客户端主动断开连接\n");
				close(sockfd);
				pthread_exit(NULL);
		}
		strcpy(user_name,usr.name);
		if(strcmp(usr.data,"tetris")==0)
		{		
				game=TETRIS;
				ser.sig=UPLOAD_GAME_READY;
		}
		else if(strcmp(usr.data,"snake")==0)
		{	
				game=SNAKE;
				ser.sig=UPLOAD_GAME_READY;
		}
		else 
				ser.sig=UPLOAD_GAME_ERR;
		write(sockfd,&ser,sizeof(ser));
		if(read(sockfd,&usr,sizeof(usr))<=0)
		{
				printf("客户端主动断开连接\n");
				close(sockfd);
				pthread_exit(NULL);
		}
		score=usr.cmd;
		if(Add_game_score(user_name,game,score))
				ser.sig=UPLOAD_GAME_ERR;
		else 
				ser.sig=UPLOAD_GAME_OK;
		write(sockfd,&ser,sizeof(ser));
		FD_SET(sockfd,&commonset);
		kill(getpid(),SIGUSR2);
		pthread_exit(NULL);
}
