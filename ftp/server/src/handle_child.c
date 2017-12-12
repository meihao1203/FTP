#include"func.h"
//ftp的所以功能,child_handle函数来和连接客户端进行交互，接收客户端发送过来的命令，解析出具体参数，然后调用具体的命令接口函数执行
void do_cd(int new_fd,pcmd pc)
{
	char dir[MAXSIZE];
	bzero(dir,sizeof(dir));
	sscanf(pc->cmd_buf+3,"%s",dir);
	chdir(dir);
	//	bzero(dir,sizeof(dir));
	//	getcwd(dir,sizeof(dir));
	//	int len = strlen(dir);
	//	send(new_fd,&len,sizeof(int),0);
	//	send_n(new_fd,dir,len);
}
void do_ls(int new_fd,pcmd pc)
{
	char *pwd = getcwd(NULL,0);
	DIR *dir = NULL;
	dir = opendir(pwd);
	if(NULL==dir)
	{
		perror("opendir");
		return ;
	}
	struct dirent *q = NULL;
	off_t I;
	while( (q=readdir(dir))!=NULL )
	{
		if((strcmp(q->d_name,".")!=0)&&(strcmp(q->d_name,"..")!=0))
		{
			bzero(pc,sizeof(cmd));
			file_stat(pc->cmd_buf,q->d_name);  //传入数组，获取ls -ll命令能显示的所有信息
			//printf("%s\n",pc->cmd_buf);
			pc->cmd_len = strlen(pc->cmd_buf);
			if(q->d_type==4)   //那个文件宏定义判断是不是目录有问题，其他正常
			{
				pc->cmd_buf[0] = 'd';
			}
			send(new_fd,&pc->cmd_len,sizeof(int),0);
			send_n(new_fd,pc->cmd_buf,pc->cmd_len);
		}
	}
	int end = 0;
	send(new_fd,&end,sizeof(int),0); //发送0，对方拿到就知道这边数据发送完了
}
void do_puts(int new_fd,pcmd pc)
{
	int flag;
	recv(new_fd,&flag,sizeof(int),0);
	if(0==flag)  //对面确定要发送的文件存在会先发一个0
	{
		recv_file(new_fd);
	}
	else //对面发送-1
	{
		return;
	}
}
void do_gets(int new_fd,pcmd pc)
{

	char gets_name[256] = {0};
	sscanf(pc->cmd_buf+5,"%s",gets_name);
	int flag;
	if(0==(is_file_exist(gets_name)))  //判断服务器上是否存在客户端要下载的这个文件
	{
		struct stat s;
		bzero(&s,sizeof(s));
		int ret = stat(gets_name,&s);
		if(-1==ret)
		{
			perror("stat");
			return ;
		}
		if(s.st_size<MMAPSIZE)
		{
			flag = 0;   //存在，发送0，通知客户端准备接受文件数据
			send_n(new_fd,(char *)&flag,sizeof(int));
			send_file(new_fd,gets_name);   //发送文件
		}
		else
		{
			flag = 0;  
			send_n(new_fd,(char *)&flag,sizeof(int));
			mmap_send_file(new_fd,gets_name);   //发送文件
		}
	}
	else 
	{
		flag = -1;  //不存在，发送-1，通知客户端文件所要下载的文件不存在
		send(new_fd,&flag,sizeof(int),0);
	}
}
void do_remove(int new_fd,pcmd pc)
{
	char re_name[256] = {0};
	sscanf(pc->cmd_buf+7,"%s",re_name);
	int flag;
	if(0==(is_file_exist(re_name)))
	{
		char cmd[256] = {0};
		sprintf(cmd,"rm -rf %s",re_name);
		system(cmd);   //直接传递字符串就不用加冒号了，如果是字符串常量就要有冒号"rm ..."
		flag = 0;
		send(new_fd,&flag,sizeof(int),0);
	}
	else
	{
		flag = -1;
		send(new_fd,&flag,sizeof(int),0);
	}
}
void do_pwd(int new_fd,pcmd pc)
{
	char pwd[256] = {0};
	getcwd(pwd,sizeof(pwd));
	int pwd_len = strlen(pwd);
	send(new_fd,&pwd_len,sizeof(int),0);
	send_n(new_fd,pwd,pwd_len);
}
void do_mkdir(int new_fd,pcmd pc)
{
	char mkdir_name[256] = {0};
	sscanf(pc->cmd_buf+6,"%s",mkdir_name);
	int flag;
	flag = mkdir(mkdir_name,0777);
	send(new_fd,&flag,sizeof(int),0);   //成功返回0，失败返回-1;客户端拿到判断就好了
}
void child_handle(int new_fd,int log_fd,struct sockaddr_in client)
{
	/*获取信号量对日志文件进行读写操作*/
	key_t semid = semget((key_t)KEY,1,IPC_CREAT|0600);   //main进程已经创建，这里就是获取
	struct sembuf sp,sv;
	bzero(&sp,sizeof(sp));
	bzero(&sv,sizeof(sv));
	sp.sem_num = 0;
	sp.sem_op = 1;
	sp.sem_flg = SEM_UNDO;

	sv.sem_num = 0;
	sv.sem_op = -1;
	sv.sem_flg = SEM_UNDO;

	char log_buf[1024] = {0};

	pcmd pc = (pcmd)calloc(1,sizeof(cmd));
	int recv_ret;

	//密码验证,必须在超级权限下运行
	int flag = authentication(new_fd);
	if(-1==flag)
	{
		close(new_fd); 
		free(pc);
		pc = NULL;
		printf("client connect reject!\n");
		bzero(log_buf,sizeof(log_buf));
		sprintf(log_buf,"client IP = %s , port = %d\n%s\n",inet_ntoa(client.sin_addr),ntohs(client.sin_port),"client connect reject!");
		semop(semid,&sp,1);
		log_write(log_fd,log_buf);
		semop(semid,&sv,1);
		return ;
	}
	printf("client login success!\n");
	bzero(log_buf,sizeof(log_buf));
	sprintf(log_buf,"client IP = %s , port = %d\n%s\n",inet_ntoa(client.sin_addr),ntohs(client.sin_port),"client login success!");
	semop(semid,&sp,1);
	log_write(log_fd,log_buf);
	semop(semid,&sv,1);
	while(1)
	{
		bzero(pc,sizeof(cmd));
		recv_ret = recv(new_fd,&pc->cmd_len,sizeof(int),0);
		if(recv_ret==0||pc->cmd_len==0)
		{
			bzero(log_buf,sizeof(log_buf));
			sprintf(log_buf,"client IP = %s , port = %d\n%s\n",inet_ntoa(client.sin_addr),ntohs(client.sin_port),"client exit!");
			semop(semid,&sp,1);
			log_write(log_fd,log_buf);
			semop(semid,&sv,1);
			printf("client exit!\n");
			//close(new_fd);
			free(pc);
			pc = NULL;
			return ;
		}
		recv_n(new_fd,pc->cmd_buf,pc->cmd_len);
		if(strncmp("cd",pc->cmd_buf,2)==0)
		{	
			bzero(log_buf,sizeof(log_buf));
			sprintf(log_buf,"client IP = %s , port = %d\ncmd: %s\n",inet_ntoa(client.sin_addr),ntohs(client.sin_port),pc->cmd_buf);
			semop(semid,&sp,1);
			log_write(log_fd,log_buf);
			semop(semid,&sv,1);
			do_cd(new_fd,pc);
		}
		else if(strncmp("ls",pc->cmd_buf,2)==0)
		{
			bzero(log_buf,sizeof(log_buf));
			sprintf(log_buf,"client IP = %s , port = %d\ncmd: %s\n",inet_ntoa(client.sin_addr),ntohs(client.sin_port),pc->cmd_buf);
			semop(semid,&sp,1);
			log_write(log_fd,log_buf);
			semop(semid,&sv,1);
			do_ls(new_fd,pc);
		}
		else if(strncmp("puts",pc->cmd_buf,4)==0)
		{
			bzero(log_buf,sizeof(log_buf));
			sprintf(log_buf,"client IP = %s , port = %d\ncmd: %s\n",inet_ntoa(client.sin_addr),ntohs(client.sin_port),pc->cmd_buf);
			semop(semid,&sp,1);
			log_write(log_fd,log_buf);
			semop(semid,&sv,1);
			do_puts(new_fd,pc);
		}
		else if(strncmp("gets",pc->cmd_buf,4)==0)
		{
			bzero(log_buf,sizeof(log_buf));
			sprintf(log_buf,"client IP = %s , port = %d\ncmd: %s\n",inet_ntoa(client.sin_addr),ntohs(client.sin_port),pc->cmd_buf);
			semop(semid,&sp,1);
			log_write(log_fd,log_buf);
			semop(semid,&sv,1);
			do_gets(new_fd,pc);
		}
		else if(strncmp("remove",pc->cmd_buf,6)==0)
		{
			bzero(log_buf,sizeof(log_buf));
			sprintf(log_buf,"client IP = %s , port = %d\ncmd: %s\n",inet_ntoa(client.sin_addr),ntohs(client.sin_port),pc->cmd_buf);
			semop(semid,&sp,1);
			log_write(log_fd,log_buf);
			semop(semid,&sv,1);
			do_remove(new_fd,pc);
		}
		else if(strncmp("pwd",pc->cmd_buf,3)==0)
		{
			bzero(log_buf,sizeof(log_buf));
			sprintf(log_buf,"client IP = %s , port = %d\ncmd: %s\n",inet_ntoa(client.sin_addr),ntohs(client.sin_port),pc->cmd_buf);
			semop(semid,&sp,1);
			log_write(log_fd,log_buf);
			semop(semid,&sv,1);
			do_pwd(new_fd,pc);
		}
		else if(strncmp("mkdir",pc->cmd_buf,5)==0)
		{
			bzero(log_buf,sizeof(log_buf));
			sprintf(log_buf,"client IP = %s , port = %d\ncmd: %s\n",inet_ntoa(client.sin_addr),ntohs(client.sin_port),pc->cmd_buf);
			semop(semid,&sp,1);
			log_write(log_fd,log_buf);
			semop(semid,&sv,1);
			do_mkdir(new_fd,pc);
		}
	}
}
