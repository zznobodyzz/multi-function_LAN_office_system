#ifndef _TETRIS
#define _TETRIS
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<time.h>
#include<signal.h>
#include<pthread.h>
#include<termio.h>
#define NORMAL 1300000
#define FAST 1000000
#define VISIBLE 0
#define INVISIBLE 1
#define WIDTH 18
#define HEIGHT 15
#define YES 1
#define NO 0
int (*surface)[WIDTH]=NULL;
pthread_mutex_t lock;
int next_shape=0;
int current_shape=0;
int pointp[8];
int score=0;
int enter_count=1;
int speed;
int mode;
#endif
