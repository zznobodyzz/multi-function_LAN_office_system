#include"client_head.h"
#include"client_game.h"
extern void * Set_upload_score_at_work(void * arg);
extern void * Alarm(void * arg);
extern char name[MAX_NAME];
extern char game_pk_name[MAX_NAME];
extern int work_queue_state;
extern sem_t sem;
extern int sync_err_flag;
extern int game_accept_flag;
extern int game_not_accept;
extern int alarm_flag;
extern int gaming_flag;
extern int free_flag;
extern pthread_mutex_t free_lock;
extern pthread_mutex_t lock;
extern int skfd;
int game_state=0;

int getch(void)
{
		int fd=0,ch;
		if(tcsetattr(fd,TCSANOW,&tm)<0)
				return -1;

		ch=getchar();

		if(tcsetattr(fd,TCSANOW,&tm_old)<0)
				return -1;
		return ch;
}

int Upload_game_score(int sockfd,int game,int score,int implicit)
{
		USR usr;
		SER ser;
		usr.cmd=UPLOAD_GAME;
		strcpy(usr.name,name);
		pthread_t tsetatwork;
		write(sockfd,&usr,sizeof(usr));
		struct {
			int game;
			int score;
			int sockfd;
		}at_work;
		at_work.game=game;
		at_work.score=score;
		at_work.sockfd=sockfd;
		if(read(sockfd,&ser,sizeof(ser))<=0)
		{
				if(implicit==0)
				{
						printf("服务器未响应,请稍后再试\n");
						if(work_queue_state==1)
						{
								pthread_create(&tsetatwork,NULL,Set_upload_score_at_work,&at_work);
								sem_wait(&sem);
								printf("系统已保留你的成绩,并将在将来某一空闲时刻上传\n");
						}
				}
				return 1;
		}
		if(ser.sig!=GET_UPLOAD_GAME)
		{
				if(implicit==0)
				{
						printf("与服务器同步发生错误,请稍后再试\n");
						if(work_queue_state==1)
						{
								pthread_create(&tsetatwork,NULL,Set_upload_score_at_work,&at_work);
								sem_wait(&sem);
								printf("系统已保留你的成绩,并将在将来某一空闲时刻上传\n");
						}
						sync_err_flag=1;
				}
				return 1;
		}
		switch(game)
		{
				case TETRIS: strcpy(usr.data,"tetris");
					 break;
				case SNAKE:  strcpy(usr.data,"snake");
					 break;
		}
		write(sockfd,&usr,sizeof(usr));
		if(read(sockfd,&ser,sizeof(ser))<=0)
		{
				if(implicit==0)
				{
						printf("上传分数失败,服务器未响应\n");
						if(work_queue_state==1)
						{
								pthread_create(&tsetatwork,NULL,Set_upload_score_at_work,&at_work);
								sem_wait(&sem);
								printf("系统已保留你的成绩,并将在将来某一空闲时刻上传\n");
						}
				}
				return 1; 
		}
		if(ser.sig==UPLOAD_GAME_READY)
		{
				usr.cmd=score;
				write(sockfd,&usr,sizeof(usr));
				if(read(sockfd,&ser,sizeof(ser))<=0)
				{
						if(implicit==0)
						{	
								printf("上传分数失败,服务器未响应\n");
								if(work_queue_state==1)
								{
										pthread_create(&tsetatwork,NULL,Set_upload_score_at_work,&at_work);
										sem_wait(&sem);
										printf("系统已保留你的成绩,并将在将来某一空闲时刻上传\n");
								}
						}
						return 1;
				}
				if(ser.sig!=UPLOAD_GAME_OK)
				{
						if(implicit==0)
						{	
								printf("上传分数失败,服务器异常\n");
								if(work_queue_state==1)
								{
										pthread_create(&tsetatwork,NULL,Set_upload_score_at_work,&at_work);
										sem_wait(&sem);
										printf("系统已保留你的成绩,并将在将来某一空闲时刻上传\n");
								}
						}
						return 1;
				}
				printf("上传游戏分数成功!注意,若比个人最高分低则不会更新\n");
				return 0;
		}
		else 
		{
				if(implicit==0)
				{
						printf("上传分数失败,服务器异常\n");
						if(work_queue_state==1)
						{
								pthread_create(&tsetatwork,NULL,Set_upload_score_at_work,&at_work);
								sem_wait(&sem);
								printf("系统已保留你的成绩,并将在将来某一空闲时刻上传\n");
						}
				}
				return 1; 
		}
}

void Rank_view(int sockfd,int game)
{
		USR usr;
		SER ser;
		usr.cmd=RANK_VIEW;
		strcpy(usr.name,name);
		write(sockfd,&usr,sizeof(usr));
		if(read(sockfd,&ser,sizeof(ser))<=0)
		{
				printf("服务器未响应,请稍后再试\n");
				return;
		}
		if(ser.sig!=GET_RANK_VIEW)
		{
				printf("与服务器同步发生错误,请稍后再试\n");
				sync_err_flag=1;
				return;
		}
		switch(game)
		{
				case WUZIQI: sprintf(usr.data,"%d",WUZIQI);
							 break;
				case TETRIS: sprintf(usr.data,"%d",TETRIS);
							 break;
				case SNAKE:  sprintf(usr.data,"%d",SNAKE);
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
		if(ser.sig==RANK_VIEW_ERR)
		{
				printf("获取天梯信息失败\n");
				return;
		}
		if(ser.sig==RANK_VIEW_YET)
		{
				printf("还未有排名\n");
				return;
		}
		if(ser.sig==RANK_VIEW_OK)
		{
				printf("%s\n",ser.data);
				while(1)
				{
						if(read(sockfd,&ser,sizeof(ser))<=0)
						{
								printf("服务器未响应,请稍后再试\n");
								return;
						}
						if(ser.sig==RANK_VIEW_OVER)
								break;
						printf("%s\n",ser.data);
				}
				printf("天梯数据接收完毕\n");
		}
}

void * Wait_for_game_acception(void * arg)
{
		pthread_detach(pthread_self());
		SER ser;
		int sockfd=*((int *)arg);
		if(read(sockfd,&ser,sizeof(ser))<0)
		{
				printf("服务器未响应,请稍后再试\n");
				return;
		}
		if(ser.sig==GAME_START)
				game_accept_flag=1;
		else if(ser.sig==GAME_NOT_ACCEPT)
				game_not_accept=1;
		alarm_flag=0;
		kill(getpid(),SIGUSR2);
		pthread_exit(NULL);
}

void Wuziqi_game(int sockfd)
{
		Wuziqi_play_with_bot();
}

void Wuziqi_pk(int sockfd,char * game_name,int accept_flag)
{
		USR usr;
		SER ser;
		usr.cmd=WUZIQI_PK;
		strcpy(usr.name,name);
		pthread_t talarm,tgameacception;
		write(sockfd,&usr,sizeof(usr));
		if(read(sockfd,&ser,sizeof(ser))<=0)
		{
				printf("服务器未响应,请稍后再试\n");
				return;
		}
		if(ser.sig==GAME_UNABLE)
		{
				printf("游戏服务正在维护,请稍后再试\n");
				return;
		}
		if(ser.sig!=GET_WUZIQI_PK)
		{
				printf("与服务器同步发生错误,请稍后再试\n");
				sync_err_flag=1;
				return;
		}
		if(game_name)
		{
				strcpy(usr.data,game_name);
				if(accept_flag==GAME_NOT_ACCEPT)
				{
						usr.cmd=GAME_NOT_ACCEPT;
						write(sockfd,&usr,sizeof(usr));
						return;
				}
				else if(accept_flag==GAME_ACCEPT)
				{
						usr.cmd=GAME_ACCEPT;
						write(sockfd,&usr,sizeof(usr));
						if(read(sockfd,&ser,sizeof(ser))<=0)
						{
								printf("服务器未响应,请稍后再试\n");
								return;
						}
						if(ser.sig==GAME_START)
						{
								printf("游戏开始!\n");
								gaming_flag=1;
								Wuziqi_game_body(sockfd);
								gaming_flag=0;
						}
						return;
				}
		}
		printf("请指定对手(输入q取消):\n");
		fgets(usr.data,MAX_DATA,stdin);
		usr.data[strlen(usr.data)-1]='\0';
		if(usr.data[0]=='q'&&usr.data[1]=='\0')
		{
				usr.cmd=GAME_CEL;
				write(sockfd,&usr,sizeof(usr));
				printf("取消了对战\n");
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
		if(ser.sig==PK_ERR)
		{
				printf("创建对战失败,对方可能已下线\n");
				return;
		}
		if(ser.sig==SET_GAME_QUEUE_OK)
		{
				printf("已发送请求,正在等待对方回应\n");
				printf("输入q取消本次对战\n");
				int alarm_time=40;
				alarm_flag=1;
				pthread_create(&talarm,NULL,Alarm,&alarm_time);
				pthread_create(&tgameacception,NULL,Wait_for_game_acception,&sockfd);
				while(1)
				{
						int c=getchar();
						if(c==-1)
						{
								if(game_accept_flag==1)
										break;  //break while(1)
								if(game_not_accept==1)
										printf("对方未接收请求\n");
								else
										printf("对方响应超时\n");
						}
						else if((char)c=='q')
						{
								getchar();
								printf("");
						}
						else 
						{
								getchar();
								continue;
						}
						usr.cmd=GAME_CEL;
						write(sockfd,&usr,sizeof(usr));
						pthread_cancel(tgameacception);
						return;
				}
				printf("对方已接受对战请求,游戏开始\n");
				usr.cmd=GAME_START;
				write(sockfd,&usr,sizeof(usr));
				gaming_flag=1;
				Wuziqi_game_body(sockfd);
				gaming_flag=0;
				return;
		}
}

void Five_in_one_line(int sockfd)
{
		int choose;
		int retu;
		while(1)
		{
				retu=0;
				Game_menu2();
				scanf("%d",&choose);
				getchar();
				switch(choose)
				{
						case 1: Wuziqi_game(sockfd);
								break;
						case 2: free_flag=1;
								pthread_mutex_lock(&free_lock);
								Rank_view(sockfd,WUZIQI);
								pthread_mutex_unlock(&free_lock);
								break;
						case 3: free_flag=1;
								pthread_mutex_lock(&free_lock);
								Wuziqi_pk(sockfd,NULL,1);
								pthread_mutex_unlock(&free_lock);
								break;
						case 0: retu=1;
								break;
				}
				if(retu==1)
						break;
				if(sync_err_flag==1)
						return;
				free_flag=0;
				printf("请按回车键继续\n");
				getchar();
		}
		return;
}

void Game_due(int signo)
{	
		int sockfd=skfd;
		printf("收到来自 %s 的五子棋对战请求\n",game_pk_name);
		printf("是否接受(y/n)\n");
		int c=getchar();
		if((char)c=='y')
		{
				free_flag=1;
				pthread_mutex_lock(&free_lock);
				Wuziqi_pk(sockfd,game_pk_name,GAME_ACCEPT);
				pthread_mutex_unlock(&free_lock);
		}
		else if((char)c=='n')
		{
				free_flag=1;
				pthread_mutex_lock(&free_lock);
				Wuziqi_pk(sockfd,game_pk_name,GAME_NOT_ACCEPT);
				pthread_mutex_unlock(&free_lock);
		}
		free_flag=0;
		printf("请按回车键继续\n");
		getchar();
}

//暂停操作，用中断实现
void Pause(int signo)
{
		int c;
		int fd=0;
		while(1)
		{
				c=getch();
				switch(c)
				{
					case 'q': pthread_mutex_unlock(&lock);
							  tcsetattr(fd,TCSANOW,&tm_old);
							  game_state=0;
							  pthread_exit(NULL);
					case 't': return;
					default:  break;
				}
		}
}

void Tetris_Snake(int sockfd,int game)
{
		int c;
		while(1)
		{
				Game_menu3();
				scanf("%d",&c);
				getchar();
				switch(c)
				{
						case 1: system("clear");
								printf("注意，由于游戏是模拟raw终端实现的getch功能，游戏过程中请务必正常退出，否则可能会导致终端错误\n");
								printf("游戏中任意时刻可以按'q'退出,按't'暂停/恢复\n");
								printf("确认已读以上提示？(按回车继续，输入q返回)\n");
								c=getchar();
								if(c=='q')
								{
										getchar();
										return;
								}
								if(c!=10)
								{
										getchar();
										printf("输入了q以外的字符,默认继续\n");
								}
								if(game==TETRIS)
										Tetris_game_body(sockfd);
								else if(game==SNAKE)
										Snake_game_body(sockfd);
								break;
						case 2: free_flag=1;
								pthread_mutex_lock(&free_lock);
								Rank_view(sockfd,game);
								pthread_mutex_unlock(&free_lock);
								break;
						case 0: return;
						default:printf("请输入有效数字\n");
								break;
				}
				free_flag=0;
				printf("请按回车键继续\n");
				getchar();
		}
}

void Game_func(int sockfd)
{
		int choose;
		int retu;
		while(1)
		{
				Game_menu();
				scanf("%d",&choose);
				getchar();
				switch(choose)
				{
						case 1: Five_in_one_line(sockfd);
								break;
						case 2: Tetris_Snake(sockfd,TETRIS);
								break;
						case 3: Tetris_Snake(sockfd,SNAKE);
								break;
						case 0: retu=1;
								break;
						default:printf("请输入有效数字\n");
								break;
				}
				if(retu==1)
						break;
				if(sync_err_flag==1)
						return;
				free_flag=0;
				printf("请按回车继续\n");
				getchar();
		}
		return;
}
