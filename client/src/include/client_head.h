#ifndef __HEAD_H
#define __HEAD_H
#include<time.h>
#include<sys/time.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<pthread.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sqlite3.h>
#include<string.h>
#include<signal.h>
#include<semaphore.h>
#include<dirent.h>

#define SERVER_TCP_PORT 8888
#define SERVER_UDP_PORT 8899
#define SERVER_IP_PORT 8877
#define MAX_NAME 32
#define MAX_DATA 512
#define MAX_CMD 512
#define MAX_TIME 24
#define MAX_ADDR 17
#define MAX_PASSWD 16
#define MAX_WORD 32
#define MAX_EXPL 256
#define MAX_FILENAME 32
#define MAX_PATH 90
#define MAX_WORK_QUEUE_TASK_NAME 20
#define MAX_WORK_QUEUE_TASK_NO 100
#define MAX_RETRY_TIMES 3
#define MAX_ADDR 17

#define REGISTER 1
#define LOGIN 2
#define LOGOUT 3
#define DIC 4
#define HIS 5
#define ONLINELIST 6
#define ONL_ALL 7
#define ONL_NAME 8
#define ALL_ALL 9
#define ALL_NAME 10
#define ONL_CEL 11
#define PWD 12
#define ADVCEL 13
#define DIC_END 14
#define HIS_ALL 15
#define HIS_SEF 16
#define MOD 17
#define MOD_YES 18
#define MOD_NO 19
#define SCORE 20
#define HIS_CEL 21
#define VDO 22
#define SDO 23
#define SDO_CEL 24
#define SDO_NAME_C 25
#define SDO_NAME_U 26
#define SDO_AUTH_C 27
#define SDO_AUTH_U 28
#define SDO_TIME 29
#define HDO 30
#define DDO 31
#define DDO_DDO 32
#define DDO_CEL 33
#define DDO_HIS 34
#define UDO 35
#define UDO_UDO 36
#define UDO_HIS 37
#define UDO_CEL 38
#define UDO_DOC 39
#define UDO_END 40
#define CDO 41
#define VDO_PRI 42
#define DDO_PRI 43
#define JCH 44
#define ECH 45
#define UDO_PRI 46
#define CHL 47
#define CFA 48
#define CFO 49
#define CHR 50
#define CRP 51
#define SCK 53
#define CAREMOD 54
#define SETCARE 55
#define SCR_CEL 56
#define CRP_CEL 57
#define DMD 58
#define SCR 59
#define CMD 60
#define DDO_TASK 63
#define UDO_TASK 64
#define EXIT 65
#define GET_SERVER_IP 66
#define UDO_AGA 67
#define UDO_DO_TASK 68

#define GET_REG 101
#define GET_LOG 102
#define GET_LOU 103
#define GET_DIC 104
#define GET_HIS 105
#define GET_VDO 106
#define GET_SDO 107
#define GET_HDO 108
#define GET_DDO 109
#define GET_UDO 110
#define GET_CDO 111
#define GET_CFA 112
#define GET_CFO 113
#define GET_CHR 114
#define GET_CRP 115
#define GET_SCK 116
#define GET_DMD 117
#define GET_SCR 118
#define GET_ONL 119
#define GET_PWD 120
#define GET_DVC 121
#define GET_SCO 122
#define GET_MOD 123
#define GET_CMD 124
#define GET_CHL 125
#define GET_ECH 126
#define GET_JCH 127
#define GET_DDO_TASK 128
#define GET_UDO_TASK 129
#define SERVER_IP 130

#define SER_ERR 999
#define REQ_DEN 998
#define SYS_INFO 997
#define ADM_INFO 996
#define BROA_INFO 995
#define PRIV_INFO 994

#define NAME_OK 200
#define NAME_ERROR 201
#define REG_OK 202
#define LOG_ERR 203
#define LOG_PWD 204
#define PWD_OK 205
#define LOG_OK 206
#define LOU_OK 207
#define DIC_ERR 208
#define DIC_OK 209
#define HIS_ERR 210
#define HIS_OK 211
#define HIS_END 212
#define HIS_UNA 213
#define HIS_ABL 214
#define VDO_OK 215
#define VDO_ERR 216
#define VDO_END 217
#define SDO_ERR 218
#define SDO_OK 219
#define SDO_END 220
#define SDO_DOC 221
#define SDO_OTH 222
#define HDO_OK 223
#define HDO_ERR 224
#define HDO_END 225
#define DDO_DOC 226
#define DDO_OK 227
#define DDO_ERR 228
#define DDO_END 229
#define UDO_OK 230
#define UDO_ERR 231
#define CDO_OK 232
#define CDO_CEL 233
#define CDO_LIST 234
#define CDO_ERR 235
#define CDO_END 236
#define CHL_END 237
#define CHL_ERR 238
#define CHL_OK 239
#define CFA_OK 240
#define CFA_ERR 241
#define CFA_CEL 242
#define PWD_ERR 243
#define PWD_NEW 244
#define ONL_ERR 245
#define ONL_OK 246
#define ONL_END 247
#define DVC_UNA 248
#define DVC_ABL 249
#define DVC_ON 250
#define DVC_OFF 251
#define SCR_ERR 252
#define SCR_OK 253
#define SCO_OK 254
#define MOD_LOW 255
#define MOD_HIG 256
#define MOD_ADM 257
#define MOD_UNA 258
#define MOD_ABL 259
#define MOD_OK 260
#define SCK_OK 261
#define SCK_NO 262
#define ECH_OK 263
#define JCH_OK 264
#define CFO_CEL 265
#define CFO_ERR 266
#define CFO_DATA 267
#define CFO_OK 268
#define CRP_OK 269
#define CRP_DATA 270
#define CRP_ERR 271
#define CHR_END 272
#define CHR_OK 273
#define CHR_ERR 274

typedef struct{
		int cmd;
		char name[MAX_NAME];
		char data[MAX_DATA];
}USR;

typedef struct{
		int sig;
		char data[MAX_DATA];
}SER;

typedef struct _work_queue{
		int no;
		int priority;
		int times;
		char task[MAX_WORK_QUEUE_TASK_NAME];
		char arg[64];
		struct _work_queue * next;
}WORK_QUEUE;

char work_queue_task_no[MAX_WORK_QUEUE_TASK_NO];
#endif
