#include"func.h"
//从配置文件conf读取服务端要设置的ip和端口port
void get_arg(char *line,char *arg)
{
	if(line[strlen(line)-1]=='\n')
	{
		line[strlen(line)-1] = '\0';
	}
	char *p = strchr(line,'=');
	if(NULL==p)
	{
		perror("strchr");
		exit(-1);
	}
	strcpy(arg,p+1);
}
void file_read_arg(char *buf,char *ip,char *port)
{
	FILE *fp = fopen(buf,"r");
	if(NULL==fp)
	{
		perror("fopen");
		exit(-1);
	}
	char line[128];
	bzero(line,sizeof(line));
	fgets(line,sizeof(line),fp);
	get_arg(line,ip);
	printf("%s\n",ip);

	bzero(line,sizeof(line));
	fgets(line,sizeof(line),fp);
	get_arg(line,port);
	printf("%s\n",port);
}