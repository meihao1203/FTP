#include"func.h"
//描述符是操作系统内核管控的信息，所以main进程要把epoll监听到的客户端链接请求发送给业务进程的时候就要用sendmsg和recvmsg和socketpair管道来实现描述符的传递
void send_fd(int fds,int fd)
{
	struct msghdr msg;
	struct iovec iov[2];
	char buf1[6] = "hello";
	char buf2[6] = "world";
	iov[0].iov_base = buf1;
	iov[0].iov_len = strlen(buf1);
	iov[1].iov_base = buf2;
	iov[1].iov_len = strlen(buf2);
	
	struct cmsghdr *cmsg;
	int len = CMSG_LEN(sizeof(int));   //我们要传递的是一个整形的描述符
	cmsg = (struct cmsghdr*)calloc(1,len);
	cmsg->cmsg_len = len;
	cmsg->cmsg_level = SOL_SOCKET;
	cmsg->cmsg_type = SCM_RIGHTS;
	int *fdptr;
	fdptr = (int *)CMSG_DATA(cmsg);
	*fdptr = fd;

	bzero(&msg,sizeof(msg));
	msg.msg_iov = iov;
	msg.msg_iovlen = 2;
	msg.msg_control = cmsg;
	msg.msg_controllen = len;

	int ret = sendmsg(fds,&msg,0);
	if(-1==ret)
	{
		perror("sendmsg");
		return ;
	}
	free(cmsg);
	return ;
}
void read_fd(int fds,int *fd)
{
	struct msghdr msg;
	bzero(&msg,sizeof(msg));
	//char buf1[6] = "";
	char buf1[6] = {0};
	//char buf2[6] = "";
	char buf2[6] = {0};
	struct iovec iov[2];
	iov[0].iov_base = buf1;
	//iov[0].iov_len = strlen(buf1);
	//最开始就因为这里写了长度，前面直接赋值的""，导致子进程在read_fd的时候不能够卡住，等待接收main进程接收到新的new_fd发送过来；以前这里写过小例子测试过，后面调试加输出才定位到这里，把这个错误解决了，这里是内核控制信息，具体原理也不懂，这边必须要放数据，随便放几个，但是并不能接收到对面发来的数据；
	iov[0].iov_len = sizeof(buf1);
	iov[1].iov_base = buf2;
	//iov[1].iov_len = strlen(buf2);
	iov[1].iov_len = sizeof(buf2);

	struct cmsghdr *cmsg;
	int len = CMSG_LEN(sizeof(int));
	cmsg = (struct cmsghdr*)calloc(1,len);
	cmsg->cmsg_len = len;
	cmsg->cmsg_level = SOL_SOCKET;
	cmsg->cmsg_type = SCM_RIGHTS;
	
	bzero(&msg,sizeof(msg));
	msg.msg_iov = iov;
	msg.msg_iovlen = 2;
	msg.msg_control = cmsg;
	msg.msg_controllen = len;
	
	int ret = recvmsg(fds,&msg,0);
	if(-1==ret)
	{
		perror("recvmsg");
		return ;
	}
	*fd = *(int *)CMSG_DATA(cmsg);
	//printf("buf1=%s buf2=%s\n",buf1,buf2);
	free(cmsg);
	return;
}
