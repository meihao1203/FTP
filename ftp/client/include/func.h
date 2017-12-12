//client端用到的所有头文件、结构体和函数
#ifndef __FUNC_H__
#define __FUNC_H__
#define MAXSIZE 1024
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
#include<dirent.h>
#include<sys/mman.h>
#include<signal.h>

typedef struct 
{
	int len;
	char buf[128];
}data,*pdata;

typedef struct
{
	 int cmd_len;
	 char cmd_buf[MAXSIZE];
}cmd,*pcmd;

void sendfile(int sfd);
void recv_n(int sfd,char *buf,int len);
void send_n(int sfd,char *buf,int len);

void deal_cmd(char *cmd);
int is_file_exist(char *filename);
int send_file(int socket_fd,char *file_name);
int recv_file(int socket_fd);
int is_file_exist_tmp(char *filename);
int mmap_send_file(int socket_fd,char *file_name); 
int login_verify(int sfd);
#endif
