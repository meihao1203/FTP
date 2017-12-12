#include"func.h"
//ftp是用进程池实现的，可以同时服务num(在func.h中定义)个客户端,日志文件只有一个，所以具体写操作的时候要用信号量pv操作保证一致性
void log_write(int log_fd,char *log_buf)
{
	time_t t;
	time(&t);
	char *str = ctime(&t);
	char div[100] = "----------------------------------------------------------\n";
	//semop(semid,&sp,1);
	write(log_fd,str,strlen(str));
	write(log_fd,log_buf,strlen(log_buf));
	write(log_fd,div,strlen(div));
	//semop(semid,&sv,1);
}
