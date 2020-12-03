#include<stdio.h>
int main()
{
		short *p,*q;
		short ar[10]={0};
		p=q=ar;
		p++;
		printf("%d\t",p-q);
		printf("%d\t",(char *)p-(char *)q);
		printf("%d\t",sizeof(ar)/sizeof(*ar));
		return 0;
}
