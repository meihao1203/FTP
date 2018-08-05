#include"func.h"
//ftp后期考虑提升效率，减少磁盘操作，用mmap内存映射来时间文件传输
int mmap_send_file(int socket_fd,char *file_name)
{
	printf("mmap send file \"%s\"\n",file_name);
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
	struct stat s;
	bzero(&s,sizeof(s));
	fstat(fd,&s);
	int file_size = s.st_size;    //得到待发送的整个文件的大小
	char *file_point = (char*)mmap(NULL,file_size,PROT_READ,MAP_SHARED,fd,0);
	if(MAP_FAILED==file_point)
	{
		perror("mmap");
		return -1;
	}
	data d;
	bzero(&d,sizeof(d));
	d.len = file_size-file_offset_len;  //考虑到断点续传，所以，前面会接收一个已近发送了的文件大小，这里要算出还需要发送多少文件,如果对端没有接收到部分文件，那么这里就是-0;
	int tmp = sizeof(d.buf);
	while(d.len>0)
	{
		if(d.len>tmp)   //为了高效，最开始整个数组长度的发，等到最后剩余的不到一个数组，在把剩下的发了
		{
			send_n(socket_fd,(char*)&tmp,sizeof(int));
			send_n(socket_fd,file_point+file_offset_len,tmp);  //发送剩余文件
			d.len -= tmp;
			file_offset_len += tmp;
		}
		else
		{
			tmp = file_size-file_offset_len;
			send_n(socket_fd,(char*)&tmp,sizeof(int));
			send_n(socket_fd,file_point+file_offset_len,tmp); 
			d.len -= tmp;
			file_offset_len += tmp;
		}
	}
	int endflag = 0;
	send_n(socket_fd,(char*)&d.len,sizeof(int)); //发送结束标志 
	printf("mmap send %s success!\n",file_name);
	close(fd);   //这里客户端解映射段错误，但是服务器端没事
	return 0;
}
