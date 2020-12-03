#include"client_head.h"
#include"client_game.h"
#include"client_wuziqi.h"
extern void * Alarm(void * arg);
extern int alarm_flag;

//创建棋盘头结点(十字链表)
POINT * Create_square(void)
{
		POINT * p;
		p=(POINT *)malloc(sizeof(POINT));
		if(p==NULL)
		{
				printf("malloc failed\n");
				exit(0);
		}
		p->up=NULL;
		p->down=NULL;
		p->left=NULL;
		p->right=NULL;
		return p;
}
//创建棋盘
int Create_point(POINT * p,int length)
{
		int i;
		POINT * work, * pos, * pos2;
		work=p;
		POINT * new;
		int x,y;
		//length为棋盘边长
		for(y=1;y<=length+1;y++)
		{
				for(x=1;x<=length;x++)
				{
						new=(POINT *)malloc(sizeof(POINT));
						if(new==NULL)
						{
								printf("malloc failed\n");
								return -1;
						}
						new->up=NULL;
						new->down=NULL;
						new->left=work;
						work->right=new;
						new->right=NULL;
						work=new;
						new->state=0;
						new->pos[0]=x;
						new->pos[1]=y;
				}
		}
		work=p->right;
		for(x=0;x<length;x++)
		{
				pos=work->right;
				pos2=work;
				while(pos!=NULL)
				{
						if(pos->pos[0]==pos2->pos[0])
						{
								pos2->down=pos;
								pos->up=pos2;
								pos2=pos;
						}
						pos=pos->right;
				}
				work=work->right;
		}
		return 0;
}
//打印棋盘
void Print_square(POINT *p,int length)
{
		POINT * work;
		work=p->right;
		int i;
		system("clear");
		//打印横坐标
		printf("   ");
		for(i=1;i<=length;i++)
		{
				if(i>=10);
						printf("%d",i/10);
				printf(" ");
		}
		printf("\n");
		printf("   ");
		for(i=1;i<=length;i++)
				printf("%d ",i%10);
		printf("\n");
		printf("   _ ");
		for(i=1;i<length;i++)
				printf("_ ");
		printf("\n");
		i=1;
		while(i<=length)
		{
				while(work->pos[1]==i&&work->right!=NULL)
				{		if(work->state==0)
						{
								if(work->pos[0]==1)
										//打印纵坐标
										printf("%2d|_|",i);
								else
										printf("_|");
						}
						else if(work->state==1)
						{
								if(work->pos[0]==1)
										printf("%2d|o|",i);
								else
										printf("o|");
						}
						else if(work->state==2)
						{
								if(work->pos[0]==1)
										printf("%2d|*|",i);
								else
										printf("*|");
						}
						work=work->right;
				}
				i++;
				printf("\n");
		}
}
//下子，c表示棋子颜色
int Put_point(POINT * p,int x,int y,int c)
{
		POINT * work;
		work=p->right;
		while(work!=NULL)
		{
				if(work->pos[0]==x&&work->pos[1]==y)
				{
						if(work->state==0)
								work->state=c;
						else 
								return -1;
				}
				work=work->right;
		}
		return 1;
}
void Remove_chess(POINT * p)
{
		POINT * work=p;
		while(work->right)
		{
				work->state=0;
				work=work->right;
		}
}
//悔棋(not used)
void Return_step(POINT * p,int x,int y,int c)
{
		POINT * work;
		work=p->right;
		while(work!=NULL)
		{
				if((work->pos[0]==x)&&(work->pos[1]==y))
				{
						work->state=0;
				}
		}
}
//下面为判断输赢的规则
//递归检测棋子的周围
int Left(POINT * work,int c)
{
		int count=1;
		if(work->left==NULL)
				return count;
		else if(work->left->state!=c)
				return count;
		else
		{
				count+=Left(work->left,c);
				return count;
		}
}

int Right(POINT * work,int c)
{
		int count=1;
		if(work->right==NULL)
				return count;
		else if(work->right->state!=c)
				return count;
		else
		{
				count+=Right(work->right,c);
				return count;
		}
}

int Up(POINT * work,int c)
{
		int  count=1;
		if(work->up==NULL)
				return count;
		else if(work->up->state!=c)
				return count;
		else
		{
				count+=Up(work->up,c);
				return count;
		}
}

int Down(POINT * work,int c)
{
		int count=1;
		if(work->down==NULL)
				return count;
		else if(work->down->state!=c)
				return count;
		else
		{
				count+=Down(work->down,c);
				return count;
		}
}

int Left_up(POINT * work,int c)
{
		int count=1;
		if(work->left==NULL)		
				return count;
		else if(work->left->up==NULL)
				return count;
		else if(work->left->up->state!=c)
				return count;
		else
		{
				count+=	Left_up(work->left->up,c);
				return count;
		}
}

int Left_down(POINT * work,int c)
{
		int count=1;
		if(work->left==NULL)		
				return count;
		else if(work->left->down==NULL)
				return count;
		else if(work->left->down->state!=c)
				return count;
		else
		{
				count+=Left_down(work->left->down,c);
				return count;
		}
}

int Right_up(POINT * work,int c)
{
		int count=1;
		if(work->right==NULL)		
				return count;
		else if(work->right->up==NULL)
				return count;
		else if(work->right->up->state!=c)
				return count;
		else
		{
				count+=Right_up(work->right->up,c);
				return count;
		}
}

int Right_down(POINT * work,int c)
{
		int count=1;
		if(work->right==NULL)		
				return count;
		else if(work->right->down==NULL)
				return count;
		else if(work->right->down->state!=c)
				return count; 
		else
		{
				count+=Right_down(work->right->down,c);
				return count;
		}
}

void Clean(POINT * p)
{
		POINT * work;
		work=p->right;
		while(work!=NULL)
		{
				p->right=work->right;
				free(work);
				work=p->right;
		}	
		free(p);
}
//判断输赢
int Judge(POINT * p,POINT * point,int x,int y,int c)
{
		POINT * work;
		if(point==NULL)
		{
				work=p->right;
				while(1) //find the point
				{
						if(work->pos[0]==x&&work->pos[1]==y)
								break;
						work=work->right;
				}
		}
		else
				work=point;
		int l=Left(work,c);
		int r=Right(work,c);
		int u=Up(work,c);
		int d=Down(work,c);
		int lu=Left_up(work,c);
		int ld=Left_down(work,c);
		int ru=Right_up(work,c);
		int rd=Right_down(work,c);

		if(l+r==6||u+d==6||lu+rd==6||ru+ld==6)
		{
				return 1;
		}
		else
		{
				printf("=====>\n");
				return 0;
		}
}

int Init_game(POINT ** p,int sockfd)
{
		USR usr;
		SER ser;
		*p=Create_square();
		if(*p==NULL)
		{
				printf("游戏初始化失败,请检查当前运行环境\n");
				if(sockfd!=0)
				{
						usr.cmd==GAME_INIT_ERR;
						write(sockfd,&usr,sizeof(usr));
				}
				return -1;
		}
		if(Create_point(*p,WUZIQI_LENGTH)==-1)
		{
				printf("游戏初始化失败,请检查当前运行环境\n");
				if(sockfd!=0)
				{
						usr.cmd==GAME_INIT_ERR;
						write(sockfd,&usr,sizeof(usr));
				}
				return -1;
		}
		printf("游戏初始化成功\n");
		if(sockfd!=0)
		{
				usr.cmd=GAME_READY;
				write(sockfd,&usr,sizeof(usr));
		}
		return 0;
}

int Set_point(int sockfd,POINT * p)
{
		int * pp;
		int c=0;
		USR usr;
		SER ser;
		pthread_t talarm;
		int x=1,y=1;
		while(1)
		{
				printf("请输入坐标((0,0)表示弃权)\n");
				printf("x:\n");
				scanf("%d",&x);
				getchar();
				printf("y:\n");
				scanf("%d",&y);
				getchar();
				if(x>WUZIQI_LENGTH||y>WUZIQI_LENGTH||x<0||y<0)
						printf("position wrong\n");
				else if(x==0&&y==0)
				{
						printf("确认弃权?(按回车确定,任意字符取消)\n");
						if(getchar()!=10)
						{
								getchar();
								continue;
						}
						printf("你弃权了\n");
						usr.cmd=GIVEUP;
						write(sockfd,&usr,sizeof(usr));
						printf("再来一局?(y/n)\n");
						alarm_flag=1;
						int alarm_time=10;
						pthread_create(&talarm,NULL,Alarm,&alarm_time);
						while(1)
						{
								c=getchar();
								if(c==-1)
								{
										printf("选择超时,默认取消\n");
										usr.cmd=GAME_OVER;
										break;
								}
								else if(c=='n')
								{
										getchar();
										printf("取消了再次对战\n");
										usr.cmd=GAME_OVER;
										pthread_cancel(talarm);
										alarm_flag=0;
										break;
								}
								else if(c=='y')
								{
										getchar();
										printf("选择了再来一局\n");
										usr.cmd=AGAIN;
										pthread_cancel(talarm);
										alarm_flag=0;
										break;
								}
						}
						write(sockfd,&usr,sizeof(usr));
						if(c==-1||c=='n')
								return -1;
						else if(c=='y')
						{	
								printf("等待对方回应...\n");
								if(read(sockfd,&ser,sizeof(ser))<=0)
								{
										printf("服务器未响应,请稍后再试\n");
										return -1;
								}
								if(ser.sig==GAME_OVER)
								{
										printf("对方取消了再次对战\n");
										return -1;
								}
								if(order==1)
								{
										order=2;
										return 2;
								}
								if(order==2)
								{
										order=1;
										return 3;
								}
						}
				}
				else if(Put_point(p,x,y,1)==-1)
						printf("该地址已有棋子\n");
				else
						break;
		}
		usr.data[0]=(char)x;
		usr.data[1]=(char)y;
		usr.data[2]='\0';
		write(sockfd,&usr,sizeof(usr));
		Print_square(p,WUZIQI_LENGTH);
		return Judge(p,NULL,x,y,1);
}

int Wait_him(int sockfd,char * data)
{
		int c=0;
		USR usr;
		SER ser;
		pthread_t talarm;
		if(read(sockfd,&ser,sizeof(ser))<=0)
		{
				printf("服务器未响应,请稍后再试\n");
				return -1;
		}
		if(ser.sig==GAME_OVER||ser.sig==GIVEUP)
		{
				if(ser.sig==GIVEUP)
						printf("对方已弃权\n");
				if(read(sockfd,&ser,sizeof(ser))<=0)
				{
						printf("服务器未响应,请稍后再试\n");
						return -1;
				}	
				if(ser.sig==AGAIN)
				{
						printf("对方发出再战请求,是否接受(y/n)\n");
						int alarm_time=10;
						pthread_create(&talarm,NULL,Alarm,&alarm_time);
						c=getchar();
						if(c==-1)
						{
								printf("选择超时,默认取消\n");
								usr.cmd=GAME_OVER;
						}
						if(c=='y')
						{
								getchar();
								pthread_cancel(talarm);
								printf("接受了再战请求,正在准备新对战\n");
								usr.cmd=AGAIN;
						}
						else if(c=='n')
						{
								getchar();
								pthread_cancel(talarm);
								printf("拒绝了再次对战\n");
								usr.cmd=GAME_OVER;
						}
						else
						{	
								if(c!=10)
										getchar();	
								pthread_cancel(talarm);
								printf("未知选项,默认取消\n");
								usr.cmd=GAME_OVER;
						}
						write(sockfd,&usr,sizeof(usr));
						if(c=='y')
						{
								if(order==1)
								{
										order=2;
										return 2;
								}
								if(order==2)
								{
										order=1;
										return 3;
								}
						}
						else 
								return -1;
				}
				if(ser.sig==GAME_OVER)
				{
						printf("对方放弃再战,游戏结束\n");
						return -1;
				}
		}
		else
		{
				strcpy(data,ser.data);
				return 0;
		}
}
int His_point(int sockfd,char * data,POINT * p)
{
		int c=0;
		USR usr;
		SER ser;
		int ret;
		int x=1,y=1;
		pthread_t talarm;
		x=(int)data[0];
		y=(int)data[1];
		Put_point(p,x,y,2);
		Print_square(p,WUZIQI_LENGTH);
		ret=Judge(p,NULL,x,y,1);
		if(ret==1)
		{
				printf("you lose 你输了!\n");
				usr.cmd=GAME_OVER;
				write(sockfd,&usr,sizeof(usr));
				printf("是否再战?(y/n)\n");
				int alarm_time=10;
				pthread_create(&talarm,NULL,Alarm,&alarm_time);
				c=getchar();
				if(c==-1)
				{
						printf("选择超时,默认取消\n");
						usr.cmd=GAME_OVER;
				}
				if(c=='y')
				{
						pthread_cancel(talarm);
						printf("选择了再战,等待对方回应...\n");
						usr.cmd=AGAIN;
				}
				else if(c=='n')
				{
						pthread_cancel(talarm);
						printf("取消了再次对战\n");
								usr.cmd=GAME_OVER;
				}
				else
				{		
						pthread_cancel(talarm);
						printf("未知选项,默认取消\n");
						usr.cmd=GAME_OVER;
				}
				write(sockfd,&usr,sizeof(usr));
				if(c=='y')
				{
						if(read(sockfd,&ser,sizeof(ser))<=0)
						{
								printf("服务器未响应,请稍后再试\n");
								return -1;
						}
						if(ser.sig==AGAIN)
						{
								printf("对方接受了再战请求,正在准备新对战\n");
								Remove_chess(p);
								if(order==1)
								{
										order=2;
										return 2;
								}
								if(order==2)
								{
										order=1;
										return 3;
								}
						}
				}
				return -1;
		}
		return 0;
}

int Judge_role(void)
{
		srand(time(NULL));
		return rand()%2;
}

int Set_point_offline(POINT * p,int * x,int * y)
{
		int c;
		while(1)
		{
				printf("请输入坐标((0,0)表示弃权)\n");
				printf("x:\n");
				scanf("%d",x);
				getchar();
				printf("y:\n");
				scanf("%d",y);
				getchar();
				if(*x>WUZIQI_LENGTH||*y>WUZIQI_LENGTH||*x<0||*y<0)
						printf("position wrong\n");
				else if(*x==0&&*y==0)
				{
						printf("确认弃权?(按回车确定,任意字符取消)\n");
						if(getchar()!=10)
						{
								getchar();
								continue;
						}
						printf("你弃权了\n");
						printf("再来一局?(y/n)\n");
						while(1)
						{
								c=getchar();
								if(c=='n')
								{
										getchar();
										printf("取消了再次对战\n");
										break;
								}
								else if(c=='y')
								{
										getchar();
										printf("选择了再来一局\n");
										break;
								}
						}
						if(c=='n')
								return -1;
						else if(c=='y')
						{	
								if(order==1)
								{
										order=2;
										return 2;
								}
								if(order==2)
								{
										order=1;
										return 3;
								}
						}
				}
				else if(Put_point(p,*x,*y,1)==-1)
						printf("该地址已有棋子\n");
				else
						break;
		}
		Print_square(p,WUZIQI_LENGTH);
		return Judge(p,NULL,*x,*y,1);
}

int Left_bot(POINT * work,int color)
{
		int count=0;
		int num=0;
		while(work!=NULL && num<4)
		{
				if(work->state==color)
						count++;
				num++;
				work=work->left;
		}
		return count;
}

int Right_bot(POINT * work,int color)
{
		int count=0;
		int num=0;
		while(work!=NULL && num<4)
		{
				if(work->state==color)
						count++;
				num++;
				work=work->right;
		}
		return count;
}

int Up_bot(POINT * work,int color)
{
		int count=0;
		int num=0;
		while(work!=NULL && num<4)
		{
				if(work->state==color)
						count++;
				num++;
				work=work->up;
		}
		return count;
}

int Down_bot(POINT * work,int color)
{
		int count=0;
		int num=0;
		while(work!=NULL && num<4)
		{
				if(work->state==color)
						count++;
				num++;
				work=work->down;
		}
		return count;
}

int Left_up_bot(POINT * work,int color)
{
		int count=0;
		int num=0;
		while(work!=NULL && num<4)
		{
				if(work->state==color)
						count++;
				num++;
				work=work->left;
				if(work!=NULL)
						work=work->up;
		}
		return count;
}

int Left_down_bot(POINT * work,int color)
{
		int count=0;
		int num=0;
		while(work!=NULL && num<4)
		{
				if(work->state==color)
						count++;
				num++;
				work=work->left;
				if(work!=NULL)
						work=work->down;
		}
		return count;
}

int Right_up_bot(POINT * work,int color)
{
		int count=0;
		int num=0;
		while(work!=NULL && num<4)
		{
				if(work->state==color)
						count++;
				num++;
				work=work->right;
				if(work!=NULL)
						work=work->up;
		}
		return count;
}

int Right_down_bot(POINT * work,int color)
{
		int count=0;
		int num=0;
		while(work!=NULL && num<4)
		{
				if(work->state==color)
						count++;
				num++;
				work=work->right;
				if(work!=NULL)
						work=work->down;
		}
		return count;
}

int Put_strategy(int l,int r,int u,int d,int lu,int ld,int ru, int rd,POINT ** t,POINT * work)
{
		int flag=0;
		POINT * temp=work;
		int i;
		int max=0;
		int count=0;
		if(l==4)
		{
				for(i=1;i<5 && work!=NULL;i++)
				{
						if(work->state==0)
						{
								work->state=2;
								temp=work;
								flag=1;
								break;
						}
						work=work->left;
				}
				if(flag==0)  //如果遍历到边框都没有下子，说明左边只有3个位置，那么右边第一个点必须堵上
				{
						if(temp->right)
						{
								if(temp->right->state==0)
								{
										temp=temp->right;
										temp->state=2;
										flag=1;
								}
						}
				}
		}
		if(flag==0 && r==4)
		{
				for(i=1;i<5 && work!=NULL;i++)
				{
						if(work->state==0)
						{
								work->state=2;
								temp=work;
								flag=1;
								break;
						}
						work=work->right;
				}
				if(flag==0)
				{
						if(temp->left)
						{
								if(temp->left->state==0)
								{
										temp=temp->left;
										temp->state=2;
										flag=1;
								}
						}
				}
		}
		if(flag==0 && u==4)
		{
				for(i=1;i<5 && work!=NULL;i++)
				{
						if(work->state==0)
						{
								work->state=2;
								temp=work;
								flag=1;
								break;
						}
						work=work->up;
				}
				if(flag==0)
				{
						if(temp->down)
						{
								if(temp->down->state==0)
								{
										temp=temp->down;
										temp->state=2;
										flag=1;
								}
						}
				}
		}
		if(flag==0 && d==4)
		{
				for(i=1;i<5 && work!=NULL;i++)
				{
						if(work->state==0)
						{
								work->state=2;
								temp=work;
								flag=1;
								break;
						}
						work=work->down;
				}
				if(flag==0)
				{
						if(temp->up)
						{
								if(temp->up->state==0)
								{
										temp=temp->up;
										temp->state=2;
										flag=1;
								}
						}
				}
		}
		if(flag==0 && lu==4)
		{
				for(i=1;i<5;i++)
				{
						if(work->state==0)
						{
								work->state=2;
								flag=1;
								temp=work;
								break;
						}
						work=work->left;
						if(work!=NULL)
								work=work->up;
				}
				if(flag==0)
				{
						if(temp->right)
						{
								if(temp->right->down)
								{
										if(temp->right->down->state==0)
										{
												temp=temp->right->down;
												temp->state=2;
												flag=1;
										}
								}
						}
				}
		}
		if(flag==0 && ld==4)
		{
				for(i=1;i<5;i++)
				{
						if(work->state==0)
						{
								work->state=2;
								temp=work;
								flag=1;
								break;
						}
						work=work->left;
						if(work!=NULL)
								work=work->down;
				}
				if(flag==0)
				{
						if(temp->right)
						{
								if(temp->right->up)
								{
										if(temp->right->up->state==0)
										{
												temp=temp->right->up;
												temp->state=2;
												flag=1;
										}
								}
						}
				}
		}
		if(flag==0 && ru==4)
		{
				for(i=1;i<5;i++)
				{
						if(work->state==0)
						{
								work->state=2;
								temp=work;
								flag=1;
								break;
						}
						work=work->right;
						if(work!=NULL)
								work=work->up;
				}
				if(flag==0)
				{
						if(temp->left)
						{
								if(temp->left->down)
								{
										if(temp->left->down->state==0)
										{
												temp=temp->left->down;
												temp->state=2;
												flag=1;
										}
								}
						}
				}
		}
		if(flag==0 && rd==4)
		{
				for(i=1;i<5;i++)
				{
						if(work->state==0)
						{
								work->state=2;
								temp=work;
								flag=1;
								break;
						}
						work=work->right;
						if(work!=NULL)
								work=work->down;
				}
				if(flag==0)
				{
						if(temp->left)
						{
								if(temp->left->up)
								{
										if(temp->left->up->state==0)
										{
												temp=temp->left->up;
												temp->state=2;
												flag=1;
										}
								}
						}
				}
		}
		if(flag==0 && l==3)
		{
				count=0;
				max=0;
				for(i=1;i<5;i++)
				{
						if(work==NULL)
								break;
						if(work->state==1)
						{
								count++;
								max=max>count?max:count;
						}
						else 
								count=0;
						work=work->left;
				}
				if(max==3)  //如果连续的三个
				{
						work=temp;
						for(i=1;i<4;i++)
								work=work->left;
						if(work)
						{
								if(temp->right)
								{
										if(work->state==0 && temp->right->state==0)
										{
												work->state=2;
												temp=work;
												flag=1;										
										}
										else if(temp->right->state==1)
										{
												if(temp->right->right)
												{
														if(temp->right->right->state==0 && work->state==2)
														{
																temp=temp->right->right;
																temp->state=2;
																flag=1;
														}
														else if(temp->right->right->state==2 && work->state==0)
														{
																work->state=2;
																temp=work;
																flag=1;
														}
												}
										}
								}
						}
				}
				else if(max==2)  //如果连续的两个
				{
						work=temp;
						if(work->left->state!=1)
						{
								if(work->left->left->state==1)
								{
										if(work->left->state==0)
										{
												work->left->state=2;
												temp=work->left;
												flag=1;
										}
								}
						}
						else if(work->left->left->left->state==1)
						{
								if(work->left->left->state==0)
								{
										work->left->left->state=2;
										temp=work->left->left;
										flag=1;
								}
						}
				}
		}
		if(flag==0 && r==3)
		{
				count=0;
				max=0;
				for(i=1;i<5;i++)
				{
						if(work==NULL)
								break;
						if(work->state==1)
						{
								count++;
								max=max>count?max:count;
						}
						else 
								count=0;
						work=work->right;
				}
				if(max==3)  //如果连续的三个
				{
						work=temp;
						for(i=1;i<4;i++)
								work=work->right;
						if(work)
						{
								if(temp->left)
								{
										if(work->state==0 && temp->left->state==0)
										{
												work->state=2;
												temp=work;
												flag=1;										
										}
										else if(temp->left->state==1)
										{
												if(temp->left->left)
												{
														if(temp->left->left->state==0 && work->state==2)
														{
																temp=temp->left->left;
																temp->state=2;
																flag=1;
														}
														else if(temp->left->left->state==2 && work->state==0)
														{
																work->state=2;
																temp=work;
																flag=1;
														}
												}
										}
								}
						}
				}
				else if(max==2)  //如果连续的两个
				{
						work=temp;
						if(work->right->state!=1)
						{
								if(work->right->right->state==1)
								{
										if(work->right->state==0)
										{
												work->right->state=2;
												temp=work->right;
												flag=1;
										}
								}
						}
						else if(work->right->right->right->state==1)
						{
								if(work->right->right->state==0)
								{
										work->right->right->state=2;
										temp=work->right->right;
										flag=1;
								}
						}
				}
		}
		if(flag==0 && u==3)
		{
				count=0;
				max=0;
				for(i=1;i<5;i++)
				{
						if(work==NULL)
								break;
						if(work->state==1)
						{
								count++;
								max=max>count?max:count;
						}
						else
								count=0;
						work=work->up;
				}
				if(max==3)  //如果连续的三个
				{
						work=temp;
						for(i=1;i<4;i++)
								work=work->up;
						if(work)
						{
								if(temp->down)
								{
										if(work->state==0 && temp->down->state==0)
										{
												work->state=2;
												temp=work;
												flag=1;										
										}
										else if(temp->down->state==1)
										{
												if(temp->down->down)
												{
														if(temp->down->down->state==0 && work->state==2)
														{
																temp=temp->down->down;
																temp->state=2;
																flag=1;
														}
														else if(temp->down->down->state==2 && work->state==0)
														{
																work->state=2;
																temp=work;
																flag=1;
														}
												}
										}
								}
						}
				}
				else if(max==2)  //如果连续的两个
				{
						work=temp;
						if(work->up->state!=1)
						{
								if(work->up->up->state==1)
								{
										if(work->up->state==0)
										{
												work->up->state=2;
												temp=work->up;
												flag=1;
										}
								}
						}
						else if(work->up->up->up->state==1)
						{
								if(work->up->up->state==0)
								{
										work->up->up->state=2;
										temp=work->up->up;
										flag=1;
								}
						}
				}
		}
		if(flag==0 && d==3)
		{
				count=0;
				max=0;
				for(i=0;i<5;i++)
				{
						if(work==NULL)
								break;
						if(work->state==1)
						{
								count++;
								max=max>count?max:count;
						}
						else	
								count=0;
						work=work->down;
				}
				if(max==3)  //如果连续的三个
				{
						work=temp;
						for(i=1;i<4;i++)
								work=work->down;
						if(work)
						{
								if(temp->up)
								{
										if(work->state==0 && temp->up->state==0)
										{
												work->state=2;
												temp=work;
												flag=1;										
										}
										else if(temp->up->state==1)
										{
												if(temp->up->up)
												{
														if(temp->up->up->state==0 && work->state==2)
														{
																temp=temp->up->up;
																temp->state=2;
																flag=1;
														}
														else if(temp->up->up->state==2 && work->state==0)
														{
																work->state=2;
																temp=work;
																flag=1;
														}
												}
										}
								}
						}
				}
				else if(max==2)  //如果连续的两个
				{
						work=temp;
						if(work->down->state!=1)
						{
								if(work->down->down->state==1)
								{
										if(work->down->state==0)
										{
												work->down->state=2;
												temp=work->down;
												flag=1;
										}
								}
						}
						else if(work->down->down->down->state==1)
						{
								if(work->down->down->state==0)
								{
										work->down->down->state=2;
										temp=work->down->down;
										flag=1;
								}
						}
				}
		}
		if(flag==0 && lu==3)
		{
				count=0;
				max=0;
				for(i=1;i<5;i++)
				{
						if(work==NULL)
								break;
						if(work->state==1)
						{
								count++;
								max=max>count?max:count;
						}
						else
								count=0;
						work=work->left;
						if(work!=NULL)
								work=work->up;
				}
				if(max==3)  //如果连续的三个
				{
						work=temp;
						for(i=1;i<4;i++)
						{
								work=work->left;
								if(work!=NULL)
										work=work->up;
						}
						if(work)
						{
								if(temp->right)
								{
										if(temp->right->down)
										{
												if(work->state==0 && temp->right->down->state==0)
												{
														work->state=2;
														temp=work;
														flag=1;										
												}
												else if(temp->right->down->state==1)
												{
														if(temp->right->down->right)
														{
																if(temp->right->down->right->down)
																{
																		if(temp->right->down->right->down->state==0 && work->state==2)
																		{
																				temp=temp->right->down->right->down;
																				temp->state=2;
																				flag=1;
																		}
																		else if(temp->right->down->right->down->state==2 && work->state==0)
																		{
																				work->state=2;
																				temp=work;
																				flag=1;
																		}
																}
														}
												}
										}
								}
						}
				}
				else if(max==2)  //如果连续的两个
				{
						work=temp;
						if(work->left->up->state!=1)
						{
								if(work->left->up->left->up->state==1)
								{
										if(work->left->up->state==0)
										{
												work->left->up->state=2;
												temp=work->left->up;
												flag=1;
										}
								}
						}
						else if(work->left->up->left->up->left->up->state==1)
						{
								if(work->left->up->left->up->state==0)
								{
										work->left->up->left->up->state=2;
										temp=work->left->up->left->up;
										flag=1;
								}
						}
				}
		}
		if(flag==0 && ld==3)
		{
				count=0;
				max=0;
				for(i=1;i<5;i++)
				{
						if(work==NULL)
								break;
						if(work->state==1)
						{
								count++;
								max=max>count?max:count;
						}
						else 
								count=0;
						work=work->left;
						if(work!=NULL)
								work=work->down;
				}
				if(max==3)  //如果连续的三个
				{
						work=temp;
						for(i=1;i<4;i++)
						{
								work=work->left;
								if(work!=NULL)
										work=work->down;
						}
						if(work)
						{
								if(temp->right)
								{
										if(temp->right->up)
										{
												if(work->state==0 && temp->right->up->state==0)
												{
														work->state=2;
														temp=work;
														flag=1;										
												}
												else if(temp->right->up->state==1)
												{
														if(temp->right->up->right)
														{
																if(temp->right->up->right->up)
																{
																		if(temp->right->up->right->up->state==0 && work->state==2)
																		{
																				temp=temp->right->up->right->up;
																				temp->state=2;
																				flag=1;
																		}
																		else if(temp->right->up->right->up->state==2 && work->state==0)
																		{
																				work->state=2;
																				temp=work;
																				flag=1;
																		}
																}
														}
												}
										}
								}
						}
				}
				else if(max==2)  //如果连续的两个
				{
						work=temp;
						if(work->left->down->state!=1)
						{
								if(work->left->down->left->down->state==1)
								{
										if(work->left->down->state==0)
										{
												work->left->down->state=2;
												temp=work->left->down;
												flag=1;
										}
								}
						}
						else if(work->left->down->left->down->left->down->state==1)
						{
								if(work->left->down->left->down->state==0)
								{
										work->left->down->left->down->state=2;
										temp=work->left->down->left->down;
										flag=1;
								}
						}
				}
		}
		if(flag==0 && ru==3)
		{
				count=0;
				max=0;
				for(i=1;i<5;i++)
				{
						if(work==NULL)
								break;
						if(work->state==1)
						{
								count++;
								max=max>count?max:count;
						}
						else 
								count=0;
						work=work->right;
						if(work!=NULL)
								work=work->up;
				}
				if(max==3)  //如果连续的三个
				{
						work=temp;
						for(i=1;i<4;i++)
						{
								work=work->right;
								if(work!=NULL)
										work=work->up;
						}
						if(work)
						{
								if(temp->left)
								{
										if(temp->right->down)
										{
												if(work->state==0 && temp->left->down->state==0)
												{
														work->state=2;
														temp=work;
														flag=1;										
												}
												else if(temp->left->down->state==1)
												{
														if(temp->left->down->left)
														{
																if(temp->left->down->left->down)
																{
																		if(temp->left->down->left->down->state==0 && work->state==2)
																		{
																				temp=temp->left->down->left->down;
																				temp->state=2;
																				flag=1;
																		}
																		else if(temp->left->down->left->down->state==2 && work->state==0)
																		{
																				work->state=2;
																				temp=work;
																				flag=1;
																		}
																}
														}
												}
										}
								}
						}
				}
				else if(max==2)  //如果连续的两个
				{
						work=temp;
						if(work->right->up->state!=1)
						{
								if(work->right->up->right->up->state==1)
								{
										if(work->right->up->state==0)
										{
												work->right->up->state=2;
												temp=work->right->up;
												flag=1;
										}
								}
						}
						else if(work->right->up->right->up->right->up->state==1)
						{
								if(work->right->up->right->up->state==0)
								{
										work->right->up->right->up->state=2;
										temp=work->right->up->right->up;
										flag=1;
								}
						}
				}
		}
		if(flag==0 && rd==3)
		{
				count=0;
				max=0;
				for(i=1;i<5;i++)
				{
						if(work==NULL)
								break;
						if(work->state==1)
						{
								count++;
								max=max>count?max:count;
						}
						else 
								count=0;
						work=work->right;
						if(work!=NULL)
								work=work->down;
				}
				if(max==3)  //如果连续的三个
				{
						work=temp;
						for(i=1;i<4;i++)
						{
								work=work->right;
								if(work!=NULL)
										work=work->down;
						}
						if(work)
						{
								if(temp->left)
								{
										if(temp->right->up)
										{
												if(work->state==0 && temp->left->up->state==0)
												{
														work->state=2;
														temp=work;
														flag=1;										
												}
												else if(temp->left->up->state==1)
												{
														if(temp->left->up->left)
														{
																if(temp->left->up->left->up)
																{
																		if(temp->left->up->left->up->state==0 && work->state==2)
																		{
																				temp=temp->left->up->left->up;
																				temp->state=2;
																				flag=1;
																		}
																		else if(temp->left->up->left->up->state==2 && work->state==0)
																		{
																				work->state=2;
																				temp=work;
																				flag=1;
																		}
																}
														}
												}
										}
								}
						}
				}
				else if(max==2)  //如果连续的两个
				{
						work=temp;
						if(work->right->down->state!=1)
						{
								if(work->right->down->right->down->state==1)
								{
										if(work->right->down->state==0)
										{
												work->right->down->state=2;
												temp=work->right->down;
												flag=1;
										}
								}
						}
						else if(work->right->down->right->down->right->down->state==1)
						{
								if(work->right->down->right->down->state==0)
								{
										work->right->down->right->down->state=2;
										temp=work->right->down->right->down;
										flag=1;
								}
						}
				}
		}
		if(flag==0 && l==2)
		{
				if(work->left->state==1)
				{
						if(work->right)
						{
								if(work->right->state==1)
								{
										if(work->left->left->state==0)
										{
												work->left->left->state=2;
												temp=work->left->left;
												flag=1;
										}
								}
						}
				}
				if(work->left->left->state==1)
				{
						if(work->right)
						{
								if(work->right->state==1)
								{
										if(work->left->state==0)
										{
												work->left->state=2;
												temp=work->left;
												flag=1;
										}
								}
						}
				}
		}
		if(flag==0 && r==2)
		{
				if(work->right->state==1)
				{
						if(work->left)
						{
								if(work->left->state==1)
								{
										if(work->right->right->state==0)
										{
												work->right->right->state=2;
												temp=work->right->right;
												flag=1;
										}
								}
						}
				}
				if(work->right->right->state==1)
				{
						if(work->left)
						{
								if(work->left->state==1)
								{
										if(work->right->state==0)
										{
												work->right->state=2;
												temp=work->right;
												flag=1;
										}
								}
						}
				}
		}
		if(flag==0 && u==2)
		{
				if(work->up->state==1)
				{
						if(work->down)
						{
								if(work->down->state==1)
								{
										if(work->up->up->state==0)
										{
												work->up->up->state=2;
												temp=work->up->up;
												flag=1;
										}
								}
						}
				}
				if(work->up->up->state==1)
				{
						if(work->down)
						{
								if(work->down->state==1)
								{
										if(work->up->state==0)
										{
												work->up->state=2;
												temp=work->up;
												flag=1;
										}
								}
						}
				}
		}
		if(flag==0 && d==2)
		{
				if(work->down->state==1)
				{
						if(work->up)
						{
								if(work->up->state==1)
								{
										if(work->down->down->state==0)
										{
												work->down->down->state=2;
												temp=work->down->down;
												flag=1;
										}
								}
						}
				}
				if(work->down->down->state==1)
				{
						if(work->up)
						{
								if(work->up->state==1)
								{
										if(work->down->state==0)
										{
												work->down->state=2;
												temp=work->down;
												flag=1;
										}
								}
						}
				}
		}
		if(flag==0 && lu==2)
		{
				if(work->left->up->state==1)
				{
						if(work->right)
						{
								if(work->right->down)
								{
										if(work->right->down->state==1)
										{
												if(work->left->up->left->up->state==0)
												{
														work->left->up->left->up->state=2;
														temp=work->left->up->left->up;
														flag=1;
												}
										}
								}
						}
				}
				if(work->left->up->left->up->state==1)
				{
						if(work->right)
						{
								if(work->right->down)
								{
										if(work->right->down->state==1)
										{
												if(work->left->up->state==0)
												{
														work->left->up->state=2;
														temp=work->left->up;
														flag=1;
												}
										}
								}
						}
				}
		}
		if(flag==0 && ld==2)
		{
				if(work->left->down->state==1)
				{
						if(work->right)
						{
								if(work->right->up)
								{
										if(work->right->up->state==1)
										{
												if(work->left->down->left->down->state==0)
												{
														work->left->down->left->down->state=2;
														temp=work->left->down->left->down;
														flag=1;
												}
										}
								}
						}
				}
				if(work->left->down->left->down->state==1)
				{
						if(work->right)
						{
								if(work->right->up)
								{
										if(work->right->up->state==1)
										{
												if(work->left->down->state==0)
												{
														work->left->down->state=2;
														temp=work->left->down;
														flag=1;
												}
										}
								}
						}
				}
		}
		if(flag==0 && ru==2)
		{
				if(work->right->up->state==1)
				{
						if(work->left)
						{
								if(work->left->down)
								{
										if(work->left->down->state==1)
										{
												if(work->right->up->right->up->state==0)
												{
														work->right->up->right->up->state=2;
														temp=work->right->up->right->up;
														flag=1;
												}
										}
								}
						}
				}
				if(work->right->up->right->up->state==1)
				{
						if(work->left)
						{
								if(work->left->down)
								{
										if(work->left->down->state==1)
										{
												if(work->right->up->state==0)
												{
														work->right->up->state=2;
														temp=work->right->up;
														flag=1;
												}
										}
								}
						}
				}
		}
		if(flag==0 && rd==2)
		{
				if(work->right->down->state==1)
				{
						if(work->left)
						{
								if(work->left->up)
								{
										if(work->left->up->state==1)
										{
												if(work->right->down->right->down->state==0)
												{
														work->right->down->right->down->state=2;
														temp=work->right->down->right->down;
														flag=1;
												}
										}
								}
						}
				}
				if(work->right->down->right->down->state==1)
				{
						if(work->left)
						{
								if(work->left->up)
								{
										if(work->left->up->state==1)
										{
												if(work->right->down->state==0)
												{
														work->right->down->state=2;
														temp=work->right->down;
														flag=1;
												}
										}
								}
						}
				}
		}
		*t=temp;
		return flag;
}

int Put_strategy2(POINT ** t,POINT * work)
{
		int ret=0;
		POINT * temp;
		if(work->left)
		{
				if(work->left->state==0)
				{
						if(work->left->left)
						{
								if(work->left->left->state!=1)
								{
										work->left->state=2;
										temp=work->left;
										ret=1;
								}
						}
				}
		}
		if(ret==0 && work->right)
		{
				if(work->right->state==0)
				{
						if(work->right->right)
						{
								if(work->right->right->state!=1)
								{
										work->right->state=2;
										temp=work->right;
										ret=1;
								}
						}
				}
		}
		if(ret==0 && work->up)
		{
				if(work->up->state==0)
				{
						if(work->up->up)
						{
								if(work->up->up->state!=1)
								{
										work->up->state=2;
										temp=work->up;
										ret=1;
								}
						}
				}
		}
		if(ret==0 && work->down)
		{
				if(work->down->state==0)
				{
						if(work->down->down)
						{
								if(work->down->down->state!=1)
								{
										work->down->state=2;
										temp=work->down;
										ret=1;
								}
						}
				}
		}
		if(ret==0 && work->left)
		{
				if(work->left->up)
				{
						if(work->left->up->state==0)
						{
								if(work->left->up->left)
								{
										if(work->left->up->left->up)
										{
												if(work->left->up->left->up->state!=1)
												{
														work->left->up->state=2;
														temp=work->left->up;
														ret=1;
												}
										}
								}
						}
				}
				if(ret==0 && work->left->down)
				{
						if(work->left->down->state==0)
						{
								if(work->left->down->left)
								{
										if(work->left->down->left->down)
										{
												if(work->left->down->left->down->state!=1)
												{
														work->left->down->state=2;
														temp=work->left->down;
														ret=1;
												}
										}
								}
						}
				}
		}
		if(ret==0 && work->right)
		{
				if(work->right->up)
				{
						if(work->right->up->state==0)
						{
								if(work->right->up->right)
								{
										if(work->right->up->right->up)
										{
												if(work->right->up->right->up->state!=1)
												{
														work->right->up->state=2;
														temp=work->right->up;
														ret=1;
												}
										}
								}
						}
				}
				if(ret==0 && work->right->down)
				{
						if(work->right->down->state==0)
						{
								if(work->right->down->left)
								{
										if(work->right->down->right->down)
										{
												if(work->right->down->right->down->state!=1)
												{
														work->right->down->state=2;
														temp=work->right->down;
														ret=1;
												}
										}
								}
						}
				}
		}
		*t=temp;
		return ret;
}

int Bot_turn(POINT * p,int x,int y)
{
		POINT * work,* temp;
		static POINT * bot_last_point=NULL;
		work=p->right;
		if(x==0 && y==0)  //说明bot先手走棋
		{
				x=y=WUZIQI_LENGTH/2;
				while(1)
				{
						if(work->pos[0]==x && work->pos[1]==y)
								break;
						work=work->right;
				}
				work->state=2;
				bot_last_point=work;
				return 0;
		}
		while(1) //find the point
		{
				if(work->pos[0]==x&&work->pos[1]==y)
						break;
				work=work->right;
		}
		temp=work;
		int ret;
		int l=Left_bot(work,1);
		int r=Right_bot(work,1);
		int u=Up_bot(work,1);
		int d=Down_bot(work,1);
		int lu=Left_up_bot(work,1);
		int ld=Left_down_bot(work,1);
		int ru=Right_up_bot(work,1);
		int rd=Right_down_bot(work,1);
		ret=Put_strategy(l,r,u,d,lu,ld,ru,rd,&temp,work);
		if(ret==1)
		{
				bot_last_point=temp;
				return Judge(p,temp,0,0,2);
		}
		//防御结束，开始进攻
		work=bot_last_point; //获得自己上一次下的子的位置
		if(work==NULL)  //如果机器没有找到上一次自己下的点，说明玩家是第一次下子，那么就下在玩家左\右
		{
				if(temp->right->state==0)
				{
						temp=temp->right;
						temp->state=2;
				}
				else if(temp->left->state==0)
				{
						temp=temp->left;
						temp->state=2;
				}
				bot_last_point=temp;
				return Judge(p,temp,0,0,2);
		}
		else
		{
				l=Left_bot(work,2);
				r=Right_bot(work,2);
				u=Up_bot(work,2);
				d=Down_bot(work,2);
				lu=Left_up_bot(work,2);
				ld=Left_down_bot(work,2);
				ru=Right_up_bot(work,2);
				rd=Right_down_bot(work,2);
				ret=Put_strategy(l,r,u,d,lu,ld,ru,rd,&temp,work);
				if(ret==1)
				{
						bot_last_point=temp;
						return Judge(p,temp,0,0,2);
				}
				else
				{
						ret=Put_strategy2(&temp,work);
						if(ret==1)
						{
								bot_last_point=temp;
								return Judge(p,temp,0,0,2);
						}
						else 
						{
								ret=Put_strategy2(&temp,temp);
								if(ret==1)
								{
										bot_last_point=temp;
										return Judge(p,temp,0,0,2);
								}
								else
								{
										printf("bot认输。。。\n");
										return 2;
								}
						}
				}
		}
}

void Wuziqi_play_with_bot(void)
{
		POINT * p;
		int x=0,y=0;
		int ret;
		int c;
		USR usr;
		SER ser;
		char data[8];
		if(Init_game(&p,0)==-1)
				return;
		if(Judge_role())
		{
				order=2;
				goto bot;
		}
		order=1;
		while(1)
		{
man:
				Print_square(p,WUZIQI_LENGTH);
				printf("请出子:\n");
				ret=Set_point_offline(p,&x,&y);
				if(ret==1)
				{
						printf("you win 你赢了!\n");
						Remove_chess(p);
						while(1)
						{
								printf("再来一局?(y/n)\n");
								if((c=getchar())=='y')
								{
										getchar();
										if(order==1)
										{
												order=2;
												goto bot;
										}
										else
										{
												order=1;
												goto man;
										}
								}
								else if(c=='n')
								{
										Clean(p);
										return;
								}
						}
				}
				else if(ret==2)
				{
						Remove_chess(p);
						goto man;
				}
				else if(ret==3)
				{
						Remove_chess(p);
						goto bot;
				}
				else if(ret==-1)
				{
						Clean(p);
						return;
				}
bot:
				ret=Bot_turn(p,x,y);
				Print_square(p,WUZIQI_LENGTH);
				if(ret==1)
				{
						printf("you lose 你输了!\n");
						Remove_chess(p);
						while(1)
						{
								printf("再来一局?(y/n)\n");
								if((c=getchar())=='y')
								{
										getchar();
										if(order==1)
										{
												order=2;
												Remove_chess(p);
												goto bot;
										}
										else
										{
												order=1;
												Remove_chess(p);
												goto man;
										}
								}
								else if(c=='n')
								{
										Clean(p);
										return;
								}
						}
				}
				else if(ret==2)
				{
						printf("you win 你赢了!\n");
						Remove_chess(p);
						while(1)
						{
								printf("再来一局?(y/n)\n");
								if((c=getchar())=='y')
								{
										getchar();
										if(order==1)
										{
												order=2;
												goto bot;
										}
										else
										{
												order=1;
												goto man;
										}
								}
								else if(c=='n')
								{
										Clean(p);
										return;
								}
						}
				}
		}
}
//主函数控制轮流下棋的流程
int Wuziqi_game_body(int sockfd)
{
		POINT * p;
		int x=1,y=1;
		int ret;
		USR usr;
		SER ser;
		char data[8];
		if(Init_game(&p,sockfd)==-1)
				return 0;
		if(read(sockfd,&ser,sizeof(ser))<=0)
		{
				printf("服务器未响应,请稍后再试\n");
				return 0;
		}
		if(ser.sig==GAME_OVER)
		{
				printf("对方初始化失败,本次对战取消\n");
				Clean(p);
				return 0;
		}
		Print_square(p,WUZIQI_LENGTH);
		printf("=====>");
		if(ser.sig==GAME_READY)
		{	
				printf("等待服务器仲裁出子顺序\n");
				if(read(sockfd,&ser,sizeof(ser))<=0)
				{
						printf("服务器未响应,请稍后再试\n");
						Clean(p);
						return 0;
				}
				if(ser.sig==FIRST)
				{
						printf("取得：先手\n");
						order=1;
						goto myturn;
				}
				if(ser.sig==SECOND)
				{
						printf("取得：后手\n");
						order=2;
						goto histurn;
				}	
		}
		while(1)
		{				
myturn:
				ret=Set_point(sockfd,p);
				if(ret==1)
						printf("you win 你赢了!\n");
				else if(ret==2)
				{
						Remove_chess(p);
						goto histurn;
				}
				else if(ret==3)
				{
						Remove_chess(p);
						goto myturn;
				}
				else if(ret==-1)
				{
						Clean(p);
						return 0;
				}
histurn:
				printf("等待对手...\n");
				ret=Wait_him(sockfd,data);
				if(ret==-1)
				{
						Clean(p);
						return 0;
				}
				if(ret==2)
						goto histurn;
				if(ret==3)
						goto myturn;
				ret=His_point(sockfd,data,p);
				if(ret==-1)
				{
						Clean(p);
						return 0;
				}
				if(ret==2)
						goto histurn;
				if(ret==3)
						goto myturn;
		}
}
