#include"func.h"
//发送，接收文件，判断文件是否存在，或者存在部分（file.mh）
int is_file_exist_tmp(char *filename)
{
	char *pwd = getcwd(NULL,0);
	DIR *dir = opendir(pwd);
	if(NULL==dir)
	{
		perror("opendir");
		return -2;  //means fail
	}
	char tmp[256] = {0};
	strcpy(tmp,filename);
	strcat(tmp,".mh");
	struct dirent *q = NULL;
	while((q=readdir(dir))!=NULL)
	{ 
		if(strcmp(q->d_name,tmp)==0)
		{
			return 1;   //返回1表示文件下载到一半没下载好，可以继续接着下载
		}
	} 
	return -1;
}
int is_file_exist(char *filename)
{
	char *pwd = getcwd(NULL,0);
	DIR *dir = opendir(pwd);
	if(NULL==dir)
	{
		perror("opendir");
		return -2;  //means fail
	}
	struct dirent *q = NULL;
	while((q=readdir(dir))!=NULL)
	{
		if(strcmp(q->d_name,filename)==0)
		{
			return 0;
		}
	}
	return -1;
}
int recv_file(int socket_fd)
{
	char file_name[256] = {0};
	int file_len;
	recv_n(socket_fd,(char*)&file_len,sizeof(int));
	recv_n(socket_fd,file_name,file_len);   //先接收文件名字
	if(1==(is_file_exist_tmp(file_name)))
	{
		char file_tmp[256] = {0};
		sprintf(file_tmp,"%s%s",file_name,".mh");
		int fd = open(file_tmp,O_APPEND|O_RDWR); //这里加了定位偏移到文件尾，但是后面保险还是用了lseek
		if(-1==fd)
		{
			perror("tmp_open");
			return -1;
		}
		struct stat s;
		bzero(&s,sizeof(s));
		fstat(fd,&s);
		send_n(socket_fd,(char*)&s.st_size,sizeof(int));   //文件没发送完就断开，接收端收到的是一个.mh结尾的临时文件，现在断点续传，要发送文件已收到的长度  
		//lseek(fd,s.st_size,SEEK_SET);  //文件偏移到文件尾；
		printf("file already accepted %ld ,keep recving！\n",s.st_size);
		data d;
		while(1)
		{
			bzero(&d,sizeof(d));
			recv_n(socket_fd,(char*)&d.len,sizeof(int));
			if(0==d.len)  //接收到对面发送的0就表示对端文件发送完毕了
			{ 
				break;
			}
			recv_n(socket_fd,(char*)d.buf,d.len);
			write(fd,d.buf,d.len);		
		}
		close(fd);
		char cmd[256] = {0};
		sprintf(cmd,"%s %s %s","mv",file_tmp,file_name);
		system(cmd);
		printf("file \"%s\" recv success!\n",file_name);
	}
	else 
	{
		char tmp_file[256] = {0};
		sprintf(tmp_file,"%s%s",file_name,".mh");
		int fd = open(tmp_file,O_CREAT|O_WRONLY|O_APPEND,0666);
		if(-1==fd)
		{
			perror("open");
			printf("file \"%s\" recv fail!\n",file_name);
			return -1;
		}
		printf("recving file!\n");
		int file_len = 0;
		send_n(socket_fd,(char*)&file_len,sizeof(int));//不存在接收了部分的，这里就发个0过去，告诉对端发送文件从头开始，偏移0
		data d;
		while(1)
		{
			bzero(&d,sizeof(d));
			recv_n(socket_fd,(char*)&d.len,sizeof(int));
			if(0==d.len)  //接收到对面发送的0就表示对端文件发送完毕了
			{
				break;
			}
			recv_n(socket_fd,(char*)d.buf,d.len);
			write(fd,d.buf,d.len);
		}
		close(fd);
		char cmd[256] = {0};
		sprintf(cmd,"%s %s %s","mv",tmp_file,file_name);
		system(cmd);
		printf("file \"%s\" recv success!\n",file_name);
	}
	return 0;
}
int send_file(int socket_fd,char *file_name)
{
	int file_len = strlen(file_name);
	send_n(socket_fd,(char*)&file_len,sizeof(int));   //发送文件名长度
	send_n(socket_fd,file_name,file_len);      //发送文件名
	int fd = open(file_name,O_RDONLY);
	if(-1==fd)
	{
		perror("open");
		printf("send %s fail!\n",file_name);
		return -1;
	}
	int file_offset_len ;
	recv_n(socket_fd,(char*)&file_offset_len,sizeof(int));  //确定文件要偏移到哪里开始发（断点续传）
	int ret = lseek(fd,file_offset_len,SEEK_SET);
	if(-1==ret)
	{
		perror("lseek");
		return -1;
	}
	printf("sending ... \n");
	data d;
	while(bzero(&d,sizeof(d)),(d.len=read(fd,d.buf,sizeof(d.buf)))>0)
	{
		send_n(socket_fd,(char*)&d.len,sizeof(int));
		send_n(socket_fd,d.buf,d.len);
	}
	int endflag = 0;
	send_n(socket_fd,(char*)&d.len,sizeof(int));  //文件发送完了就发送一个0，对面读到就知道 这边文件已经发送结束
	printf("send %s success!\n",file_name);
	close(fd);
	return 0;
}