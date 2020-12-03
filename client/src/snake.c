#include"client_head.h"
#include"client_snake.h"
#include"client_game.h"
extern int getch(void);
extern sem_t sem;
extern int free_flag;
extern pthread_mutex_t free_lock;
extern struct termios tm,tm_old;
extern int game_state;

void Score_count(int flag)
{
	static int speed_flag=0;	
	if(flag==1)
		score+=10;
	printf("current score: %d\n",score);
	if(score==100)
	{
		if(speed_flag==0)
		{
			speed-=20000;
			speed_flag=1;
		}
	}
	else if(score==200)
	{
		if(speed_flag==1)
		{
			speed-=20000;
			speed_flag=0;
		}
	}
	else if(score==300)
	{
		if(speed_flag==0)
		{	
			speed-=20000;
			speed_flag=1;
		}
	}
	else if(score==400)
	{
		if(speed_flag==1)
		{
			speed-=20000;
			speed_flag=0;
		}
	}
	else if(score==500)
	{
		if(speed_flag==0)
		{	
			speed-=20000;
			speed_flag=1;
		}
	}
}

void Print_menu(void)
{
	system("clear");
	printf("          (@_@)\n");
	printf("welcome to play \"the snake\"\n\n");
	printf("1.normal mode\n\n");
	printf("2.hard mode\n\n");
	printf("3.challenge mode\n\n");
	printf("4.watch bot play\n\n");
	printf("0.return\n");
}

int Random(int num)
{
	int i;
	srand((unsigned)time(NULL));
	i=rand();
	if(num<0)
		return i%((-num)-2);
	return i%(num-10)+5;
}

HEAD * Snake_init(void)
{
	int i;
	HEAD * head=NULL;
	head=(HEAD *)malloc(sizeof(HEAD));
	if(head==NULL)
	{
		printf("Snake init failed\n");
		return NULL;
	}
	head->next=NULL;
	head->tail=NULL;
	head->pos[0]=Random(SNAKE_HEIGHT);
	head->pos[1]=Random(SNAKE_WIDTH);
	head->direction=Random(14)-5;
	for(i=0;i<3;i++)
	{
		BODY * body=NULL;
		body=(BODY *)malloc(sizeof(BODY));
		if(body==NULL)
		{
			printf("Snake init failed\n");
			return NULL;
		}
		if(head->next==NULL)
			head->next=body;
		body->next=NULL;
		body->prev=head->tail;
		if(head->tail!=NULL)
			head->tail->next=body;
		head->tail=body;
	}
	BODY * work=head->next;
	switch(head->direction)
	{
		case LEFT: 
			for(i=1;i<=3;i++)
			{	
				work->pos[0]=head->pos[0];
				work->pos[1]=head->pos[1]+i;
				work=work->next;
			}
			break;
		case RIGHT:
			for(i=1;i<=3;i++)
			{	
				work->pos[0]=head->pos[0];
				work->pos[1]=head->pos[1]-i;
				work=work->next;
			}
			break;
		case UP:
			for(i=1;i<=3;i++)
			{	
				work->pos[0]=head->pos[0]+i;
				work->pos[1]=head->pos[1];
				work=work->next;
			}
			break;
		case DOWN:
			for(i=1;i<=3;i++)
			{	
				work->pos[0]=head->pos[0]-i;
				work->pos[1]=head->pos[1];
				work=work->next;
			}
			break;
		default:return NULL;
	}
	return head;
}

void Snake_set(HEAD * head,int option)
{
	if(option==0)	
		platform[head->pos[0]][head->pos[1]]=0;
	else
		platform[head->pos[0]][head->pos[1]]=1;  //1 means head
	BODY * work=head->next;
	while(work!=NULL)
	{
		if(option==0)
			platform[work->pos[0]][work->pos[1]]=0;
		else
			platform[work->pos[0]][work->pos[1]]=2;  //2 means body
		work=work->next;
	}
}

void Food_set(void)
{
	int h,w;	
	int count=0;
	switch(food)
	{
		case NORMAL_FOOD:
				while(1)
				{
					h=Random(-SNAKE_HEIGHT)+1;
					w=Random(-SNAKE_WIDTH)+1;
					if(platform[h][w]==0)
					{
						platform[h][w]=3;	//3 means food
						break;
					}
				}
				break;
		case STRANGE_FOOD:
				while(1)
				{	
					if(Random(-4)==1)
					{
						h=Random(-4)+1;
						w=Random(-SNAKE_WIDTH)+1;
					}
					else
					{
						h=Random(-SNAKE_HEIGHT)+1;
						w=Random(-4)+1;
					}	
					if(platform[h][w]==0)
					{
						platform[h][w]=3;	
						break;
					}
					count++;
					if(count>=3)
					{
						h=Random(-SNAKE_HEIGHT);
						w=Random(-SNAKE_WIDTH);
						if(platform[h][w]==0)
						{
							platform[h][w]=3;	 // 3 means food
							break;
						}
					}
				}
				break;
	}
	foodpos[0]=h;
	foodpos[1]=w;
	printf("x: %d,y: %d\n",w,h);
    return;	
}

void Platform_init(HEAD * head)
{
	pthread_mutex_init(&lock,NULL);
	sem_init(&sem,0,0);
	int fd=0;
	if(tcgetattr(fd,&tm)<0)
		exit(0);
	tm_old=tm;
	cfmakeraw(&tm);
	int i;
	for(i=0;i<SNAKE_HEIGHT;i++)
	{
		platform[i][0]=4;
		platform[i][SNAKE_WIDTH-1]=4;  //4 means wall
	}
	for(i=0;i<SNAKE_WIDTH;i++)
	{
		platform[0][i]=4;
		platform[SNAKE_HEIGHT-1][i]=4;
	}
	Snake_set(head,1);
	Food_set();
}

void Print_all(void)
{
	int i,j;
	system("clear");
	for(i=0;i<SNAKE_HEIGHT;i++)
	{
		for(j=0;j<SNAKE_WIDTH;j++)
		{
			if(platform[i][j]>1)
				printf("*");
			else if(platform[i][j]==1)
				printf("O");
			else
				printf(" ");
		}
		printf("\n");
	}
}

void Snake_move(HEAD * head)
{
	BODY * work=head->tail;
	while(work->prev!=NULL)
	{
		work->pos[0]=work->prev->pos[0];
		work->pos[1]=work->prev->pos[1];
		work=work->prev;
	}
	work->pos[0]=head->pos[0];
	work->pos[1]=head->pos[1];
}

static void * Control(void * arg)
{
	pthread_detach(pthread_self());
	int c;
	int fd=0;
	while(1)
	{
		c=getch();
		pthread_mutex_lock(&lock);
		switch(c)
		{
			case 'w':
				if(snake_head->direction!=DOWN)	
					snake_head->direction=UP;
				break;
			case 's':
				if(snake_head->direction!=UP)
					snake_head->direction=DOWN;
				break;
			case 'a':
				if(snake_head->direction!=RIGHT)
					snake_head->direction=LEFT;
				break;
			case 'd':
				if(snake_head->direction!=LEFT)
					snake_head->direction=RIGHT;
				break;
			case 't':
				pthread_kill(pthread_self(),SIGALRM);	
				break;
			case 'q':
				printf("sure to quit？(y for yes,any key for cancel)\n");
				c=getch();
				if(c!='y')
					break;
				game_state=0;
				pthread_mutex_unlock(&lock);
				pthread_exit(NULL);
		}
		pthread_mutex_unlock(&lock);
	}
}

void * Bot(void * arg)
{
	int i;	
	HEAD * head=snake_head;
	while(1)
	{
start:			
		sem_wait(&sem);	
		if(foodpos[0]>head->pos[0] && foodpos[1]>head->pos[1]) //if food is right-below 
		{
			if(head->direction==UP)	
			{
				if(platform[head->pos[0]][head->pos[1]+1]==2)  //if right cann't go
				{
					if(platform[head->pos[0]][head->pos[1]]==2)  //if up cann't go 
						head->direction=LEFT;
					else
						continue;
				}
				else
					head->direction=RIGHT;	
			}	
			else if(platform[head->pos[0]+1][head->pos[1]]==2)
			{
				if(head->direction==DOWN)
				{
					for(i=0;i<head->pos[1];i++)	
					{
						if(platform[head->pos[0]][i]==2)
						{
							head->direction=RIGHT;
							goto start;
						}
					}
					head->direction=LEFT;	
				}
				else	
					continue;
			}
			else
				head->direction=DOWN;		
		}
		else if(foodpos[0]>head->pos[0] && foodpos[1]<head->pos[1]) //if food is left-below 
		{
			if(head->direction==UP)	
			{
				if(platform[head->pos[0]][head->pos[1]-1]==2)  //if left cann't go
				{
					if(platform[head->pos[0]-1][head->pos[1]]==2)  //if up cann't go
						head->direction=RIGHT;
					else
						continue;
				}				
				else
					head->direction=LEFT;	
			}
			else if(platform[head->pos[0]+1][head->pos[1]]==2)
			{
				if(head->direction==DOWN)
				{
					for(i=0;i<head->pos[1];i++)	
					{
						if(platform[head->pos[0]][i]==2)
						{
							head->direction=RIGHT;
							goto start;
						}
					}
					head->direction=LEFT;	
				}
				else
					continue;	
			}
			else		
				head->direction=DOWN;	
		}
		else if(foodpos[0]<head->pos[0] && foodpos[1]>head->pos[1]) //if food is right-above 
		{
			if(head->direction==DOWN)
			{
				if(platform[head->pos[0]][head->pos[1]+1]==2)  //if right cann't go
				{
					if(platform[head->pos[0]+1][head->pos[1]]==2)  //if down cann't go
						head->direction=LEFT;
					else 
						continue;
				} 
				else
					head->direction=RIGHT;
			}
			else if(platform[head->pos[0]-1][head->pos[1]]==2)
			{
				if(head->direction==UP)
				{
					for(i=0;i<head->pos[1];i++)	
					{
						if(platform[head->pos[0]][i]==2)
						{
							head->direction=RIGHT;
							goto start;
						}
					}
					head->direction=LEFT;	
				}
				else
					continue;
			}
			else		
				head->direction=UP;
		}
		else if(foodpos[0]<head->pos[0] && foodpos[1]<head->pos[1]) //if food is left-above
		{
			if(head->direction==DOWN)
			{
				if(platform[head->pos[0]][head->pos[1]-1]==2)  //if left cann't go
				{
					if(platform[head->pos[0]][head->pos[1]]==2)  //if down cann't go
						head->direction=RIGHT;
					else
						continue;				
				}
				else
					head->direction=LEFT;	
			}
			else if(platform[head->pos[0]-1][head->pos[1]]==2)  //if up cann't go
			{
				if(head->direction==UP)
				{
					for(i=1;i<head->pos[1];i++)
					{
						if(platform[head->pos[0]][i]==2)
						{
							head->direction=RIGHT;
							goto start;
						}
					}
					head->direction=LEFT;
				}
				else
					continue;
			}
			else
				head->direction=UP;
		}
		else if(foodpos[0]==head->pos[0]) //if food is on the same row
		{
			if(head->pos[1]>foodpos[1])  //if food is on the left
			{
				if(head->direction==RIGHT) 
				{
					if(head->pos[0]==1)
						head->direction=DOWN;
					else if(head->pos[0]==SNAKE_HEIGHT-2)
						head->direction=UP;
					else if(platform[head->pos[0]-1][head->pos[1]]==2)
						head->direction=DOWN;
					else
						head->direction=UP;		
				}	
				else if(platform[head->pos[0]][head->pos[1]-1]==2)  //if left cann't go
				{
					if(head->direction==LEFT)
					{
						if(platform[head->pos[0]+1][head->pos[1]]==2)
							head->direction=UP;
						else if(platform[head->pos[0]-1][head->pos[1]]==2)
							head->direction=DOWN;
						else
						{	
							for(i=1;i<head->pos[0];i++)
							{
								if(platform[i][head->pos[1]]==2)
								{
									head->direction=DOWN;
									goto start;
								}
							}	
							head->direction=UP;
						}
					}
					else if(head->direction==UP)
					{
						if(platform[head->pos[0]-1][head->pos[1]]==2) //if up cann't go
							head->direction=RIGHT;
						else
							continue;
					}
					else if(head->direction==DOWN)
					{
						if(platform[head->pos[0]+1][head->pos[1]]==2) //if down cann't go
							head->direction=RIGHT;
						else	
							continue;
					}
				}
				else
					head->direction=LEFT;	
			}
			else if(head->pos[1]<foodpos[1]) //if food is on the right
			{
				if(head->direction==LEFT)
				{
					if(head->pos[0]==1)
						head->direction=DOWN;
					else if(head->pos[0]==SNAKE_HEIGHT-2)
						head->direction=UP;
					else if(platform[head->pos[0]-1][head->pos[1]]==2)
						head->direction=DOWN;
					else
						head->direction=UP;		
				}	
				else if(platform[head->pos[0]][head->pos[1]+1]==2)  //if right cann't go
				{
					if(head->direction==RIGHT)
					{
						if(platform[head->pos[0]+1][head->pos[1]]==2)
							head->direction=UP;
						else if(platform[head->pos[0]-1][head->pos[1]]==2)
							head->direction=DOWN;
						else
						{
							for(i=1;i<head->pos[0];i++)
							{
								if(platform[i][head->pos[1]]==2)
								{
									head->direction=DOWN;
									goto start;
								}
							}	
							head->direction=UP;
						}
					}
					else if(head->direction==UP)
					{
						if(platform[head->pos[0]-1][head->pos[1]]==2) //if up cann't go
							head->direction=LEFT;
						else
							continue;
					}
					else if(head->direction==DOWN)
					{
						if(platform[head->pos[0]+1][head->pos[1]]==2) //if down cann't go
							head->direction=LEFT;
						else	
							continue;
					}
				}
				else	
					head->direction=RIGHT;	
			}
		}
		else if(foodpos[1]==head->pos[1]) //if on the same column
		{
			if(head->pos[0]>foodpos[0])  //if above
			{
				if(head->direction==DOWN)
				{
					if(head->pos[1]==1)
						head->direction=RIGHT;
					else if(head->pos[1]==SNAKE_WIDTH-2)
						head->direction=LEFT;		
					else if(platform[head->pos[0]][head->pos[1]-1]==2)
						head->direction=RIGHT;
					else
						head->direction=LEFT;		
				}	
				else if(platform[head->pos[0]-1][head->pos[1]]==2)  //if up cann't go
				{
					if(head->direction==UP)
					{
						if(platform[head->pos[0]][head->pos[1]-1]==2)
							head->direction=RIGHT;
						else if(platform[head->pos[0]-1][head->pos[1]+1]==2)
							head->direction=LEFT;
						else
						{	
							for(i=1;i<head->pos[1];i++)
							{
								if(platform[head->pos[0]][i]==2)
								{
									head->direction=RIGHT;
									goto start;
								}
							}	
							head->direction=LEFT;
						}
					}
					else if(head->direction==LEFT)
					{
						if(platform[head->pos[0]][head->pos[1]-1]==2) //if left cann't go
							head->direction=DOWN;
						else
							continue;
					}
					else if(head->direction==RIGHT)
					{
						if(platform[head->pos[0]][head->pos[1]+1]==2) //if right cann't go
							head->direction=DOWN;
						else	
							continue;
					}
				}
				else		
					head->direction=UP;	
			}	
			else if(head->pos[0]<foodpos[0])  //if below
			{
				if(head->direction==UP)
				{
					if(head->pos[1]==1)
						head->direction=RIGHT;	
					else if(head->pos[1]==SNAKE_WIDTH-2)
						head->direction=LEFT;		
					else if(platform[head->pos[0]][head->pos[1]-1]==2)
						head->direction=RIGHT;
					else
						head->direction=LEFT;		
				}
				else if(platform[head->pos[0]+1][head->pos[1]]==2)  //if down cann't go
				{
					if(head->direction==DOWN)
					{
						if(platform[head->pos[0]][head->pos[1]-1]==2)
							head->direction=RIGHT;
						else if(platform[head->pos[0]-1][head->pos[1]+1]==2)
							head->direction=LEFT;
						else
						{						
							for(i=1;i<head->pos[1];i++)
							{
								if(platform[head->pos[0]][i]==2)
								{
									head->direction=RIGHT;
									goto start;
								}
							}	
							head->direction=LEFT;
						}	
					}
					else if(head->direction==LEFT)
					{
						if(platform[head->pos[0]][head->pos[1]-1]==2) //if left cann't go
							head->direction=UP;
						else
							continue;
					}
					else if(head->direction==RIGHT)
					{
						if(platform[head->pos[0]][head->pos[1]+1]==2) //if right cann't go
							head->direction=UP;
						else	
							continue;
					}
				}
				else		
					head->direction=DOWN;	
			}
		}
	}	
}

void * Control_bot(void * arg)
{
	while(1)
	{
		switch(getch())
		{
			case 't':
				pthread_mutex_lock(&lock);
				pthread_kill(pthread_self(),SIGALRM);	
				break;
			case 'q':
				game_state=0;
				pthread_exit(NULL);
		}
		pthread_mutex_unlock(&lock);
	}	
}

static int Judge_failure(HEAD * head)
{
	if(head->pos[0]==0 || head->pos[0]==SNAKE_HEIGHT-1 || head->pos[1]==0 || head->pos[1]==SNAKE_WIDTH-1)
		return 1;
	BODY * work=head->next;
	while(work)
	{
		if(head->pos[0]==work->pos[0]&&head->pos[1]==work->pos[1])
			return 1;
		work=work->next;
	}
	return 0;
}

static int Judge_success(HEAD * head)
{
	return (head->pos[0]==foodpos[0] && head->pos[1]==foodpos[1]);
}

int Add_body(HEAD * head,int * tailpos)
{
	BODY * new=NULL;
	new=(BODY *)malloc(sizeof(BODY));
	if(new==NULL)
	{
		printf("some error happened\n");
		return 1;
	}
	head->tail->next=new;
	new->next=NULL;
	new->prev=head->tail;
	head->tail=new;
	new->pos[0]=tailpos[0];
	new->pos[1]=tailpos[1];
	return 0;
}

void Game_start(HEAD * head,int sockfd)
{
	pthread_t tcon;
	pthread_t tauto,tconbot;
	snake_head=head;
	int flag=0;
	int fd=0;
	int tailpos[2]={0};
	int choise;
	Print_all();
	if(autoflag==0)
		pthread_create(&tcon,NULL,Control,NULL);
	else
	{
		pthread_create(&tconbot,NULL,Control_bot,NULL);
		pthread_create(&tauto,NULL,Bot,NULL);
	}
	while(1)
	{
		usleep(speed);
		if(game_state==0)
		{
			tcsetattr(fd,TCSANOW,&tm_old);
			break;
		}
		pthread_mutex_lock(&lock);
		Snake_set(head,0);
		tailpos[0]=head->tail->pos[0];
		tailpos[1]=head->tail->pos[1];
		Snake_move(head);
		switch(head->direction)
		{
			case UP:
				head->pos[0]-=1;
				break;
			case DOWN:
				head->pos[0]+=1;
				break;
			case LEFT:
				head->pos[1]-=1;
				break;
			case RIGHT:
				head->pos[1]+=1;
				break;
		}
		Snake_set(head,1);
		if(Judge_failure(head))
		{
			tcsetattr(fd,TCSANOW,&tm_old);
			break;
		}
		if(Judge_success(head))
		{
			if(Add_body(head,tailpos))
				break; 
			Food_set();
			flag=1;
		}
		tcsetattr(fd,TCSANOW,&tm_old);
		Print_all();
		Score_count(flag);
		tcsetattr(fd,TCSANOW,&tm);
		flag=0;
		if(autoflag==1)
			sem_post(&sem);	
		pthread_mutex_unlock(&lock);
	}
	if(autoflag==0)
		pthread_cancel(tcon);
	else
	{	
		pthread_cancel(tauto);
		pthread_cancel(tconbot);
	}
	printf(" (@^_^@)\n");
	printf("game over~\n");
	if(autoflag==1)
		printf("bot ");
	else 
		printf("you ");
	printf("have earn %d score!!",score);
	if(autoflag!=1)
	{	
		printf("是否上传分数？(y/n)\n");
		choise=getchar();
		getchar();
		if(choise=='y')
		{
			free_flag=1;
			pthread_mutex_lock(&free_lock);
			Upload_game_score(sockfd,SNAKE,score,0);
			pthread_mutex_unlock(&free_lock);
		}
		else if(choise!='n')
			printf("未知选择,默认取消\n");
	}
	score=0;
	printf("press enter to continue\n");
	getchar();
}

void Destroy_snake(HEAD * head)
{
	BODY * work=head->next;
	BODY * del;
	while(work->next)
	{
		del=work->next;
		work->next=del->next;
		free(del);
	}		
	free(work);
	free(head);
}

void Game_clean(HEAD * head)
{
	int i,j;
	for(i=0;i<SNAKE_HEIGHT;i++)
		for(j=0;j<SNAKE_WIDTH;j++)
				platform[i][j]=0;
	Destroy_snake(head);
}

void Snake_game_body(int sockfd)
{
	int choise;
	int flag=0;
	HEAD * head=NULL;
	while(1)
	{
		Print_menu();
		scanf("%d",&choise);
		getchar();
		game_state=1;
		switch(choise)
		{
			case 1: speed=LOW_SPEED;
				    food=NORMAL_FOOD;
				    break;
			case 2: speed=MID_SPEED;
				    food=NORMAL_FOOD;
				    break;
			case 3: speed=HIGH_SPEED;
				    food=STRANGE_FOOD;
				    break;
			case 4: speed=HIGH_SPEED;
				    food=NORMAL_FOOD;
					autoflag=1;
					break;		
			case 0:	game_state=0;
					autoflag=0;
					return;
			default:flag=1;
					printf("press enter to choose again\n");
					getchar();
					break;
		}
		if(flag==1)
			continue;
		head=Snake_init();
		if(head==NULL)
		{
			printf("some error happened\n");	
			return;
		}
		Platform_init(head);
		Game_start(head,sockfd);
		Game_clean(head);
	}
}
