#include"server_head.h"
#include"global_data.h"

extern void * Fun(void *arg);
extern void * Tell_client_ip(void * arg);
extern void * Adv(void * arg);
extern void Create_game_queue(void);

void Signal_handler(int signo)
{
		return;
}

int main(int argc, char **argv)
{
		struct sigaction act;
		//设置从信号处理函数返回后，结束阻塞
		sigaction(SIGUSR2,NULL,&act);
		act.sa_handler=Signal_handler;
		act.sa_flags&=~SA_RESTART;
		sigaction(SIGUSR2,&act,NULL);
		signal(SIGPIPE,SIG_IGN);
		pthread_t tt;
		char addr[MAX_ADDR];
		int lsfd,sockfd,newfd,clen,maxfd;
		struct sockaddr_in caddr;
		int i;
		sem_init(&sem,0,0);

		//绑定地址
		system("clear");
		printf("服务器正在绑定当前ip地址\n");
		if(Get_current_ip(addr))
				exit(0);
		printf("当前ip地址为:%s\n",addr);
		lsfd=Init_fd(addr);
		printf("ip地址绑定成功\n");
		//初始化配置信息
		Init_config();
		printf("正在初始化数据库\n");
		//初始化数据库
		Init_db();
		maxfd=udpfd;
		//设置监听列表
		FD_ZERO(&globalset);
		FD_SET(lsfd,&commonset);
		//创建游戏对战队列
		Create_game_queue();
		pthread_mutex_init(&game_queue_lock,NULL);
		printf("正在开启推送服务\n");
		//开启UDP服务
		pthread_create(&tadv,NULL,Adv,NULL);
		sem_wait(&sem);
		pthread_create(&ttellip,NULL,Tell_client_ip,addr);
		printf("所有初始化完成，服务器开始接受来访\n");
		//开启服务器终端界面
		pthread_create(&tt,NULL,Fun,NULL);

		clen=sizeof(struct sockaddr_in);
		struct timeval tv;
		while(1)
		{
				globalset=commonset;
				//开始监听
				if(select(maxfd+1,&globalset,NULL,NULL,NULL)<=0)
				{
						//如果已连接的客户端请求处理完成，接收到SIGUSR2信号，就重新设置globalset
						continue;          
				}
				for(i=0;i<=maxfd;i++)
				{
						if(FD_ISSET(i,&globalset))
						{
								if(i==lsfd)
								{
										if((newfd=accept(lsfd,(struct sockaddr *)&caddr,&clen))<0)
										{
												perror("accept failed");
												continue;
										}
										printf("一位用户已建立连接：ip：%s   port：%d\n",inet_ntoa(caddr.sin_addr),caddr.sin_port);
										if(newfd>maxfd)
										{
												maxfd=newfd;
										}
										FD_SET(newfd,&commonset);
								}
								else
								{
										Service_choose(i);
								}
						}
				}
		}
}
