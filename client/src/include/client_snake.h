#ifndef __SNAKE_H
#define __SNAKE_H
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<time.h>
#include<pthread.h>
#include<termio.h>
#include<signal.h>

#define SNAKE_HEIGHT 20
#define SNAKE_WIDTH 50

#define LEFT 0
#define RIGHT 1
#define UP 2
#define DOWN 3

#define LOW_SPEED 320000
#define MID_SPEED 200000
#define HIGH_SPEED 160000

#define NORMAL_FOOD 0
#define STRANGE_FOOD 1
int speed;
int food;
int score;

typedef struct _snake_body{
	struct _snake_body * next;
	struct _snake_body * prev;
	int pos[2];
}BODY;

typedef struct _snake_head{
	BODY * tail;
	BODY * next;
	int pos[2];
	int direction;
}HEAD;

HEAD * snake_head;

pthread_mutex_t lock;

int platform[SNAKE_HEIGHT][SNAKE_WIDTH];
int foodpos[2];
int autoflag;
#endif
