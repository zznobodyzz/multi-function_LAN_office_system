#include"client_head.h"
extern pthread_mutex_t work_queue_lock;
extern WORK_QUEUE * work_queue_head;
extern int work_queue_for_load;
extern sem_t sem;
extern int free_flag;
extern pthread_mutex_t free_lock;

int Set_work_queue(int priority,const char * task,void * arg)
{
		struct _game_at_work{
			int game;
			int score;
			int sockfd;
		};
		struct _upload_at_work{
			char filename[MAX_FILENAME];
			unsigned long offset;
			int sockfd;
			int mode;
		};
		struct _download_at_work{
			char filename[MAX_FILENAME];
			char downloadname[MAX_FILENAME];
			unsigned long offset;
			int sockfd;
			int mode;
		};
		int i;
		WORK_QUEUE * new;
		new=(WORK_QUEUE *)malloc(sizeof(WORK_QUEUE));
		if(new==NULL)
		{
				perror("malloc failed\n");
				return -1;  
		}
		pthread_mutex_lock(&work_queue_lock);
		new->next=work_queue_head->next;
		work_queue_head->next=new;
		new->priority=priority;
		new->times=0;
		strcpy(new->task,task);
		if(strcmp(task,"upload_game_score")==0)
				memcpy(&(new->arg),(struct _game_at_work *)arg,sizeof(struct _game_at_work));  //将参数拷贝进队列，读取时根据task转换成不同类型结构体
		else if(strcmp(task,"upload_document")==0)
		{
				work_queue_for_load++;
				memcpy(&(new->arg),(struct _upload_at_work *)arg,sizeof(struct _upload_at_work));
		}
		else if(strcmp(task,"download_ducument")==0)
		{
				work_queue_for_load++;
				memcpy(&(new->arg),(struct _download_at_work *)arg,sizeof(struct _download_at_work));
		}
		for(i=0;i<MAX_WORK_QUEUE_TASK_NO;i++)   //遍历编号数组，找到一个没有被用的编号，赋给此任务
		{
				if(work_queue_task_no[i]==0)
				{		
						work_queue_task_no[i]=1;
						break;
				}
		}
		if(i==MAX_WORK_QUEUE_TASK_NO)
				return -1;                 //如果队列满。。。返回失败,不过这应该不可能,除非你同时传100个大型文件。。。
		new->no=i;
		pthread_mutex_unlock(&work_queue_lock);
		return i;
}

void * Set_upload_score_at_work(void * arg)
{
		pthread_detach(pthread_self());
		int i=3;
		struct _at_work{
				int game;
				int score;
				int sockfd;
		}at_work;
		at_work=*((struct _at_work *)arg);
		sem_post(&sem);
		while(i>0)
		{
				if(Set_work_queue(0,"upload_game_score",&at_work)!=-1)  //将该任务放入工作队列
						break;
				sleep(20);
				i--;
		}
		if(i==0)
				printf("加入工作队列失败\n");
		else 
				printf("加入工作队列成功\n");
		return;
}

int Set_load_file_at_work(void * arg,int flag)
{
		pthread_detach(pthread_self());
		struct _up_at_work{
				char filename[MAX_FILENAME];
				unsigned long offset;
				int sockfd;
				int mode;
		}up_at_work;
		struct _down_at_work{
				char filename[MAX_FILENAME];
				char downloadname[MAX_FILENAME];
				unsigned long offset;
				int sockfd;
				int mode;
		}down_at_work;
		if(flag==1)
		{
				up_at_work=*((struct _up_at_work *)arg);
				if(Set_work_queue(0,"upload_document",&up_at_work)==-1)  //将该任务放入工作队列
						return -1;
		}
		else if(flag==2)
		{
				down_at_work=*((struct _down_at_work *)arg);
				if(Set_work_queue(0,"download_document",&down_at_work)==-1)  //将该任务放入工作队列
						return -1;
		}
		printf("加入工作队列成功\n");
		return 0;
}

//检索工作队列中优先级高的任务，取出并执行
WORK_QUEUE * Traverse_work_queue()
{
		if(work_queue_head->next==NULL)
				return NULL;
		WORK_QUEUE * work=work_queue_head->next;
		WORK_QUEUE * p=work_queue_head; //p表示优先级最高任务(除0级任务外)
		while(work)
		{
				if(work->priority==0) //直接执行最近加入的0级任务
						return work;
				if(work->priority <= p->priority)  //找出优先级最高的,如果有同优先级的任务，优先做先加入的
						p=work;
				work=work->next;
		}
		return p; //never return work_queue_head,because no task`s priority can lower than work_queue_head
}

void Destroy_work_queue(void)
{
		if(NULL==work_queue_head->next)
				return;
		else
		{
				WORK_QUEUE * work=work_queue_head->next;
				while(work)
				{
						work_queue_head->next=work->next;
						free(work);
						work=work_queue_head->next;
				}
		}
}

void Destroy_task(WORK_QUEUE * work,char * task,int id)  //never failed
{
		if(work_queue_head->next==NULL)
				return;
		WORK_QUEUE * del;
		WORK_QUEUE * trav=work_queue_head;
		if(task!=NULL)
		{
				while(trav->next)
				{
						if(strcmp(trav->next->task,task)==0)
						{
								del=trav->next;
								work_queue_task_no[del->no]=0;
								trav->next=del->next;
								free(del);	
						}
						trav=trav->next;
						if(trav==NULL)
								break;
				}
		}
		else
		{ 		
				if(work)
				{
						while(trav->next)
						{
								if(trav->next->no==work->no)
								{
										del=trav->next;
										trav->next=del->next;
										free(del);
										work_queue_task_no[work->no]=0; //将工作队列任务编号表对应编号置为可用
										break;
								}
								trav=trav->next;
								if(trav==NULL)
										break;
						}
				}
				else if(id!=-1)
				{
						while(trav->next)
						{
								if(trav->next->no==id)
								{
										del=trav->next;
										trav->next=del->next;
										free(del);
										work_queue_task_no[id]=0;
										break;
								}
								trav=trav->next;
								if(trav==NULL)
										break;
						}
				}
		}
}

//工作队列，本质上为一个线程
void * Work_queue_do_task(void * arg)
{
		WORK_QUEUE * work;
		while(1)
		{
				sleep(1);
				if(free_flag==1)
						continue;
				pthread_mutex_lock(&work_queue_lock);
				work=Traverse_work_queue();
				pthread_mutex_unlock(&work_queue_lock);
				if(work==NULL)
						continue;
				if(strcmp(work->task,"upload_game_score")==0)
				{		
						int ret;
						struct _game_at_work{
								int game;
								int score;
								int sockfd;
						}game_at_work;
						memcpy(&game_at_work,&(work->arg),sizeof(game_at_work));
						if(free_flag==0)//若空闲，则尝试再次上传
						{
								pthread_mutex_lock(&free_lock);
								ret=Upload_game_score(game_at_work.sockfd,game_at_work.game,game_at_work.score,1);  //返回0成功,1失败
								pthread_mutex_unlock(&free_lock);
								if(ret==1)
								{ //如果失败
										pthread_mutex_unlock(&free_lock);
										if((++work->times)<MAX_RETRY_TIMES)
										{	//如果还没超过重试次数	
												printf("上传游戏分数失败,正在重试\n");
												continue;
										}
										else  //超过了重试次数
												printf("上传游戏分数失败,请检查网络环境\n");
								}//或者成功   删除该任务
								pthread_mutex_lock(&work_queue_lock);
								Destroy_task(work,NULL,-1);
								work=NULL; //销毁任务后防止野指针
								pthread_mutex_unlock(&work_queue_lock);	
						}
				}
				else if(strcmp(work->task,"upload_document")==0)
				{
						int ret;
						struct _load_at_work{
								char filename[MAX_FILENAME];
								unsigned long offset;
								int sockfd;
								int mode;
						}upload_at_work;
						memcpy(&upload_at_work,&(work->arg),sizeof(upload_at_work));
						if(free_flag==0)
						{
								pthread_mutex_lock(&free_lock);
								if((ret=Upload_doc_task(upload_at_work.sockfd,upload_at_work.filename,upload_at_work.offset,&(work->times),upload_at_work.mode))<=0) //函数返回最新的文件偏移量,返回0表示完成,-1出错
								{
										pthread_mutex_unlock(&free_lock);
										if(ret<0)
										{
												if(work->times<MAX_RETRY_TIMES)	
												{
														printf("上传文件 %s 时发生错误，正在重试\n",upload_at_work.filename);
														work->times++;
														continue;
												}
												else
														printf("上传文件 %s 时发生错误，请重试\n",upload_at_work.filename);
										}
										pthread_mutex_lock(&work_queue_lock);   //成功或超过重试次数就移除任务
										Destroy_task(work,NULL,-1);
										work_queue_for_load--;
										work=NULL;
										pthread_mutex_unlock(&work_queue_lock);
								}
								else
								{
										pthread_mutex_unlock(&free_lock);
										upload_at_work.offset=ret;
										memcpy(&(work->arg),&upload_at_work,sizeof(upload_at_work));  //将新的偏移量写回参数结构体
								}
						}
				}
				else if(strcmp(work->task,"download_document")==0)
				{
						int ret;
						struct _load_at_work{
								char filename[MAX_FILENAME];
								char downloadname[MAX_FILENAME];
								unsigned long offset;
								int sockfd;
								int mode;
						}download_at_work;
						memcpy(&download_at_work,&(work->arg),sizeof(download_at_work));
						if(free_flag==0)
						{
								pthread_mutex_lock(&free_lock);
								if((ret=Download_doc_task(download_at_work.sockfd,download_at_work.filename,download_at_work.downloadname,download_at_work.offset,&(work->times),download_at_work.mode))<=0)//函数返回最新的文件偏移量,返回0表示完成
								{
										pthread_mutex_unlock(&free_lock);
										if(ret<0)
										{
												if(work->times<=MAX_RETRY_TIMES)	
												{
														printf("下载文件 %s 时发生错误，正在重试\n",download_at_work.filename);
														work->times++;
														continue;
												}
												else
														printf("下载文件 %s 时发生错误，请重试\n",download_at_work.filename);
										}
										pthread_mutex_lock(&work_queue_lock);  //成功或超过重试次数就移除任务
										Destroy_task(work,NULL,-1);
										work_queue_for_load--;
										work=NULL;
										pthread_mutex_unlock(&work_queue_lock);
								}
								else
								{
										pthread_mutex_unlock(&free_lock);
										download_at_work.offset=ret;
										memcpy(&(work->arg),&download_at_work,sizeof(download_at_work));  //将新的偏移量写回参数结构体
								}
						}
				}
		}
}
