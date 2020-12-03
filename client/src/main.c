#include"client_head.h"
#include"global_data.h"
extern void * Work_queue_do_task(void * arg);
extern void * Advertise(void * arg);
extern void Game_due(int signo);
extern void Pause(int signo);
extern void Init(int sockfd);

void Init_config(void)
{
		FILE * fp;
		char buf[128];
		fp=fopen("./client_config","r");
		if(fp==NULL)
		{
				perror("配置文件打开失败\n");
				exit(-1);
		}
		char * p;
		while(fgets(buf,128,fp))
		{
				if(strncmp(buf,"docpath=",8)==0)
				{
						for(p=buf;*p!='\n';p++);
						*p='\0';
						break;
				}
		}
		p=&buf[8];
		strcpy(doc_path,p);
		fclose(fp);
		printf("文档默认目录加载完毕\n");
}

//初始化各种环境
void Init_env(void)
{
		/* 初始化工作队列相关 */
		printf("正在初始化工作队列\n");
		work_queue_head=(WORK_QUEUE *)malloc(sizeof(WORK_QUEUE));
		if(work_queue_head!=NULL)
		{		
				pthread_mutex_init(&free_lock,NULL);
				pthread_mutex_init(&work_queue_lock,NULL);
				work_queue_state=1;
				work_queue_head->priority=7;  //设置最低优先级，仅在遍历时用作参考，没有任务可以设置为此优先级
				work_queue_head->next=NULL;
				int i;
				for(i=0;i<MAX_WORK_QUEUE_TASK_NO;i++)
						work_queue_task_no[i]=0;  //初始化工作队列中任务的编号表
				pthread_create(&tworkqueue,NULL,Work_queue_do_task,NULL);
				printf("初始化工作队列成功\n");
		}
		else
		{
				work_queue_state=0;
				printf("初始化工作队列失败,将无法启用后台下载/上传\n");
		}
		sem_init(&sem,0,0); //初始化一个值为0的线程信号量
		/******可扩展******/
}

void * Alarm(void * arg)
{
		pthread_detach(pthread_self());
		int alarm_time=*((int *)arg);
		sleep(alarm_time);
		if(alarm_flag==1)
				kill(getpid(),SIGUSR2);
		pthread_exit(NULL);
}

void Exit(int sockfd)
{
		int c;
		USR usr;
		pthread_mutex_lock(&free_lock);
		pthread_mutex_lock(&work_queue_lock);
		if(work_queue_head->next)
		{
				pthread_mutex_unlock(&free_lock);
				pthread_mutex_unlock(&work_queue_lock);
				printf("当前还有后台任务未完成,确定退出？(y/n)");
				c=getchar();
				if(c=='n')
						return;
				else if(c!='y') 
				{
						printf("未知选择,默认返回\n");
						return;
				}
		}
		else 
		{
				pthread_mutex_unlock(&work_queue_lock);
				pthread_mutex_unlock(&free_lock);
		}
		pthread_cancel(tworkqueue);
		Destroy_work_queue();
		free(work_queue_head);
		pthread_mutex_destroy(&free_lock);
		pthread_mutex_destroy(&work_queue_lock);
		if(chatflag==1)
				Exit_chat(sockfd);
		if(logout==0)
				Logout(sockfd);
		//备份数据库
		Bake_data();
		pthread_cancel(tadvertise);
		usr.cmd=EXIT;
		write(sockfd,&usr,sizeof(usr));
		close(sockfd);
		printf("***感谢使用***\n");
		exit(0);
}

//接受到ctrl+c也自动完成退出流程
void Exit_fun(int sig)
{
		if(skfd!=0)
		{	
				if(chatflag==1)
						Exit_chat(skfd);
				if(logout==0)
						Logout(skfd);
				Exit(skfd);
				close(skfd);
		}
		exit(0);
}

//超时返回
void handler(int signo)
{
		return;
}

int main(int argc,char **argv)
{
		//设置当服务器挂掉时不会主动结束客户端
		signal(SIGPIPE,SIG_IGN);
		//收到游戏对战请求时，选择是否迎战
		signal(SIGUSR1,Game_due);
		//设置游戏中暂停功能
		signal(SIGALRM,Pause);
		//设置用户强制退出时也要执行正确的退出流程
		signal(SIGINT,Exit_fun);
		//设置输入超时机制
		struct sigaction act;
		sigaction(SIGUSR2,NULL,&act);
		act.sa_handler=handler;
		act.sa_flags&=~SA_RESTART;
		sigaction(SIGUSR2,&act,NULL);
		char addr[MAX_ADDR];
		int sockfd;
		int ret=0;
		int choose;
		int retu=0;
		struct sockaddr_in saddr;
		saddr.sin_port=htons(SERVER_TCP_PORT);
		saddr.sin_family=PF_INET;
		int len;
		len=sizeof(struct sockaddr_in);
		
		system("clear");
		//检查并初始化数据库
		Init_data();
		//初始化配置
		Init_config();
		//初始化环境
		Init_env();
		printf("***所有初始化完成***\n");
		sleep(1);
		printf("正在自动获取当前局域网内服务器ip\n");
		if(Get_lan_server_ip(addr))
		{
				printf("请手动输入服务器ip地址:\n");
				fgets(addr,MAX_ADDR,stdin);
				addr[strlen(addr)-1]='\0';
		}
		else
		{
				printf("自动获取ip成功,正在与服务器连接\n");
				sleep(1);
		}
		saddr.sin_addr.s_addr=inet_addr(addr);
		sockfd=Sockfd(addr);
		skfd=sockfd;
		printf("请按回车键继续\n");
		getchar();
		Title();
		//用户首先必须登录或注册
		Init(sockfd);
		login_flag=1;
		printf("请按回车键开始\n");
		getchar();
		while(1)
		{
				retu=0;
				//如果处于注销状态，则只能选择登录或注册或退出
				if(login_flag==0)
				{
						sockfd=socket(PF_INET,SOCK_STREAM,0);
						if(connect(sockfd,(struct sockaddr *)&saddr,len)<0)
						{
								printf("客户端遇到致命错误\n");
								perror("connect failed");
								exit(0);
						}
						system("clear");
						Init(sockfd);
						printf("请按回车键开始\n");
						getchar();
				}
				Menu();
				scanf("%d",&choose);
				getchar();
				free_flag=0;//为0代表空闲,只有真正进入需要和服务器通信的子功能时，才为不空闲
				switch(choose)
				{
								//词典功能
						case 1: Diction_func(sockfd);
								break;
								//文档功能
						case 2: File_func(sockfd);
								break;	
								//聊天功能
						case 3: Chat_func(sockfd);
								break;
								//游戏功能
						case 4: Game_func(sockfd);
								break;
								//用户选项
						case 5: Client_func(sockfd);
								break;
								//超级用户选项
						case 6:	VIP_func(sockfd);
								break;
								//登出
						case 7: Logout_func(sockfd);
								break;
						case 8: Score(sockfd);
								break;
						case 0: free_flag=1;
								Exit(sockfd);
						default:printf("请输入有效数字\n");
								break;
				}
				if(sync_err_flag==1)
				{
						int times=0;
						close(sockfd);
						login_flag=0;
						while((times++)<3)
						{
								printf("正在尝试重新连接服务器\n");
								sockfd=socket(PF_INET,SOCK_STREAM,0);
								if(connect(sockfd,(struct sockaddr *)&saddr,len)<0)
								{
										printf("客户端遇到致命错误\n");
										perror("connect failed");
										exit(0);
								}
								if(Login(sockfd))
								{
										pthread_create(&tadvertise,NULL,Advertise,NULL);
										login_flag=1;
										sync_err_flag=0;
										break;
								}
								printf("连接失败\n");
								sleep(3);
						}
				}
				free_flag=0;//为0代表空闲
				printf("请按回车键继续\n");
				getchar();
		}
}
