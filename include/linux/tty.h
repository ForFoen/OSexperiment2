/*
 * 'tty.h' defines some structures used by tty_io.c and some defines.
 *
 * NOTE! Don't touch this without checking that nothing in rs_io.s or
 * con_io.s breaks. Some constants are hardwired into the system (mainly
 * offsets into 'tty_queue'
 */

#ifndef _TTY_H
#define _TTY_H

#include <termios.h>

#define TTY_BUF_SIZE 1024

struct tty_queue {
	unsigned long data;
	unsigned long head;
	unsigned long tail;
	struct task_struct * proc_list;
	char buf[TTY_BUF_SIZE];
};

#define INC(a) ((a) = ((a)+1) & (TTY_BUF_SIZE-1))
#define DEC(a) ((a) = ((a)-1) & (TTY_BUF_SIZE-1))
#define EMPTY(a) ((a).head == (a).tail)
#define LEFT(a) (((a).tail-(a).head-1)&(TTY_BUF_SIZE-1))
#define LAST(a) ((a).buf[(TTY_BUF_SIZE-1)&((a).head-1)])
#define FULL(a) (!LEFT(a))
#define CHARS(a) (((a).head-(a).tail)&(TTY_BUF_SIZE-1))
#define GETCH(queue,c) \
(void)({c=(queue).buf[(queue).tail];INC((queue).tail);})
#define PUTCH(c,queue) \
(void)({(queue).buf[(queue).head]=(c);INC((queue).head);})

#define INTR_CHAR(tty) ((tty)->termios.c_cc[VINTR])
#define QUIT_CHAR(tty) ((tty)->termios.c_cc[VQUIT])
#define ERASE_CHAR(tty) ((tty)->termios.c_cc[VERASE])
#define KILL_CHAR(tty) ((tty)->termios.c_cc[VKILL])
#define EOF_CHAR(tty) ((tty)->termios.c_cc[VEOF])
#define START_CHAR(tty) ((tty)->termios.c_cc[VSTART])
#define STOP_CHAR(tty) ((tty)->termios.c_cc[VSTOP])
#define SUSPEND_CHAR(tty) ((tty)->termios.c_cc[VSUSP])

struct tty_struct {
	struct termios termios;
	int pgrp;
	int stopped;
	void (*write)(struct tty_struct * tty);
	struct tty_queue read_q;
	struct tty_queue write_q;
	struct tty_queue secondary;
	};

extern struct tty_struct tty_table[];

/*	intr=^C		quit=^|		erase=del	kill=^U
	eof=^D		vtime=\0	vmin=\1		sxtc=\0
	start=^Q	stop=^S		susp=^Z		eol=\0
	reprint=^R	discard=^U	werase=^W	lnext=^V
	eol2=\0
*/
#define INIT_C_CC "\003\034\177\025\004\0\1\0\021\023\032\0\022\017\027\026\0"
void rs_init(void);
void con_init(void);
void tty_init(void);

int tty_read(unsigned c, char * buf, int n);
int tty_write(unsigned c, char * buf, int n);

void rs_write(struct tty_struct * tty);
void con_write(struct tty_struct * tty);

void copy_to_cooked(struct tty_struct * tty);

//..........................................................
#define MSG_MOUSE_CLICK 1
#define MSG_TIME 2
#define vga_graph_memstart 0xA0000
#define vga_graph_memsize 64000
#define cursor_side 6
#define vga_width 320
#define vga_height 200
typedef struct MESS{
	unsigned char mid; //��Ϣ��ID
	int pid; //��Ϣ��Ŀ�����ID������ǵ�ǰ���̣���ֵ����Ϊ-1
	struct MESS *next; //�γ���Ϣ����Ҫ�õ���Ϣ
}message;

typedef struct usertimer{
	long jiffies;
	int type; //����Ϊ1��ʾֻ������1�����ӣ�0��ʾ���������������� 
	long init_jiffies;
	int pid; //�ĸ����̴����Ķ�ʱ��
	struct usertimer *next;
}user_timer;

typedef struct OB{
    unsigned char posx;//�������Ͻǵ�x����
    unsigned char posy;//�������Ͻǵ�y����
    unsigned char width;//����Ŀ��
    unsigned char height;//����ĸ߶�
	unsigned char color;//�������ɫ
}object;

extern void post_message(message *msg);
extern message *message_head;
extern message *message_tail;
extern user_timer *timer_head;
#endif
