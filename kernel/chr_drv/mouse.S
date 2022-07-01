#include <linux/config.h>
.globl mouse_interrupt
mouse_interrupt:
	pushl %eax
	pushl %ebx
	pushl %ecx
	pushl %edx
	push %ds
	
	movl $0x10,%eax
	mov %ax,%ds
	xor %eax,%eax
	inb $0x60,%al
	pushl %eax
	call readmouse
    addl $4,%esp
	
    movb $0x20,%al //8259A 操作命令字 EOI
    outb %al,$0xA0 //向 8259A 从片发送 EOI
    outb %al,$0x20 //向 8259A 主片发送 EOI
	
	pop %ds
	popl %edx
	popl %ecx
	popl %ebx
	popl %eax
	iret