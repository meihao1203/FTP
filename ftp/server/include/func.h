//ftp项目用到的所有头文件和结构体、函数声明
#ifndef __FUNC_H__
#define __FUNC_H__
#define MAXSIZE 1024
#define KEY 1234
#define MMAPSIZE 300*1024*1024
#include<stdio.h>
#include<string.h>
#include<strings.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/stat.h>
#include<sys/epoll.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<fcntl.h>
#include<unistd.h>
#include<time.h>
#include<pwd.h>
#include<grp.h>
#include<dirent.h>
#include<sys/sem.h>
#include<sys/ipc.h>
#include<signal.h>
#include<sys/wait.h>
#include<sys/mman.h>
#include<shadow.h>
#include<errno.h>



typedef struct
{
	int pid;
	int fds;
	int status;
}child,*pchild;

typedef struct
{
	int len;  //buf里面存放的数据长度
	char buf[128];
}data,*pdada;

typedef struct
{
	 int cmd_len;
	 char cmd_buf[MAXSIZE];
}cmd,*pcmd;

void send_fd(int fds,int fd);
void read_fd(int fds,int *fd);
void child_work(int fds,int log_fd);
void child_handle(int new_fd,int log_fd,struct sockaddr_in client);
void make_child(pchild p,int num,int log_fd);
void sendfile(int new_fd);
void recv_n(int new_fd,char *buf,int len);
void send_n(int new_fd,char *buf,int len);

void get_arg(char *line,char *arg);
void file_read_arg(char *buf,char *ip,char *port);

void do_cd(int new_fd,pcmd pc);
void do_ls(int new_fd,pcmd pc);
void do_puts(int new_fd,pcmd pc);
void do_gets(int new_fd,pcmd pc);
void do_pwd(int new_fd,pcmd pc);
void do_mkdir(int new_fd,pcmd pc);
void change_time_type(char *t);
void change_st_mode(char *perm,struct stat buf);
void file_stat(char *res,char *filename);
int recv_file(int socket_fd);
int send_file(int socket_fd,char *file_name);
int is_file_exist(char *filename);

void sig_handle1(int signum);
void signal_init();
void log_write(int log_fd,char *log_buf);
int is_file_exist_tmp(char *filename);

void sig_handle1(int signum);
void sig_handle2(int signum);

int mmap_send_file(int socket_fd,char *file_name);

int authentication(int new_fd);
void get_salt(char *salt,char *passwd);
char *crypt(const char *key, const char *salt);
#endif
