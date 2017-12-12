#include"func.h"
//利用Linux的加密算法SHA-512来对连接客户端发送过来的用户名密码验证
void get_salt(char *salt,char *passwd)
{
	int i,j;
	for(i=0,j=0;passwd[i]&&j!=3;++i)
	{
		if(passwd[i]=='$')
			j++;
	}
	strncpy(salt,passwd,i-1);
}
int authentication(int new_fd)
{
	char usrname[512] = {0};
	char passwd[512] = {0};
	char salt[512] = {0};
	//接收链接客户端的用户名
	int usrname_len;
	int passwd_len;
	recv_n(new_fd,(char *)&usrname_len,sizeof(int));
	recv_n(new_fd,usrname,usrname_len);
	//接收链接客户端的密码
	recv_n(new_fd,(char *)&passwd_len,sizeof(int));
	recv_n(new_fd,passwd,passwd_len);
	struct spwd *sp;
	//getspnam可以根据用户名从/etc/shadow中得到密码
	sp = getspnam(usrname);
	int flag;
	if(NULL==sp)
	{
		//perror("getspnam");
		flag = -1;   //没有这样的用户名
		send(new_fd,&flag,sizeof(int),0);
		return -1;
	}
	get_salt(salt,sp->sp_pwdp);   //获取salt值，第二个参数是encrypted password
	if( (strcmp(sp->sp_pwdp,crypt(passwd,salt))) == 0 )
	{
		flag = 0;   //密码验证成功
		send(new_fd,&flag,sizeof(int),0);
		return 0;
	}
	else 
	{
		flag = -1;   //密码验证失败
		send(new_fd,&flag,sizeof(int),0);
		return -1;
	}
}
