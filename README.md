# EsayHttpServer
用C语言在Linux服务器上搭建Http服务  源码
! 本程序运用了 TCP HTTP协议， 并且进行设置守护进程， 对GET请求进行了解析， 其他请求暂未进行解析
  thread_work.h --
  thread_work.c --- 接受客户请求创建线程 返回响应报文
  
  httphub.c     ---
  httphub.h     --- 服务器链接与接受请求
  
  server.c      --- 主程序
  
  Makefile      --- 构建文件   clean 清除.o 文件  
                  make  进行编译  
                  编译出  server 是编译出可运行程序
  start.sh      --- 进行重新编译然后在端口80运行 server
## 最终效果：
