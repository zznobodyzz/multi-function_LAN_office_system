#ifndef __GLOBAL_H
#define __GLOBAL_H
//配置信息
char doc_path[MAX_PATH];
char adv_per_time=60;
char data_base[MAX_FILENAME];
//定时推送内容
char advbuf[MAX_DATA];
sqlite3 * db;
fd_set commonset,globalset;
int udpfd;
sem_t sem;
//游戏等待对列头
GAME_LIST * game_queue_head;
//游戏功能标志位
char game_queue_stat=1;
//广播功能标志位
char adv_flag=0;
pthread_mutex_t game_queue_lock;
//查看用户聊天记录的顶级密码
char highestpass[20]="chathisview123.";
//修改权限的密码
char modpwd[7]="9507";
char print_log_flag=1;
pthread_t tadv;
pthread_t ttellip;
#endif
