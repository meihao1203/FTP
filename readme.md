FTP 项目
运行描述:	
	项目分为两个部分，服务器端server和客户端client,先运行服务器端./server_noverify ../conf/server.conf<br>
	然后在运行客户端./client_noverify ip port(ip和端口号在conf文件夹里的server.conf文件里)，
	程序有日志记录功能，要建立log文件并放到和二进制文件同一个文件夹(bin)下;
	程序采用进程池来实现，在服务器开启监听listen客户端连接的时候所有子进程都已经创建好并进入S睡眠状态，
	默认只设置了5个业务进程，可以去头文件func.h里面自行更改重新编译
功能： 
	1.cd 进入对应目录
	2.ls 列出相应目录文件 （与LINUX系统下 "ls -l" 效果相同）
	3.puts 文件名  将本地文件上传至服务器
	4.gets 文件名  下载服务器文件到本地
	5.remove 删除服务器上文件
	6.pwd 显示目前所在路径
	7.mkdir 当前路径创建文件夹
	8.密码验证，运行./server_verify ../conf/server.conf 是带密码验证的，必须在root权限下运行
	9.日志记录，log文件记录客户端操作
	10.断点续传，文件没下载好是.mh结尾的临时文件，可以gets或puts继续下载或上传
	11.mmap将大文件(>300M)映射进内存再进行网络传出，多大的文件进行映射已经放到func.h里

注：
	如果自己要重新编译要把所有文件放到一起，由于项目采用了crypt进行linux自带的SHA-512加密验证，编译的时候要加上 -lcrypt;
	执行带加密验证的二进制文件，用户名密码是server_verify所运行的电脑的用户的；


----------
08052018更新，服务端增加log文件夹单独存放日志文件、增减不带密码验证的运行脚本，可直接./run.sh启动服务端。readme增加程序运行截图

> 程序开始服务器端执行"ls -ll"
> ![开始服务器端文件](https://github.com/meihao1203/FTP/blob/master/%E7%A8%8B%E5%BA%8F%E8%BF%90%E8%A1%8C%E6%88%AA%E5%9B%BE/%E5%BC%80%E5%A7%8B%E6%9C%8D%E5%8A%A1%E5%99%A8%E7%AB%AF%E6%96%87%E4%BB%B6.png)
> 
> 程序开始客户端bin/文件夹下文件
> ![程序开始客户端目录下bin/下文件](https://github.com/meihao1203/FTP/blob/master/%E7%A8%8B%E5%BA%8F%E8%BF%90%E8%A1%8C%E6%88%AA%E5%9B%BE/%E5%BC%80%E5%A7%8B%E5%AE%A2%E6%88%B7%E7%AB%AF%E5%8F%AF%E6%89%A7%E8%A1%8C%E6%96%87%E4%BB%B6%E5%A4%B9.png)
> 
> 服务器端启动系统中的进程
> ![](https://github.com/meihao1203/FTP/blob/master/%E7%A8%8B%E5%BA%8F%E8%BF%90%E8%A1%8C%E6%88%AA%E5%9B%BE/%E7%B3%BB%E7%BB%9F%E5%90%AF%E5%8A%A8%E7%B3%BB%E7%BB%9F%E5%90%8E%E5%8F%B0%E8%BF%9B%E7%A8%8B.png)
> 
> 客户端运行，执行ls命令
> ![](https://github.com/meihao1203/FTP/blob/master/%E7%A8%8B%E5%BA%8F%E8%BF%90%E8%A1%8C%E6%88%AA%E5%9B%BE/%E5%AE%A2%E6%88%B7%E7%AB%AF%E6%89%A7%E8%A1%8Cls%E5%91%BD%E4%BB%A4.png)
> 
> 客户端执行pwd命令
> ![](https://github.com/meihao1203/FTP/blob/master/%E7%A8%8B%E5%BA%8F%E8%BF%90%E8%A1%8C%E6%88%AA%E5%9B%BE/%E5%AE%A2%E6%88%B7%E7%AB%AF%E6%89%A7%E8%A1%8Cpwd%E5%91%BD%E4%BB%A4.png)
> 
> 客户端执行gets命令
> ![](https://github.com/meihao1203/FTP/blob/master/%E7%A8%8B%E5%BA%8F%E8%BF%90%E8%A1%8C%E6%88%AA%E5%9B%BE/%E5%AE%A2%E6%88%B7%E7%AB%AF%E6%89%A7%E8%A1%8Cgets%E5%91%BD%E4%BB%A4.png)
> 
> 客户端执行gets命令获取服务器端的视频文件后客户端下的文件情况
> ![](https://github.com/meihao1203/FTP/blob/master/%E7%A8%8B%E5%BA%8F%E8%BF%90%E8%A1%8C%E6%88%AA%E5%9B%BE/%E5%AE%A2%E6%88%B7%E7%AB%AFgets%E6%96%87%E4%BB%B6%E5%90%8E.png)
    md5sum验证下载下来的文件是不是和服务器端完全一致

> 客户端执行remove命令
> ![](https://github.com/meihao1203/FTP/blob/master/%E7%A8%8B%E5%BA%8F%E8%BF%90%E8%A1%8C%E6%88%AA%E5%9B%BE/%E5%AE%A2%E6%88%B7%E7%AB%AF%E6%89%A7%E8%A1%8Cremove%E5%91%BD%E4%BB%A4.png)

> 客户端执行mkdir命令
> ![](https://github.com/meihao1203/FTP/blob/master/%E7%A8%8B%E5%BA%8F%E8%BF%90%E8%A1%8C%E6%88%AA%E5%9B%BE/%E5%AE%A2%E6%88%B7%E7%AB%AF%E6%89%A7%E8%A1%8Cmkdir%E5%91%BD%E4%BB%A4.png)

> 客户端执行cd命令
> ![](https://github.com/meihao1203/FTP/blob/master/%E7%A8%8B%E5%BA%8F%E8%BF%90%E8%A1%8C%E6%88%AA%E5%9B%BE/%E5%AE%A2%E6%88%B7%E7%AB%AF%E6%89%A7%E8%A1%8Ccd%E5%91%BD%E4%BB%A4.png)

> 最终服务器端主目录下文件
> ![](https://github.com/meihao1203/FTP/blob/master/%E7%A8%8B%E5%BA%8F%E8%BF%90%E8%A1%8C%E6%88%AA%E5%9B%BE/%E6%9C%80%E7%BB%88%E6%9C%8D%E5%8A%A1%E7%AB%AF%E7%9A%84%E6%96%87%E4%BB%B6.png)

#### 测试运行的程序截图中的视频文件84mb,没有push到项目之上；
### 下面是测试大文件的mmap映射上传和断点续传 ###
![](https://github.com/meihao1203/FTP/blob/master/%E7%A8%8B%E5%BA%8F%E8%BF%90%E8%A1%8C%E6%88%AA%E5%9B%BE/mmap%E5%A4%A7%E6%96%87%E4%BB%B6%E6%98%A0%E5%B0%84%E4%B8%8A%E4%BC%A0%E4%B8%8B%E8%BD%BD.png)
> 
> 客户端下载大文件，下载一半断开
> ![](https://github.com/meihao1203/FTP/blob/master/%E7%A8%8B%E5%BA%8F%E8%BF%90%E8%A1%8C%E6%88%AA%E5%9B%BE/%E5%AE%A2%E6%88%B7%E7%AB%AF%E6%96%87%E4%BB%B6%E4%B8%8B%E8%BD%BD%E4%BA%86%E4%B8%80%E5%8D%8A.png)

> 客户端连接继续下载
> ![](https://github.com/meihao1203/FTP/blob/master/%E7%A8%8B%E5%BA%8F%E8%BF%90%E8%A1%8C%E6%88%AA%E5%9B%BE/%E5%AE%A2%E6%88%B7%E7%AB%AF%E6%96%AD%E7%82%B9%E7%BB%AD%E4%BC%A0.png)

> 下载的大文件MD5码比对
> ![](https://github.com/meihao1203/FTP/blob/master/%E7%A8%8B%E5%BA%8F%E8%BF%90%E8%A1%8C%E6%88%AA%E5%9B%BE/%E5%AE%A2%E6%88%B7%E7%AB%AF%E5%A4%A7%E6%96%87%E4%BB%B6mmap%E4%B8%8B%E8%BD%BD.png)
