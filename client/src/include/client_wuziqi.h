#ifndef __WUZIQI__H
#define __WUZIQI__H

#define FIRST 1
#define SECOND 2
#define WUZIQI_LENGTH 12
typedef struct square{
		int pos[2];
		int state;
		struct square * up;
		struct square * down;
		struct square * left;
		struct square * right;
}POINT;
int order=0;


#endif

