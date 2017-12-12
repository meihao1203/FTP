#include"func.h"
//删除输入的命令中多余的空格，达到:"命令 参数"中间一个空格的效果
void deal_cmd(char *cmd)
{
	int cur,index;
	for(cur=0,index=0;index<strlen(cmd);index++)
	{
		if(cmd[index]!=' '&&cmd[index]!='\n')
		{
			cmd[cur++] = cmd[index];
		}
		else
		{
			if(cur!=0&&cmd[cur-1]!=' ')
			{
				cmd[cur++] = ' ';
			}
		}
	}
	cmd[cur] = '\0';
}