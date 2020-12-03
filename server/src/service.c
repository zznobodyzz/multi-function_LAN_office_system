#include"server_head.h"
#include"server_game.h"
#include"server_wuziqi.h"

extern void * Register(void * arg);
extern void * Login(void * arg);
extern void * Logout(void * arg);
extern void * Diction(void * arg);
extern void * History(void * arg);
extern void * Viewdoc(void * arg);
extern void * Searchdoc(void * arg);
extern void * Downloaddoc(void * arg);
extern void * Downloaddoc_task(void * arg);
extern void * Uploaddoc(void * arg);
extern void * Uploaddoc_task(void * arg);
extern void * Historydoc(void * arg);
extern void * Canceldoc(void * arg);
extern void * Joinchat(void * arg);
extern void * Exitchat(void * arg);
extern void * Chatlist(void * arg);
extern void * Chatforall(void * arg);
extern void * Chatforone(void * arg);
extern void * Chathistoryreturn(void * arg);
extern void * Chatreport(void * arg);
extern void * Passwd(void * arg);
extern void * Onlinelist(void * arg);
extern void * Advcel(void * arg);
extern void * Mod(void * arg);
extern void * Score(void * arg);
extern void * Docmode(void * arg);
extern void * Supercheck(void * arg);
extern void * Caremode(void * arg);
extern void * Setcare(void * arg);
extern void * Rank_view(void * arg);
extern void * Wuziqi_pk(void * arg);
extern void * Upload_game_score(void * arg);
extern fd_set commonset;

void Service_choose(int servicefd)
{
		pthread_t treg,tlogin,tlogout,tdic,this;
		pthread_t tvdo,tsdo,thdo,tddo,tddotask,tudo,tudotask,tcdo;
		pthread_t tjch,tech,tchl,tcfo,tcfa,tcrp,tchr;
		pthread_t tpwd,tonline,tadvcel,tscore,tmod;
		pthread_t tsck,tdocmod,tcare,tsetcare;
		pthread_t trankview,twuziqi,tuploadgame;
		USR usr;
		int * sockfp;
		//先读一次，确定客户端请求类型
		read(servicefd,&usr,sizeof(USR));
		sockfp=(int *)malloc(sizeof(int));
		if(sockfp==NULL)
		{
				printf("服务器遇到致命错误\n");
				perror("malloc failed");
				exit(0);
		}
		*sockfp=servicefd;
		FD_CLR(servicefd,&commonset);
		//分支客户端请求
		switch(usr.cmd)
		{
				//注册
				case REGISTER:
						pthread_create(&treg,NULL,Register,sockfp);
						break;
						//登陆
				case LOGIN:
						pthread_create(&tlogin,NULL,Login,sockfp);
						break;
						//登出
				case LOGOUT:
						pthread_create(&tlogout,NULL,Logout,sockfp);
						break;
						//单词查询
				case DIC:
						pthread_create(&tdic,NULL,Diction,sockfp);
						break;
						//单词查询记录查询
				case HIS:
						pthread_create(&this,NULL,History,sockfp);
						break;
						//查看文档列表
				case VDO:
						pthread_create(&tvdo,NULL,Viewdoc,sockfp);
						break;
						//搜索列表
				case SDO:
						pthread_create(&tsdo,NULL,Searchdoc,sockfp);
						break;
						//文档查询历史查询
				case HDO:
						pthread_create(&thdo,NULL,Historydoc,sockfp);
						break;
						//下载
				case DDO:
						pthread_create(&tddo,NULL,Downloaddoc,sockfp);
						break;
				case DDO_TASK:
						pthread_create(&tddotask,NULL,Downloaddoc_task,sockfp);
						//上传（分享）
				case UDO:
						pthread_create(&tudo,NULL,Uploaddoc,sockfp);
						break;
						//后台的快速上传线程
				case UDO_TASK:
						pthread_create(&tudotask,NULL,Uploaddoc_task,sockfp);
						break;
						//取消分享
				case CDO:
						pthread_create(&tcdo,NULL,Canceldoc,sockfp);
						break;
						//加入聊天室
				case JCH:
						pthread_create(&tjch,NULL,Joinchat,sockfp);
						break;
						//退出聊天室
				case ECH:
						pthread_create(&tech,NULL,Exitchat,sockfp);
						break;
						//聊天室成员列表
				case CHL:
						pthread_create(&tchl,NULL,Chatlist,sockfp);
						break;
						//广播
				case CFA:
						pthread_create(&tcfa,NULL,Chatforall,sockfp);
						break;
						//私聊
				case CFO:
						pthread_create(&tcfo,NULL,Chatforone,sockfp);
						break;
						//聊天记录下载
				case CHR:
						pthread_create(&tchr,NULL,Chathistoryreturn,sockfp);
						break;
						//举报
				case CRP:
						pthread_create(&tcrp,NULL,Chatreport,sockfp);
						break;
						//修改密码
				case PWD:
						pthread_create(&tpwd,NULL,Passwd,sockfp);
						break;
						//总体用户列表
				case ONLINELIST:
						pthread_create(&tonline,NULL,Onlinelist,sockfp);
						break;
						//定时广播开启关闭
				case ADVCEL:
						pthread_create(&tadvcel,NULL,Advcel,sockfp);
						break;
						//修改用户等级
				case MOD:
						pthread_create(&tmod,NULL,Mod,sockfp);
						break;
						//软件评分
				case SCORE:
						pthread_create(&tscore,NULL,Score,sockfp);
						break;
						//下载速度设置
				case DMD:
						printf("testtest\n");
						pthread_create(&tdocmod,NULL,Docmode,sockfp);
						break;
						//超级权限检查
				case SCK:
						pthread_create(&tsck,NULL,Supercheck,sockfp);
						break;
						//特别关心模式开启关闭
				case CAREMOD:
						pthread_create(&tcare,NULL,Caremode,sockfp);
						break;
						//设置特别关心
				case SETCARE:
						pthread_create(&tsetcare,NULL,Setcare,sockfp);
						break;
						//查看游戏天梯
				case RANK_VIEW:
						pthread_create(&trankview,NULL,Rank_view,sockfp);
						break;
						//五子棋对战
				case WUZIQI_PK:
						pthread_create(&twuziqi,NULL,Wuziqi_pk,sockfp);
						break;
						//上传游戏分数
				case UPLOAD_GAME:
						pthread_create(&tuploadgame,NULL,Upload_game_score,sockfp);
						break;
				case EXIT:
						printf("一位用户已断开连接\n");
						close(servicefd);
						break;
				default:printf("收到无法识别的请求,自动忽略\n");
						break;
		}
}
