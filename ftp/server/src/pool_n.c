#include"func.h"
//针对发送和接收速度不匹配实现的，ftp最开始没有考虑断点续传，所以客户端发送接收一般文件突然关闭会导致进程卡死在recv
//改进，可以加上返回值判断，如果连接描述符断开或者其他错误recv，send会返回-1
void send_n(int new_fd,char *buf,int len)
{
	int ret;
	int total = 0;
	while(total<len)
	{
		ret = send(new_fd,buf+total,len-total,0); //socket缓冲区是有大小的，接收方和发送方各自的socket缓冲区都是64K，如果接收方接收速度不匹配，那么会导致接收数据方缓冲区满，发送方成功发送的数据不等于预定发送长度；
		total += ret;
	}
}
void recv_n(int new_fd,char *buf,int len)
{
	int ret;
	int total = 0;
	while(total<len)
	{
		ret = recv(new_fd,buf+total,len-total,0);
		total += ret;
	}
}
