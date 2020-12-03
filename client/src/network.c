#include"client_head.h"
extern int udpfd;
extern int alarm_flag;
extern void * Alarm(void * arg);

//自动获取服务器ip
int Get_lan_server_ip(char * addr)
{
		USR usr;
		SER ser;
		int	tellipfd;
		pthread_t talarm;
		struct sockaddr_in broadcast,saddr;
		int	on;
		int try=0;
		int slen=sizeof(struct sockaddr_in);

		if((tellipfd=socket(PF_INET,SOCK_DGRAM,0))<0)  //udp
		{
				perror("自动获取ip失败\n");
				return -1;
		}
		on = 1;
		if(setsockopt(tellipfd,SOL_SOCKET,SO_BROADCAST,&on,sizeof(on))<0)  //broadcast
		{
				perror("自动获取ip失败\n");
				close(tellipfd);
				return -1;
		}
		broadcast.sin_family=PF_INET;
		broadcast.sin_addr.s_addr=htonl(INADDR_BROADCAST);
		broadcast.sin_port= htons(SERVER_IP_PORT);
		usr.cmd=GET_SERVER_IP;
		while(try<3)
		{
				if(sendto(tellipfd,&usr,sizeof(usr),0,(struct sockaddr *)&broadcast,sizeof(broadcast))<0) 
				{
						perror("自动获取ip失败\n");
						close(tellipfd);
						return -1;
				}
				int alarm_time=1;
				alarm_flag=1;
				pthread_create(&talarm,NULL,Alarm,&alarm_time);
				if(recvfrom(tellipfd,&ser,sizeof(ser),0,(struct sockaddr *)&saddr,&slen)<0)
				{
						try++;
						continue;
				}
				if(ser.sig==SERVER_IP)
						break;
				try++;
		}
		close(tellipfd);
		alarm_flag=0;
		if(try!=3)
		{
			strcpy(addr,ser.data);
			return 0;
		}
		printf("自动获取ip失败\n");
		return -1;
}

void Udp_init(char * addr)
{
		struct sockaddr_in saddr;
		int len;
		char buf[MAX_DATA];
		memset(buf,0,MAX_DATA);
		len=sizeof(struct sockaddr_in);
		saddr.sin_family=PF_INET;
		saddr.sin_port=htons(SERVER_UDP_PORT);
		saddr.sin_addr.s_addr=inet_addr(addr);
		if(bind(udpfd,(struct sockaddr *)&saddr,len)<0)
		{
				printf("推送接收服务初始化失败\n");
				printf("客户端正常启动\n");
				perror("bind failed");
				return;
		}
		printf("推送接收服务已启动,可在登录后关闭\n");
		return;
}

int Sockfd(char * addr)
{
		system("clear");
		int sockfd;
		struct sockaddr_in saddr,caddr;
		int len;
		int clen;
		len=sizeof(struct sockaddr_in);
		clen=sizeof(struct sockaddr_in);
		sockfd=socket(PF_INET,SOCK_STREAM,0);
		udpfd=socket(PF_INET,SOCK_DGRAM,0);
		if(sockfd<0||udpfd<0)
		{
				printf("客户端遇到致命错误\n");
				perror("socket failed");
				exit(0);
		}
		saddr.sin_family=PF_INET;
		saddr.sin_port=htons(SERVER_TCP_PORT);
		saddr.sin_addr.s_addr=inet_addr(addr);
		if(connect(sockfd,(struct sockaddr *)&saddr,len)<0)
		{
				printf("服务器未在线,客户端启动失败\n");
				perror("connect failed");
				exit(0);
		}
		if(getsockname(sockfd,(struct sockaddr *)&caddr,&clen)<0)
		{
				printf("客户端遇到致命错误\n");
				perror("connect failed");
				exit(0);
		}
		printf("正在初始化推送接收服务\n");
		Udp_init(inet_ntoa(caddr.sin_addr));
		printf("已连接至服务器\n");
		return sockfd;
}
