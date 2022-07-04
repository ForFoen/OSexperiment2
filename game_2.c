#define __LIBRARY__
#include<stdio.h>
#include<unistd.h>
#include<errno.h>
#include<time.h>
#include<stdlib.h>
#define __NR_init_graphics 87
#define __NR_get_message 88
#define __NR_timer_create 89
#define __NR_paint 90

#define SCREEN_COLOR 3

#define MSG_MOUSE_CLICK 1
#define MSG_TIME 2

#define BIRD_X 30
#define BIRD_Y 100
#define BIRD_WIDTH 10
#define BIRD_HEIGHT 8
#define BIRD_COLOR 6
#define BIRD_UP_SPEED 6
#define BIRD_FALL_SPEED 4

#define OBJECT_WIDTH 15
#define OBJECT_HEIGHT 40
#define OBJECT_COLOR 12
#define OBJECT_POSX_L 50
#define OBJECT_POSX_R 150 
#define OBJECT_POSY_L 0 
#define OBJECT_POSY_R 150 
#define OBJECT_SPEED 4
typedef struct MESS{
	unsigned char mid; 
	int pid; 
	struct MESS *next; 
}message;

typedef struct usertimer{
	long jiffies;
	int type; 
	long init_jiffies;
	int pid; 
	struct usertimer *next;
}user_timer;

typedef struct OB{
    unsigned char posx;
    unsigned char posy;
    unsigned char width;
    unsigned char height;
	unsigned char color;
}object;

_syscall1(int,init_graphics,int,color)
_syscall1(int,get_message,unsigned char*,msg)
_syscall2(int,timer_create,long,milliseconds,int,type)
_syscall1(int,paint,unsigned char*,ob)
static unsigned long next=1;//静态全局变量，作为种子

void my_srand(unsigned long seed)//通过传不同的参数更改种子值，一般传time(NULL)
{
    next=seed;
}

int my_rand(void)//将srand更改过的种子值通过公式计算出结果作为随机值
{
    next = next * 1103515245 + 12345;
return((unsigned)(next/65536) % 32768);
}
void forpaint(object ob){
    unsigned char *c=(unsigned char *)malloc(sizeof(object));
    *c=ob.posx;
    *(c+1)=ob.posy;
    *(c+2)=ob.width;
    *(c+3)=ob.height;
    *(c+4)=ob.color;
    paint(c);
    free(c);
}
int main(){
    object objects[20];
    unsigned char *c;
    int i;
    objects[0].posx=BIRD_X;
    objects[0].posy=BIRD_Y;
    objects[0].width=BIRD_WIDTH;
    objects[0].height=BIRD_HEIGHT;
    objects[0].color=BIRD_COLOR;

    for(i=1;i<20;i++){
        objects[i].height=OBJECT_HEIGHT;
        objects[i].width=OBJECT_WIDTH;
        objects[i].posx=OBJECT_POSX_L+30*i;
        objects[i].posy=(i%2)?OBJECT_POSY_R:OBJECT_POSY_L;
        objects[i].color=OBJECT_COLOR;
    }
    init_graphics(SCREEN_COLOR);
    for(i=0;i<20;i++){
        forpaint(objects[i]);
    }
    timer_create(100,0);
    while(1){
        unsigned char *msg=(unsigned char *)malloc(sizeof(unsigned char));
        get_message(msg);
        if(*msg==MSG_MOUSE_CLICK){
            objects[0].color=SCREEN_COLOR;
            forpaint(objects[0]);
            objects[0].posy-=BIRD_UP_SPEED;
            objects[0].color=BIRD_COLOR;
            forpaint(objects[0]);
        }
        else if(*msg==MSG_TIME){
        	my_srand(time(NULL));
            objects[0].color=SCREEN_COLOR;
            forpaint(objects[0]);
            objects[0].posy+=BIRD_FALL_SPEED;
            objects[0].color=BIRD_COLOR;
            forpaint(objects[0]);
            for(i=1;i<20;i++){
                objects[i].color=SCREEN_COLOR;
                forpaint(objects[i]);
                objects[i].posx-=OBJECT_SPEED;
                objects[i].color=OBJECT_COLOR;
                forpaint(objects[i]);
                if((objects[i].posx+objects[i].width)<0){
                    objects[i].posx=150;
                    objects[i].posy=my_rand()%150;
                    sleep(1);
                }
            }
        }
        free(msg);
    }
    return 0;
}


