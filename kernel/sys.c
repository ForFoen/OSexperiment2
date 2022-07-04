/*
 *  linux/kernel/sys.c
 *
 *  (C) 1991  Linus Torvalds
 */

#include <errno.h>
#include <asm/io.h>
#include <linux/sched.h>
#include <linux/tty.h>
#include <linux/kernel.h>
#include <asm/segment.h>
#include <sys/times.h>
#include <sys/utsname.h>

int sys_ftime()
{
	return -ENOSYS;
}

int sys_break()
{
	return -ENOSYS;
}

int sys_ptrace()
{
	return -ENOSYS;
}

int sys_stty()
{
	return -ENOSYS;
}

int sys_gtty()
{
	return -ENOSYS;
}

int sys_rename()
{
	return -ENOSYS;
}

int sys_prof()
{
	return -ENOSYS;
}

int sys_setregid(int rgid, int egid)
{
	if (rgid>0) {
		if ((current->gid == rgid) || 
		    suser())
			current->gid = rgid;
		else
			return(-EPERM);
	}
	if (egid>0) {
		if ((current->gid == egid) ||
		    (current->egid == egid) ||
		    suser()) {
			current->egid = egid;
			current->sgid = egid;
		} else
			return(-EPERM);
	}
	return 0;
}

int sys_setgid(int gid)
{
/*	return(sys_setregid(gid, gid)); */
	if (suser())
		current->gid = current->egid = current->sgid = gid;
	else if ((gid == current->gid) || (gid == current->sgid))
		current->egid = gid;
	else
		return -EPERM;
	return 0;
}

int sys_acct()
{
	return -ENOSYS;
}

int sys_phys()
{
	return -ENOSYS;
}

int sys_lock()
{
	return -ENOSYS;
}

int sys_mpx()
{
	return -ENOSYS;
}

int sys_ulimit()
{
	return -ENOSYS;
}

int sys_time(long * tloc)
{
	int i;

	i = CURRENT_TIME;
	if (tloc) {
		verify_area(tloc,4);
		put_fs_long(i,(unsigned long *)tloc);
	}
	return i;
}

/*
 * Unprivileged users may change the real user id to the effective uid
 * or vice versa.
 */
int sys_setreuid(int ruid, int euid)
{
	int old_ruid = current->uid;
	
	if (ruid>0) {
		if ((current->euid==ruid) ||
                    (old_ruid == ruid) ||
		    suser())
			current->uid = ruid;
		else
			return(-EPERM);
	}
	if (euid>0) {
		if ((old_ruid == euid) ||
                    (current->euid == euid) ||
		    suser()) {
			current->euid = euid;
			current->suid = euid;
		} else {
			current->uid = old_ruid;
			return(-EPERM);
		}
	}
	return 0;
}

int sys_setuid(int uid)
{
/*	return(sys_setreuid(uid, uid)); */
	if (suser())
		current->uid = current->euid = current->suid = uid;
	else if ((uid == current->uid) || (uid == current->suid))
		current->euid = uid;
	else
		return -EPERM;
	return(0);
}

int sys_stime(long * tptr)
{
	if (!suser())
		return -EPERM;
	startup_time = get_fs_long((unsigned long *)tptr) - jiffies/HZ;
	return 0;
}

int sys_times(struct tms * tbuf)
{
	if (tbuf) {
		verify_area(tbuf,sizeof *tbuf);
		put_fs_long(current->utime,(unsigned long *)&tbuf->tms_utime);
		put_fs_long(current->stime,(unsigned long *)&tbuf->tms_stime);
		put_fs_long(current->cutime,(unsigned long *)&tbuf->tms_cutime);
		put_fs_long(current->cstime,(unsigned long *)&tbuf->tms_cstime);
	}
	return jiffies;
}

int sys_brk(unsigned long end_data_seg)
{
	if (end_data_seg >= current->end_code &&
	    end_data_seg < current->start_stack - 16384)
		current->brk = end_data_seg;
	return current->brk;
}

/*
 * This needs some heave checking ...
 * I just haven't get the stomach for it. I also don't fully
 * understand sessions/pgrp etc. Let somebody who does explain it.
 */
int sys_setpgid(int pid, int pgid)
{
	int i;

	if (!pid)
		pid = current->pid;
	if (!pgid)
		pgid = current->pid;
	for (i=0 ; i<NR_TASKS ; i++)
		if (task[i] && task[i]->pid==pid) {
			if (task[i]->leader)
				return -EPERM;
			if (task[i]->session != current->session)
				return -EPERM;
			task[i]->pgrp = pgid;
			return 0;
		}
	return -ESRCH;
}

int sys_getpgrp(void)
{
	return current->pgrp;
}

int sys_setsid(void)
{
	if (current->leader && !suser())
		return -EPERM;
	current->leader = 1;
	current->session = current->pgrp = current->pid;
	current->tty = -1;
	return current->pgrp;
}

int sys_getgroups()
{
	return -ENOSYS;
}

int sys_setgroups()
{
	return -ENOSYS;
}

int sys_uname(struct utsname * name)
{
	static struct utsname thisname = {
		"linux .0","nodename","release ","version ","machine "
	};
	int i;

	if (!name) return -ERROR;
	verify_area(name,sizeof *name);
	for(i=0;i<sizeof *name;i++)
		put_fs_byte(((char *) &thisname)[i],i+(char *) name);
	return 0;
}

int sys_sethostname()
{
	return -ENOSYS;
}

int sys_getrlimit()
{
	return -ENOSYS;
}

int sys_setrlimit()
{
	return -ENOSYS;
}

int sys_getrusage()
{
	return -ENOSYS;
}

int sys_gettimeofday()
{
	return -ENOSYS;
}

int sys_settimeofday()
{
	return -ENOSYS;
}


int sys_umask(int mask)
{
	int old = current->umask;

	current->umask = mask & 0777;
	return (old);
}

static int is_set=0;
int sys_init_graphics(int color)
{
	int i;
	char *p=vga_graph_memstart;
	if(!is_set){
		outb(0x05,0x3CE);
		outb(0x40,0x3CF);//设置256色，且取出方式为移动拼装
		outb(0x06,0x3CE);
		outb(0x05,0x3CF);//设置显存的地址区域，禁止字符模式
		outb(0x04,0x3C4);
		outb(0x08,0x3C5);//设定将四个显存片连在一起


		outb(0x01,0x3D4);
		outb(0x4F,0x3D5);//设置End Horizontal Display 为79
		outb(0x03,0x3D4);
		outb(0x82,0x3D5);//设置Display Enable Skew 为0

		outb(0x07,0x3D4);
		outb(0x1F,0x3D5);//设置Vertical Display End 第8、9位为1、0
		outb(0x12,0x3D4);
		outb(0x8F,0x3D5);//设置Vertical Display End 低7位为0x8F
		outb(0x17,0x3D4);
		outb(0xA3,0x3D5);//设置SLDIV=1，将Scanline clock除以2

		outb(0x14,0x3D4);
		outb(0x40,0x3D5);//设置DW=1
		outb(0x13,0x3D4);
		outb(0x28,0x3D5);//设置Offset=40

		outb(0x0C,0x3D4);//设置Start Address高八位
		outb(0x00,0x3D5);//
		outb(0x0D,0x3D4);//设置Start Address低八位
		outb(0x00,0x3D5);//将Start Address设置为0xA0000
		is_set=1;
	}
	
	for(i=0;i<vga_graph_memsize;i++) *p++=color;//将背景颜色设为蓝绿色
	
	return 0;
}

message *message_head=NULL;
message *message_tail=NULL;
int sys_get_message(unsigned char *msg)
{
	if(!message_head){ 
		put_fs_byte(0,msg);
		return 1;
	}
	message *m=message_head;
	message_head=message_head->next;
	put_fs_byte(m->mid,msg);
	free(m);
	return 1;
}
user_timer *timer_head=NULL;
int sys_timer_create(long milliseconds,int type)
{
	long jiffies = milliseconds / 10;//Linux时钟是每个jiffies（滴答）（10毫秒）中断一次
	user_timer *timer = (user_timer*)malloc(sizeof(user_timer));
	timer->init_jiffies=jiffies;
	timer->jiffies=jiffies;
	timer->next=timer_head;
	timer->type=type;
	timer->pid=-1;
	timer_head=timer;
	return 1;
}

int sys_paint(unsigned char *ob){
	int i,j,x,y,w,h,color;
	x=get_fs_byte(ob);
	y=get_fs_byte(ob+1);
	w=get_fs_byte(ob+2);
	h=get_fs_byte(ob+3);
	color=get_fs_byte(ob+4);
	w+=x;
	h+=y;
	char *p;
	for(i=x;i<=w;i++){
		for(j=y;j<=h;j++){
			p=(char *)vga_graph_memstart+j*vga_width+i;
			*p=color;
		}
	}
	return 1;
}