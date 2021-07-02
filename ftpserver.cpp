#include <event2/event.h>
#include <event2/listener.h>
#ifndef _WIN32
#include <signal.h>
#endif

#include <iostream>
#include "XThreadPool.h"
#include "XFtpFactory.h"
using namespace std;

#define PORT 21


void listen_cb(struct evconnlistener *e, evutil_socket_t s, struct sockaddr *a,
	int socklen, void *arg)
{
	cout << "回调函数listen_cb执行/客户端连接成功！" << endl;
	XTask *task = XFtpFactory::Get()->CreateTask();
	task->sock=s;

	XThreadPool::Get()->Dispatch(task);

}

int main()
{
#ifdef _WIN32
	//初始化socket库
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
#else
	//忽略管道信号，发送数据给已关闭的socket；
	if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
		return 1;
#endif

	std::cout << "test_thread_pool!\n" << endl;
	//1、初始化线程池
	XThreadPool::Get()->Init(10);


	//创建libevent上下文
	event_base *base = event_base_new();
	if (base) {
		cout << "event_base_new success!" << endl;
	}

	struct sockaddr_in serv_addr;
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);




	//监听窗口 包括 socket bind listen 绑定事件
	evconnlistener *ev=evconnlistener_new_bind(base,//libevent的上下文
		listen_cb,              //接收到连接的回调函数
		base,                   //回调函数获取的参数arg
		LEV_OPT_REUSEABLE|LEV_OPT_CLOSE_ON_FREE,//地址重用，listen关闭，同时关闭socket
		10,//连接队列大小,对应listen函数backlog
		(struct sockaddr*)&serv_addr,
		sizeof(serv_addr)
		
		);
	if (base)
	      event_base_dispatch(base);//事件分发处理
	if (ev)
	      evconnlistener_free(ev);
	if (base)
		event_base_free(base);
#ifdef _WIN32
	WSACleanup();
#endif
	return 0;
}
