#include"func.h"
//密码验证，发送用户名和密码给服务端，进行验证；采用的SHA-512加密算法，用户名是server所在LINUX系统上的用户名和密码
int login_verify(int sfd)
{
	char *usrname;
	char *passwd;
	int usrname_len;
	int passwd_len;
	usrname = getpass("please input usrname:");
	usrname_len = strlen(usrname);
	passwd = getpass("please input passwd:");
	passwd_len = strlen(passwd);
	send(sfd,(char *)&usrname_len,sizeof(int),0);
	send_n(sfd,usrname,usrname_len);
	send(sfd,(char *)&passwd_len,sizeof(int),0);
	send_n(sfd,passwd,passwd_len);
	int flag;
	recv(sfd,&flag,sizeof(int),0);
	return flag;
}