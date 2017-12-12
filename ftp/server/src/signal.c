#include"func.h"
void sig_handle2(int signum)
{
	//printf("I am SIGCHLD\n");
	while(waitpid(-1,NULL,WNOHANG)>0);  //信号函数调用最简单的接口，所以在执行信号的过程中有下一个信号产生是不可重入的;改进：sigaction
	//waitpid(-1,NULL,WNOHANG);
}
void signal_init()
{
	signal(SIGINT,sig_handle1);  //进程ctrl+c后要能删除创建的信号量
	signal(SIGCHLD,sig_handle2);  //客户端在执行操作，比如下载上传文件突然中断，要能够回收进程，防止出现僵尸进程
}