# OSexperiment2
鼠标驱动和简单的图形接口实现

本实验根据《操作系统原理、实现与实践》进行操作

1.鼠标的驱动：这一步仅需根据书上的指示进行操作，编写mouse.S，对control_init()进行修改等，同时简单地编写readmouse()函数（具体地编写需要结合三四步）。

2.显示器的图形工作模式：同样根据书上的代码进行修改即可，添加一个系统调用，为后续的游戏做准备。

(前两步的完成并没有实际的效果，第二步的系统调用用于游戏代码，而不用于添加在main()函数中，仅可通过gdb模式简单感受鼠标中断的反馈）

3.消息驱动框架：该步骤与第四步制作flappy bird游戏紧密结合，目的在于记录游戏中的鼠标点击和时间推移，并传输这些信息用于改变场景，具体添加了sys_get_message()这个系统调用，并完善了readmouse(),代码主要位于kernel/sys.c和kernel/chr_drv/tty_io.c，这一步最好与第四步一起做。

4.可视化应用程序的设计与实现:添加了两个系统调用sys_timer_create()和sys_paint()，分别用于记录并传输时间信息和改变显示图像，主要代码位于kernel/sys.c、game.c、kernel/sched.c。
（宏与结构的定义位于include/linux/tty.h）

最终将game.c放入虚拟机中编译并运行，启动程序，将虚拟机左上角鼠标按钮解锁，即可进行游戏。
