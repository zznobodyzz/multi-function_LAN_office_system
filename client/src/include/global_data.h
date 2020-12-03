pthread_t tworkqueue;
pthread_t tadvertise;
//为了防止空闲任务和用户任务发生冲突的锁
pthread_mutex_t free_lock;
//为了防止加入工作队列和检索工作队列发生冲突的锁
pthread_mutex_t work_queue_lock;
WORK_QUEUE * work_queue_head;
int work_queue_state=0;
//用来记录当前的后台下载/上传任务数
int work_queue_for_load=0;
sem_t sem;
//网络出错标志位，若被置一则会重连
int sync_err_flag=0;
//空闲时刻标志位
int free_flag=0;
//已登录标志
int login_flag=0;
//已登出标志
int logout=0;
int udpfd;
int skfd;
char name[MAX_NAME];
char passwd[MAX_PASSWD];
char game_pk_name[MAX_NAME];
char call[MAX_NAME]={0};
//聊天室登入标志
int chatflag=0;
int alarm_flag=0;
int game_accept_flag=0;
int game_not_accept=0;
int gaming_flag=0;
sqlite3 * db;
int countchat=0;
int countchat_second=0;
char doc_path[MAX_PATH];
