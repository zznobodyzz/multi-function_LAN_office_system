#ifndef __GAME__H
#define __GAME__H

#define WUZIQI 1
#define TETRIS 2
#define SNAKE 3

#define GAME_CEL 600
#define GAME_INIT_ERR 601
#define GAME_READY 602
#define GIVEUP 603
#define AGAIN 604
#define UPLOAD_GAME 605

#define GAME_START 607
#define GAME_NOT_ACCEPT 608
#define GAME_UNABLE 609
#define GAME_ACCEPT 610
#define GAME_OVER 611
#define PK_ERR 612
#define SET_GAME_QUEUE_OK 613
#define SET_GAME_QUEUE_FAILURE 614
#define REGULAR 615
#define UPLOAD_GAME_READY 616
#define GET_UPLOAD_GAME 617
#define UPLOAD_GAME_ERR 618
#define UPLOAD_GAME_OK 619
#define RANK_VIEW 500
#define GET_RANK_VIEW 501
#define GET_WUZIQI_PK 502
#define RANK_VIEW_ERR 503
#define RANK_VIEW_YET 504
#define RANK_VIEW_OK 505
#define RANK_VIEW_OVER 506
#define WUZIQI_PK 507
#include<termios.h>
struct termios tm,tm_old;
#endif
