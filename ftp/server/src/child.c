#include"func.h"
//创建子进程，child_word是子进程（业务进程）while（1）一直做得事
void child_work(int fds,int log_fd)
{
	int new_fd;
	struct sockaddr_in client;
	int flag = 1;
	while(1)
	{
		//printf("wait read new_fd\n");
		read_fd(fds,&new_fd);  //子进程等待接收main进程发送过来的客户端链接请求产生的newfd;
		read(fds,&client,sizeof(client));   //main进程通过socketpair发送连接客户端的结构体信息过来，后面写日志要用到
		child_handle(new_fd,log_fd,client);    //子进程拿到连接客户端产出的new_fd，执行对应的操作
		write(fds,&flag,sizeof(int));  //子进程与客户端的交互完成，通过管道给main进程发送一个数据，触发epoll_wait,重置子进程状态等待新的客户端链接；
	}
}
void make_child(pchild p,int num,int log_fd)
{
	int fds[2];
	int ret;
	pid_t pid;
	for(int i=0;i<num;i++)
	{
		ret = socketpair(AF_LOCAL,SOCK_STREAM,0,fds);
		if(-1==ret)
		{
			perror("socketpair");
			return ;
		}
		pid = fork();
		if( 0==pid )
		{
			close(fds[1]);
			child_work(fds[0],log_fd);    //子任务要做的事,后面加入了记录日志文件，所以要传一个日志文件的描述符
		}
		close(fds[0]);
		p[i].fds = fds[1];
		p[i].pid = pid;
		p[i].status = 0;//刚创建好，待命状态，非忙碌，status=0;
	}
}
