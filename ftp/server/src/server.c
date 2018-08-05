#include"func.h"
//server端的main函数，创建信号量，业务进程，epoll监听
static key_t semid;
void sig_handle1(int signum) 
{
	//printf("I am SIGINT\n");
	semctl(semid,0,IPC_RMID,NULL);   //因为这个注册函数用到了semid，所以只能放到这里了
	exit(0);  
}
int main(int argc,char **argv)
{
	if(2!=argc)
	{
		printf("error argcs\n");
		return -1;
	}
	semid = semget((key_t)KEY,1,IPC_CREAT|0600); 
	if(-1==semid)
	{  
		perror("semid"); 
		return -1;  
	} 
	int ret = semctl(semid,0,SETVAL,1);   
	if(-1==ret)
	{ 
		perror("semctl");
		return -1;   
	}
	
	struct sembuf sp,sv;  
	bzero(&sp,sizeof(sp)); 
	bzero(&sv,sizeof(sv));
	sp.sem_num = 0;  
	sp.sem_op = 1; 
	sp.sem_flg = SEM_UNDO;  

	sv.sem_num = 0;    
	sv.sem_op = -1;    
	sv.sem_flg = SEM_UNDO;    

	signal_init();

	char ip[32];
	char port[32];
	file_read_arg(argv[1],ip,port);  //从配置文件中读取服务器ip地址和端口
	int sfd = socket(AF_INET,SOCK_STREAM,0);
	if(-1==sfd)
	{
		perror("socket");
		return -1;
	}
	struct sockaddr_in ser;
	bzero(&ser,sizeof(ser));
	ser.sin_port = htons(atoi(port));
	ser.sin_addr.s_addr = inet_addr(ip);
	ser.sin_family = AF_INET;
	ret = bind(sfd,(struct sockaddr*)&ser,sizeof(ser));
	if(-1==ret)
	{
		perror("bind");
		return -1;
	}
	int num = 5;
	ret = listen(sfd,num);
	if(-1==ret)
	{
		perror("listen");
		return -1;
	}
	//注册描述符、sfd和创建好的子进程对应的父进程端的管道
	int epfd  = epoll_create(1);
	if(-1==epfd)
	{
		perror("epoll_create");
		return -1;
	}
	struct epoll_event eve,eves[num+1];  //num个连接到子进程的管道和一个sfd
	bzero(&eve,sizeof(eve));
	eve.events = EPOLLIN;
	eve.data.fd = sfd;
	ret = epoll_ctl(epfd,EPOLL_CTL_ADD,sfd,&eve);
	if(-1==ret)
	{
		perror("epoll_ctl");
		return -1;
	}
	int log_fd = open("./log/log",O_CREAT|O_WRONLY|O_APPEND,0600);   //打开日志文件记录
	if(-1==log_fd)
	{
		printf("%s\n","\"log\" file can not find , should run in the server folder");
		perror("open");
		return -1;
	}
	//信号量pv操作对log文件操作
	//创建子进程
	pchild p = (pchild)calloc(num,sizeof(child));
	make_child(p,num,log_fd);    //这个文件描述符是在创建子进程前面打开的，所以这里可以不用传递log_fd描述符，但是为了后面程序可读性，还是给传递过去了
	for(int i=0;i<num;i++)
	{
		bzero(&eve,sizeof(eve));
		eve.events = EPOLLIN;
		eve.data.fd = p[i].fds;
		epoll_ctl(epfd,EPOLL_CTL_ADD,p[i].fds,&eve);
	}	
	int new_fd;
	int flag;
	int j = 0;
	int i;
	struct sockaddr_in client;
	memset(&client,0,sizeof(client));
	int addrlen = sizeof(client);
	char log_buf[256] = {0};
	while(1)
	{
		memset(eves,0,(num+1)*sizeof(eve));
		ret = epoll_wait(epfd,eves,num+1,-1);
		if(ret>0)
		{
			for(i=0;i<ret;i++)
			{
				if(eves[i].data.fd == sfd)
				{
					new_fd = accept(sfd,(struct sockaddr*)&client,&addrlen);
					bzero(log_buf,sizeof(log_buf));
					sprintf(log_buf,"client IP = %s , port = %d\n%s\n",inet_ntoa(client.sin_addr),ntohs(client.sin_port),"client login!");
					semop(semid,&sp,1);   //多个进程要来对日志文件写操作，所以必须互斥访问
					log_write(log_fd,log_buf);
					semop(semid,&sv,1);
					for(j=0;j<num;j++)
					{
						if(p[j].status == 0)
						{
							break;
						}
					}
					send_fd(p[j].fds,new_fd);
					write(p[j].fds,&client,sizeof(client));  //用进程间的管道把客户端信息发过去
					p[j].status = 1;
					close(new_fd);  //new_fd已经发送给空闲子进程，main进程不在需要new_fd
				}
				for(j=0;j<num;j++)
				{
					if(eves[i].data.fd == p[j].fds)
					{
						read(p[j].fds,&flag,sizeof(int));
						p[j].status = 0;
					}
				}
			}
		}
	}
	return 0;
}
