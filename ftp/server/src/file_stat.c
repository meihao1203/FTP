#include"func.h"
//实现ftp的ls的功能，这里利用LINUX的接口实现出"ls -ll"的样子
void change_time_type(char *t)
{
	char *p;
	p = t+strlen(t)-1;
	while(*p!=':')
	{
		p--;
	}
	*p = '\0';
}
void change_st_mode(char *perm,struct stat buf)
{
        if (S_ISDIR(buf.st_mode))
        {
                perm[0]='d';
        }
        if(S_ISREG(buf.st_mode))
        {
                perm[0]='-';
        }
        else
        {       
                perm[0]='*';
        }
        if( (buf.st_mode & S_IRUSR)==S_IRUSR )
        {
                perm[1]='r';
        }
        if( (buf.st_mode & S_IWUSR)==S_IWUSR )
        {
                perm[2]='w';
        }
        if( (buf.st_mode & S_IXUSR)==S_IXUSR )
        {
                perm[3]='x';
        }
        if( (buf.st_mode & S_IRGRP)==S_IRGRP )
        {
                perm[4]='r';
        }
        if( (buf.st_mode & S_IWGRP)==S_IWGRP )
        {
                perm[5]='w';
        }
        if( (buf.st_mode & S_IXGRP)==S_IXGRP )
        {
                perm[6]='x';
        }
        if( (buf.st_mode & S_IROTH)==S_IROTH )
        {
                perm[7]='r';
        }
        if( (buf.st_mode & S_IWOTH)==S_IWOTH )
        {
                perm[8]='w';
        }
        if( (buf.st_mode & S_IXOTH)==S_IXOTH )
        {
                perm[9]='x';
        }
        perm[10]='\0';
}
void file_stat(char *res,char *filename)
{
	struct stat buf;
	memset(&buf,0,sizeof(buf));
	int ret = stat(filename,&buf);
	if(-1==ret)
	{
		perror("stat");
		return ;
	}
	char t[128];
	strcpy(t,ctime(&buf.st_mtime));
	change_time_type(t);
	char perm[10];
	memset(perm,'-',sizeof(perm));
	change_st_mode(perm,buf);
	sprintf(res,"%s %ld %s %s %6ld %s %s",perm,buf.st_nlink,getpwuid(buf.st_uid)->pw_name,getgrgid(buf.st_gid)->gr_name,buf.st_size,t+4,filename);
	return ;
}