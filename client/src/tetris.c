#include"client_head.h"
#include"client_game.h"
#include"client_tetris.h"
extern int getch(void);
extern int free_flag;
extern pthread_mutex_t free_lock;
extern struct termios tm,tm_old;
extern int game_state;

//菜单
void Print_head(void)
{
		printf("============\n");
		printf("|1.开始游戏|\n");
		printf("|2.疯狂模式|\n");
		printf("|3.变态模式|\n");
		printf("|4.退出游戏|\n");
		printf("============\n");
}
//记录分数
void Tetris_score(int flag)
{
		if(flag>0)
				printf("分数：%d\n",score=score+10*flag);
		else
				printf("分数：%d\n",score);
}
//malloc为了在游戏结束可以释放
int Malloc_surface()
{
	surface=(int (*)[WIDTH])malloc(HEIGHT*WIDTH*sizeof(int));
	if(surface==NULL)
	{
			printf("游戏初始化失败\n");
			return 1;
	}
	return 0;
}
//创建下一个图形
int Create_shape(void)
{
		int i;
		srand(time(NULL));
		i=rand()%7+1;
		return 6;
}
//改变需要改变的某点
void Change_point(int x,int y,int stat)
{
		surface[y][x]=stat;
}
//打印所有点
int * Print_point()
{
		int i,j,k=0;
		system("clear");
		if(mode==INVISIBLE)
		{
				for(i=-2;i<WIDTH-1;i++)
				{
						if(i<=9)
								printf(" ");
						else if(i>9)
								printf("%d",i/10);
				}
				printf("\n");
				for(i=-2;i<WIDTH-1;i++)
				{
						if(i<0)
								printf(" ");
						else if(i>9)
								printf("%d",i%10);
						else
								printf("%d",i);
				}
				printf("\n");
		}
		for(i=0;i<HEIGHT;i++)
		{
				if(mode==INVISIBLE)
				{
						printf("%2d",i);
				}
				for(j=0;j<WIDTH;j++)
				{
						if(i==0||i==HEIGHT-1)
						{
								printf("-");
						}
						else
						{
								if(j==0||j==WIDTH-1)
								{
										printf("|");
								}
								else
								{
										if(surface[i][j]==0)
										{
												printf(" ");
										}
										else if(surface[i][j]==1)
										{
												printf("O");
										}
										else if(surface[i][j]==2)
										{
												if(mode!=INVISIBLE)
												{
														printf("O");
												}
												else 
														printf(" ");
												*(pointp+(k++))=j;
												*(pointp+(k++))=i;
										}
								}
						}
				}
				switch(i)
				{
						case 0: printf("      下一个图形");
								break;
						case 1: printf("        -------");
								break;
						case 2: printf("        |     |");
								break;
						case 3: switch(next_shape)
								{
										case 1:
										case 6:
										case 2: printf("        |  O  |");
												break;
										case 3:
										case 4: 
										case 7:
										case 5: printf("        |     |");
												break;
								}
								break;
						case 4: switch(next_shape)
								{
										case 1: 
										case 2:
										case 6:
										case 3:	printf("        |  O  |");
												break;
										case 4:
										case 5: printf("        | OO  |");
												break;
										case 7: printf("        |  OO |");
												break;
								}			
								break;
						case 5:	switch(next_shape)
								{
										case 1: printf("        |  O  |");
												break;
										case 2: printf("        |  OO |");
												break;
										case 3: printf("        | OOO |");
												break;
										case 4: printf("        | OO  |");
												break;
										case 5: printf("        |  OO |");
												break;
										case 6: printf("        | OO  |");
												break;
										case 7: printf("        | OO  |");
												break;
								}
								break;
						case 6: switch(next_shape)
								{
										case 1: printf("        |  O  |");
												break;
										case 2:
										case 3: 
										case 4:
										case 6:
										case 7:
										case 5: printf("        |     |");
												break;
								}
								break;
						case 7: printf("        |     |");
								break;
						case 8: printf("        -------");
								break;
						default:break;
				}
				printf("\n");
		}
		if(mode==INVISIBLE)
		{
				printf("坐标：");
				for(i=0;i<8;i++)
				{
						printf("%d,",pointp[i]);
				}
				printf("\n");
		}
		return;
}
void Change_shape(void)
{
		switch(current_shape)
		{
				case 1: switch(enter_count%2)
						{
								case 1: if(pointp[0]==1)
										{
												if(surface[pointp[3]][pointp[2]+1]!=1 && surface[pointp[3]][pointp[2]+2]!=1 && surface[pointp[3]][pointp[2]+3]!=1)
												{
														Change_point(pointp[2]+1,pointp[3],2);
														Change_point(pointp[2]+2,pointp[3],2);
														Change_point(pointp[2]+3,pointp[3],2);
														Change_point(pointp[0],pointp[1],0);
														Change_point(pointp[4],pointp[5],0);
														Change_point(pointp[6],pointp[7],0);
												}
										}
										else if(pointp[0]==WIDTH-3)
										{
												if(surface[pointp[3]][pointp[2]-1]!=1 && surface[pointp[3]][pointp[2]-2]!=1 && surface[pointp[3]][pointp[2]+1]!=1)
												{
														Change_point(pointp[2]-1,pointp[3],2);
														Change_point(pointp[2]-2,pointp[3],2);
														Change_point(pointp[2]+1,pointp[3],2);
														Change_point(pointp[0],pointp[1],0);
														Change_point(pointp[4],pointp[5],0);
														Change_point(pointp[6],pointp[7],0);
												}
										}
										else if(pointp[0]==WIDTH-2)
										{
												if(surface[pointp[3]][pointp[2]-1]!=1 && surface[pointp[3]][pointp[2]-2]!=1 && surface[pointp[3]][pointp[2]-3]!=1)
												{
														Change_point(pointp[2]-1,pointp[3],2);
														Change_point(pointp[2]-2,pointp[3],2);
														Change_point(pointp[2]-3,pointp[3],2);
														Change_point(pointp[0],pointp[1],0);
														Change_point(pointp[4],pointp[5],0);
														Change_point(pointp[6],pointp[7],0);
												}
										}
										else if(surface[pointp[3]][pointp[2]-1]!=1 && surface[pointp[3]][pointp[2+1]]!=1 && surface[pointp[3]][pointp[2]+2]!=1)
										{
												Change_point(pointp[2]-1,pointp[3],2);
												Change_point(pointp[2]+1,pointp[3],2);
												Change_point(pointp[2]+2,pointp[3],2);
												Change_point(pointp[0],pointp[1],0);
												Change_point(pointp[4],pointp[5],0);
												Change_point(pointp[6],pointp[7],0);
										}
										enter_count++;
										Print_point();
										Tetris_score(0);
										return;

								case 0:	if(surface[pointp[3]-1][pointp[2]]!=1 && surface[pointp[3]+1][pointp[2]]!=1 && surface[pointp[3]+2][pointp[2]]!=1)
										{
												Change_point(pointp[2],pointp[3]-1,2);
												Change_point(pointp[2],pointp[3]+1,2);
												Change_point(pointp[2],pointp[3]+2,2);
												Change_point(pointp[0],pointp[1],0);
												Change_point(pointp[4],pointp[5],0);
												Change_point(pointp[6],pointp[7],0);
										}	
										enter_count++;
										Print_point();
										Tetris_score(0);
										return;
						}	
				case 2: switch(enter_count%4)
						{
								case 1: if(pointp[0]==1)
										{
												if(surface[pointp[3]][pointp[2]+1]!=1 && surface[pointp[3]][pointp[2]+2]!=1)
												{
														Change_point(pointp[2]+1,pointp[3],2);
														Change_point(pointp[2]+2,pointp[3],2);
														Change_point(pointp[6],pointp[7],0);
														Change_point(pointp[0],pointp[1],0);
												}
										}
										else if(surface[pointp[3]][pointp[2]-1]!=1 && surface[pointp[3]][pointp[2]+1]!=1 && surface[pointp[5]][pointp[4]-1]!=1)
										{
												Change_point(pointp[2]-1,pointp[3],2);
												Change_point(pointp[2]+1,pointp[3],2);
												Change_point(pointp[4]-1,pointp[5],2);
												Change_point(pointp[0],pointp[1],0);
												Change_point(pointp[4],pointp[5],0);
												Change_point(pointp[6],pointp[7],0);
										}
										enter_count++;
										Print_point();
										Tetris_score(0);
										return;
								case 2: if(surface[pointp[1]-1][pointp[0]]!=1 && surface[pointp[3]-1][pointp[2]]!=1 && surface[pointp[3]+1][pointp[2]]!=1)
										{
												Change_point(pointp[0],pointp[1]-1,2);
												Change_point(pointp[2],pointp[3]-1,2);
												Change_point(pointp[2],pointp[3]+1,2);
												Change_point(pointp[0],pointp[1],0);
												Change_point(pointp[4],pointp[5],0);
												Change_point(pointp[6],pointp[7],0);
										}
										enter_count++;
										Print_point();
										Tetris_score(0);
										return;
								case 3:	if(pointp[2]==WIDTH-2)
										{
												if(surface[pointp[5]][pointp[4]-1]!=1 && surface[pointp[5]][pointp[4]-2]!=1)
												{
														Change_point(pointp[4]-1,pointp[5],2);
														Change_point(pointp[4]-2,pointp[5],2);
														Change_point(pointp[6],pointp[7],0);
														Change_point(pointp[0],pointp[1],0);
												}
										}
										else if(pointp[6]<WIDTH-2 && surface[pointp[5]][pointp[4]-1]!=1 && surface[pointp[5]][pointp[4]+1]!=1 && surface[pointp[3]][pointp[2]+1]!=1)
										{
												Change_point(pointp[4]-1,pointp[5],2);
												Change_point(pointp[4]+1,pointp[5],2);
												Change_point(pointp[2]+1,pointp[3],2);
												Change_point(pointp[0],pointp[1],0);
												Change_point(pointp[2],pointp[3],0);
												Change_point(pointp[6],pointp[7],0);
										}	
										enter_count++;
										Print_point();
										Tetris_score(0);
										return;
								case 0:	if(surface[pointp[3]-1][pointp[2]]!=1 && surface[pointp[3]+1][pointp[2]]!=1 && surface[pointp[5]+1][pointp[4]]!=1)
										{
												Change_point(pointp[2],pointp[3]-1,2);
												Change_point(pointp[2],pointp[3]+1,2);
												Change_point(pointp[4],pointp[5]+1,2);
												Change_point(pointp[0],pointp[1],0);
												Change_point(pointp[4],pointp[5],0);
												Change_point(pointp[6],pointp[7],0);
										}
										enter_count++;
										Print_point();
										Tetris_score(0);
										return;
						}
				case 3: switch(enter_count%4)
						{
								case 1: if(surface[pointp[5]+1][pointp[4]]!=1)
										{
												Change_point(pointp[4],pointp[5]+1,2);
												Change_point(pointp[2],pointp[3],0);
												enter_count++;
												Print_point();
												Tetris_score(0);
										}
										return;
								case 2:	if(pointp[0]==1)
										{
												if(surface[pointp[5]][pointp[4]+1]!=1 && surface[pointp[5]+1][pointp[4]]!=1)
												{
														Change_point(pointp[4]+1,pointp[5],2);
														Change_point(pointp[4],pointp[5]+1,2);
														Change_point(pointp[0],pointp[1],0);
														Change_point(pointp[6],pointp[7],0);
												}
										}
										if(surface[pointp[3]][pointp[2]-1]!=1)
										{
												Change_point(pointp[2]-1,pointp[3],2);
												Change_point(pointp[0],pointp[1],0);
										}
										enter_count++;
										Print_point();
										Tetris_score(0);
										return;
								case 3:	if(surface[pointp[3]-1][pointp[2]]!=1)
										{
												Change_point(pointp[2],pointp[3]-1,2);
												Change_point(pointp[4],pointp[5],0);
										}
										enter_count++;
										Print_point();
										Tetris_score(0);
										return;
								case 0: if(pointp[0]==WIDTH-2)
										{
												if(surface[pointp[1]][pointp[0]-1]!=1 && surface[pointp[3]][pointp[2]-1]!=1)
												{
														Change_point(pointp[0]-1,pointp[1],2);
														Change_point(pointp[2]-1,pointp[3],2);
														Change_point(pointp[0],pointp[1],0);
														Change_point(pointp[6],pointp[7],0);
												}
										}
										else if(surface[pointp[5]][pointp[4]+1]!=1)
										{
												Change_point(pointp[4]+1,pointp[5],2);
												Change_point(pointp[6],pointp[7],0);
										}
										enter_count++;
										Print_point();
										Tetris_score(0);
										return;
						}
				case 4: return;
				case 5: switch(enter_count%2)
						{
								case 1:	if(pointp[1]==1)
												return;
										if(surface[pointp[3]][pointp[2]+1]!=1 && surface[pointp[3]-1][pointp[2]+1]!=1)
										{
												Change_point(pointp[2]+1,pointp[3],2);
												Change_point(pointp[2]+1,pointp[3]-1,2);
												Change_point(pointp[0],pointp[1],0);
												Change_point(pointp[6],pointp[7],0);
												enter_count++;
												Print_point();
												Tetris_score(0);
										}
										return;
								case 0:	if(pointp[2]==1)
										{
												if(surface[pointp[7]][pointp[6]+1]!=1 && surface[pointp[7]][pointp[6]+2]!=1)
												{
														Change_point(pointp[6]+1,pointp[7],2);
														Change_point(pointp[6]+2,pointp[7],2);
														Change_point(pointp[0],pointp[1],0);
														Change_point(pointp[6],pointp[7],0);
												}
										}
										else if(surface[pointp[3]][pointp[2]-1]!=1 && surface[pointp[7]][pointp[6]+1]!=1)
										{
												Change_point(pointp[2]-1,pointp[3],2);
												Change_point(pointp[6]+1,pointp[7],2);
												Change_point(pointp[0],pointp[1],0);
												Change_point(pointp[4],pointp[5],0);
										}
										enter_count++;
										Print_point();
										Tetris_score(0);
										return;
						}
				case 6: switch(enter_count%4)
						{
								case 1: if(pointp[0]==WIDTH-2)
										{
												if(surface[pointp[1]][pointp[0]-2]!=1 && surface[pointp[3]][pointp[2]-2]!=1 && surface[pointp[3]][pointp[2]-1]!=1)
												{
														Change_point(pointp[0]-2,pointp[1],2);
														Change_point(pointp[2]-2,pointp[3],2);
														Change_point(pointp[2]-1,pointp[3],2);
														Change_point(pointp[0],pointp[1],0);
														Change_point(pointp[4],pointp[5],0);
														Change_point(pointp[6],pointp[7],0);
												}
										}
										else if(surface[pointp[1]][pointp[0]-1]!=1 && surface[pointp[3]][pointp[2]-1]!=1 && surface[pointp[3]][pointp[2]+1]!=1)
										{
												Change_point(pointp[0]-1,pointp[1],2);
												Change_point(pointp[2]+1,pointp[3],2);
												Change_point(pointp[2]-1,pointp[3],2);
												Change_point(pointp[0],pointp[1],0);
												Change_point(pointp[4],pointp[5],0);
												Change_point(pointp[6],pointp[7],0);
										}
										enter_count++;
										Print_point();
										Tetris_score(0);
										return;
								case 2: if(surface[pointp[1]][pointp[0]+1]!=1 && surface[pointp[1]][pointp[0]+2]!=1 && surface[pointp[5]+1][pointp[4]]!=1)
										{
												Change_point(pointp[0]+1,pointp[1],2);
												Change_point(pointp[0]+2,pointp[1],2);
												Change_point(pointp[4],pointp[5]+1,2);
												Change_point(pointp[0],pointp[1],0);
												Change_point(pointp[2],pointp[3],0);
												Change_point(pointp[6],pointp[7],0);
										}
										enter_count++;
										Print_point();
										Tetris_score(0);
										return;
								case 3:	if(pointp[0]==1)
										{
												if(surface[pointp[7]][pointp[6]+2]!=1 && surface[pointp[5]][pointp[4]+2]!=1 && surface[pointp[5]][pointp[4]+1]!=1)
												{
														Change_point(pointp[6]+2,pointp[7],2);
														Change_point(pointp[4]+2,pointp[5],2);
														Change_point(pointp[4]+1,pointp[5],0);
														Change_point(pointp[0],pointp[1],0);
														Change_point(pointp[2],pointp[3],0);
														Change_point(pointp[6],pointp[7],0);
												}
										}
										else if(surface[pointp[5]][pointp[4]-1]!=1 && surface[pointp[5]][pointp[4]+1]!=1 && surface[pointp[7]][pointp[6]+1]!=1)
										{
												Change_point(pointp[4]-1,pointp[5],2);
												Change_point(pointp[4]+1,pointp[5],2);
												Change_point(pointp[6]+1,pointp[7],2);
												Change_point(pointp[0],pointp[1],0);
												Change_point(pointp[2],pointp[3],0);
												Change_point(pointp[6],pointp[7],0);
										}	
										enter_count++;
										Print_point();
										Tetris_score(0);
										return;
								case 0:	if(surface[pointp[1]+1][pointp[0]]!=1 && surface[pointp[3]+1][pointp[2]]!=1 && surface[pointp[3]-1][pointp[2]]!=1)
										{
												Change_point(pointp[0],pointp[1]+1,2);
												Change_point(pointp[2],pointp[3]+1,2);
												Change_point(pointp[2],pointp[3]-1,2);
												Change_point(pointp[0],pointp[1],0);
												Change_point(pointp[4],pointp[5],0);
												Change_point(pointp[6],pointp[7],0);
										}
										enter_count++;
										Print_point();
										Tetris_score(0);
										return;
						}
				case 7: switch(enter_count%2)
						{
								case 1:	if(pointp[1]==1)
												return;
										if(surface[pointp[1]][pointp[0]-1]!=1 && surface[pointp[1]-1][pointp[0]-1]!=1)
										{
												Change_point(pointp[0]-1,pointp[1],2);
												Change_point(pointp[0]-1,pointp[1]-1,2);
												Change_point(pointp[2],pointp[3],0);
												Change_point(pointp[4],pointp[5],0);
												enter_count++;
												Print_point();
												Tetris_score(0);
										}
										return;
								case 0:	if(pointp[6]==WIDTH-2)
										{
												if(surface[pointp[7]][pointp[6]-1]!=1 && surface[pointp[7]][pointp[6]-2]!=1)
												{
														Change_point(pointp[6]-1,pointp[7],2);
														Change_point(pointp[6]-2,pointp[7],2);
														Change_point(pointp[0],pointp[1],0);
														Change_point(pointp[6],pointp[7],0);
												}
										}
										else if(surface[pointp[5]][pointp[4]+1]!=1 && surface[pointp[7]][pointp[6]-1]!=1)
										{
												Change_point(pointp[4]+1,pointp[5],2);
												Change_point(pointp[6]-1,pointp[7],2);
												Change_point(pointp[0],pointp[1],0);
												Change_point(pointp[2],pointp[3],0);
										}
										enter_count++;
										Print_point();
										Tetris_score(0);
										return;
						}
		}
}
//往左移
void Move_left()
{
		//判到没到边缘
		if(pointp[0]==1||pointp[2]==1||pointp[4]==1||pointp[6]==1)
		{
				return;
		}
		//判左边有没有方块
		if( surface[pointp[1]][pointp[0]-1]==1 || surface[pointp[3]][pointp[2]-1]==1 || 
						surface[pointp[5]][pointp[4]-1]==1 || surface[pointp[7]][pointp[6]-1]==1)
		{
				return;
		}
		Change_point(pointp[0],pointp[1],0);
		Change_point(pointp[2],pointp[3],0);
		Change_point(pointp[4],pointp[5],0);
		Change_point(pointp[6],pointp[7],0);
		Change_point(pointp[0]-1,pointp[1],2);
		Change_point(pointp[2]-1,pointp[3],2);
		Change_point(pointp[4]-1,pointp[5],2);
		Change_point(pointp[6]-1,pointp[7],2);
		Print_point();
		Tetris_score(0);
		return;
}
void Move_right()
{
		//判到到没到边缘
		if(*(pointp)==WIDTH-2||*(pointp+2)==WIDTH-2||*(pointp+4)==WIDTH-2||*(pointp+6)==WIDTH-2)
		{
				return;
		}
		//判右边有没有方块
		if( surface[*(pointp+1)][*(pointp)+1]==1 || surface[*(pointp+3)][*(pointp+2)+1]==1 || surface[*(pointp+5)][*(pointp+4)+1]==1 || surface[*(pointp+7)][*(pointp+6)+1]==1)
		{
				return;
		}
		Change_point(*pointp,*(pointp+1),0);
		Change_point(*(pointp+2),*(pointp+3),0);
		Change_point(*(pointp+4),*(pointp+5),0);
		Change_point(*(pointp+6),*(pointp+7),0);
		Change_point(*pointp+1,*(pointp+1),2);
		Change_point(*(pointp+2)+1,*(pointp+3),2);
		Change_point(*(pointp+4)+1,*(pointp+5),2);
		Change_point(*(pointp+6)+1,*(pointp+7),2);
		Print_point();
		Tetris_score(0);
		return;
}
void Move_down()
{
		if(Judge_stop()==YES)
		{
				return;
		}
		Change_point(pointp[0],pointp[1],0);
		Change_point(pointp[2],pointp[3],0);
		Change_point(pointp[4],pointp[5],0);
		Change_point(pointp[6],pointp[7],0);
		Change_point(pointp[0],pointp[1]+1,2);
		Change_point(pointp[2],pointp[3]+1,2);
		Change_point(pointp[4],pointp[5]+1,2);
		Change_point(pointp[6],pointp[7]+1,2);
		Print_point();
		Tetris_score(0);
}

//用户操作
static void * Control(void * arg)
{
		pthread_detach(pthread_self());
		int fd=0;
		char c;
		while(1)
		{
				c=getch();
				pthread_mutex_lock(&lock);
				switch(c)
				{
						case 'w': Change_shape();
								  break;
						case 'a': Move_left();
								  break;
						case 'd': Move_right();
								  break;
						case 's': Move_down();
								  break;
						case 't':
						//直接进入中断处理函数，不释放互斥锁，主进程即被暂停
								pthread_kill(pthread_self(),SIGALRM);
								break;
						case 'q': 
								printf("确定要退出吗？(y确定,任意键取消)\n");
								c=getch();
								if(c!='y')
										break;
								pthread_mutex_unlock(&lock);
								tcsetattr(fd,TCSANOW,&tm_old);
								game_state=0;						
								pthread_exit(NULL);
						default:break;
				}
				pthread_mutex_unlock(&lock);	
		}

}
//判成功，从下往上，判行满就清空该行
static int Judge_success(void)
{
		int i;
		for(i=HEIGHT-2;i>=1;i--)
		{
				if(surface[i][0]==WIDTH-2)
						return i;
		}	
		return NO;
}
//判失败，判最上面一行，触顶则失败，游戏结束
static int Judge_failure()
{
		int i;
		for(i=1;i<WIDTH-1;i++)
		{
				if(surface[1][i]==1)
						return YES;
		}
		return NO;
}
//判图形落地，图形任何一点的下方与其他点有接触就停止下落
int Judge_stop()
{
		if(surface[*(pointp+1)+1][*pointp]==1)
				return YES;
		if(surface[*(pointp+3)+1][*(pointp+2)]==1)
				return YES;
		if(surface[*(pointp+5)+1][*(pointp+4)]==1)
				return YES;
		if(surface[*(pointp+7)+1][*(pointp+6)]==1)
				return YES;
		if(*(pointp+1)==HEIGHT-2||*(pointp+3)==HEIGHT-2||*(pointp+5)==HEIGHT-2||*(pointp+7)==HEIGHT-2)
				return YES;
		else 
				return NO;
}
//初始化第一个图形和第二个图形的编号
void Init_surface()
{
		int i,j;
		for(i=1;i<HEIGHT-1;i++)
		{
				for(j=0;j<WIDTH-1;j++)
						surface[i][j]=0;
		}
}
void Count_down()
{
		printf("3");
		fflush(stdout);
		current_shape=Create_shape();
		sleep(1);
		printf("\b");
		printf("2");
		fflush(stdout);
		next_shape=Create_shape();
		sleep(1);
		printf("\b");
		printf("1");
		fflush(stdout);
		Init_surface();
		sleep(1);
		return;
}
void Clean_point(int height)
{
		int i,j;
		for(i=0;i<WIDTH-1;i++)
				surface[height][i]==0;
		for(i=height-1;i>0;i--)
		{
				for(j=0;j<WIDTH-1;j++)
				{
						surface[i+1][j]=surface[i][j];
				}
		}
}
//初始化图形起始位置
void Init_shape()
{
		int i,j;
		switch(current_shape)
		{
				case 1: for(i=1;i<=4;i++)
								Change_point((WIDTH-2)/2,i,2);
						break;
				case 2: for(i=1;i<=3;i++)
								Change_point((WIDTH-2)/2,i,2);
						Change_point((WIDTH-2)/2+1,3,2);
						break;
				case 3: for(i=7;i<=9;i++)
								Change_point(i,2,2);
						Change_point((WIDTH-2)/2,1,2);
						break;
				case 4: for(i=(WIDTH-2)/2-1;i<=(WIDTH-2)/2;i++)
								for(j=1;j<=2;j++)
										Change_point(i,j,2);
						break;
				case 5: for(i=(WIDTH-2)/2-1;i<=(WIDTH-2)/2;i++)
								Change_point(i,1,2);
						for(i=(WIDTH-2)/2;i<=(WIDTH-2)/2+1;i++)
								Change_point(i,2,2);
						break;
				case 6: for(i=1;i<=3;i++)
								Change_point((WIDTH-2)/2,i,2);
						Change_point((WIDTH-2)/2-1,3,2);
						break;
				case 7: for(i=(WIDTH-2)/2;i<=(WIDTH-2)/2+1;i++)
								Change_point(i,1,2);
						for(i=(WIDTH-2)/2-1;i<=(WIDTH-2)/2;i++)
								Change_point(i,2,2);
						break;
				default:break;
		}
}
void Level_up(void)
{
		static int count=0;
		if(score==50)
		{
				if(count<2)
						printf("升级！当前等级 2\n");
				speed-=100000;	
				count++;
		}
		else if(score==100)
		{
				printf("升级！当前等级 3\n");
				speed-=100000;	
		}
		else if(score==150)
		{
				printf("升级！当前等级 4\n");
				speed-=100000;	
		}
		else if(score==200)
		{
				printf("升级！当前等级 5\n");
				speed-=100000;	
		}
		else if(score==250)
		{
				printf("升级！当前等级 6\n");
				speed-=100000;
		}
		else if(score==300)
		{
				printf("升级！当前等级 最终级\n");
				mode=INVISIBLE;
		}
		if(count==2)
				count=0;
}
//控制图形自动下落
void Shape_down(int speed)
{
		int i;
		int flag=0;
		int ret;
		int point_count=0;
		int fd=0;
		while(1)
		{
				flag=0;
				usleep(speed);
				pthread_mutex_lock(&lock);
				if(game_state==0)
						return;
				//如果到底了，停止下落，判断
				if(Judge_stop()==YES)
				{
						Change_point(*pointp,*(pointp+1),1);
						Change_point(*(pointp+2),*(pointp+3),1);
						Change_point(*(pointp+4),*(pointp+5),1);
						Change_point(*(pointp+6),*(pointp+7),1);
						//判断底部有没有满
						for(i=1;i<WIDTH-1;i++)
						{
								if(surface[pointp[7]][i]==1)
										point_count++;
						}
						surface[pointp[7]][0]=point_count;
						point_count=0;
						if((ret=Judge_success())!=NO)
						{
								Clean_point(ret);
								flag+=1;
								while((ret=Judge_success())!=NO)
								{
										Clean_point(ret);
										flag+=1;
								}
						}
						//判断有没有到顶
						else if(Judge_failure()==YES)
						{
								if(tcsetattr(fd,TCSANOW,&tm_old)<0)
										exit(0);
								printf("game over\n");
								score=0;
								return;
						}
						current_shape=next_shape;
						Init_shape();
						enter_count=1;
						next_shape=Create_shape();
				}
				else
				{
						Change_point(*pointp,*(pointp+1),0);
						Change_point(*(pointp+2),*(pointp+3),0);
						Change_point(*(pointp+4),*(pointp+5),0);
						Change_point(*(pointp+6),*(pointp+7),0);
						Change_point(*pointp,*(pointp+1)+1,2);
						Change_point(*(pointp+2),*(pointp+3)+1,2);
						Change_point(*(pointp+4),*(pointp+5)+1,2);
						Change_point(*(pointp+6),*(pointp+7)+1,2);
				}
				if(tcsetattr(fd,TCSANOW,&tm_old)<0)
						exit(0);
				Print_point();
				if(flag>0)
				{
						Tetris_score(flag);
						Level_up();
				}
				else
						Tetris_score(0);
				if(tcsetattr(fd,TCSANOW,&tm)<0)
						exit(0);
				pthread_mutex_unlock(&lock);
		}
}
//初始化图形，分数，起始位置
void Tetris_init_game()
{
		pthread_t tcon;
		//生成当前和下一个图形，并倒计时
		Count_down();
		if(current_shape>7||current_shape<1)
		{
				printf("生成形状错误\n");
				exit(-1);
		}
		if(next_shape>7||next_shape<1)
		{
				printf("生成形状错误\n");
				exit(-1);
		}
		Init_shape();
		Print_point();
		pthread_mutex_init(&lock,NULL);
		pthread_create(&tcon,NULL,Control,NULL);
		Tetris_score(0);
		Shape_down(speed);
		int fd=0;
		tcsetattr(fd,TCSANOW,&tm_old);
		pthread_cancel(tcon);
		return;
}

//主函数，选择模式
void Tetris_game_body(int sockfd)
{
		if(Malloc_surface())
				return;
		int choise;
		int fd=0;
		if(tcgetattr(fd,&tm)<0)
				exit(0);
		tm_old=tm;
		cfmakeraw(&tm);
		system("clear");
		while(1)
		{
				free_flag=0;
				Print_head();
				scanf("%d",&choise);
				getchar();
				switch(choise)
				{
						case 1:printf("test2\n"); 
								speed=NORMAL;
								mode=VISIBLE;
								break;
						case 2: speed=FAST;
								mode=VISIBLE;
								break;
						case 3: speed=FAST;
								mode=INVISIBLE;
								break;
						case 0: return;
						default:system("clear");
								printf("皮？重选\n");
								sleep(1);
								continue;
				}
				system("clear");
				game_state=1;
				Tetris_init_game(time,mode);  //go to play!
				printf("是否上传分数？(y/n)\n");  //have returned from playing
				choise=getchar();
				getchar();
				if(choise=='y')
				{
						free_flag=1;
						pthread_mutex_lock(&free_lock);
						Upload_game_score(sockfd,TETRIS,score,0);
						pthread_mutex_unlock(&free_lock);
				}
				else if(choise!='n')
						printf("未知选择,默认取消\n");
				printf("再来一局？(y/n)");
				choise=getchar();
				if(choise=='y')
				{
						getchar();
						continue;
				}
				else if(choise==10)
						continue;
				else if(choise=='n')
				{
						getchar();
						break;
				}
				else
				{
						getchar();
						printf("未知选择,默认返回\n");
						break;
				}
		}
		pthread_mutex_destroy(&lock);
		free(surface);
		return;
}
