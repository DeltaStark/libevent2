#include "XFtpServerCmd.h"
#include <iostream>
#include <event.h>
#include <event2/bufferevent.h>
#include <string.h>
#include <string>
using namespace std;

void XFtpServerCmd::Register(std::string cmd, XFtpTask *call) {
	if (!call) {
		cout << "XFtpServerCmd::register call is null " << endl;
		return;
	}
	if (cmd.empty()) {
		cout << "XFtpServerCmd::register cmd is null " << endl;
		return;
	}
	//已经注册的是否覆盖   不覆盖，提示错误
	if (calls.find(cmd) != calls.end()) {
		cout << cmd << "has already registered! " << endl;
		return;
	}
	calls[cmd] = call;
}
void XFtpServerCmd::Read (struct bufferevent *bev) {

	char data[1024] = {0};
	for (;;) {
		int len = bufferevent_read(bev, data, sizeof(data) - 1);
		if (len <= 0)
			break;
		data[len] = '\0';

		cout << "recv cmd:"<<data << flush;
		//分发到处理对象
		//分析出类型 USER	anonymous
		string type = "";
		for (int i = 0; i < len; i++) {
			if (data[i] == ' ' || data[i] == '\r') {
				break;
			}
			type += data[i];
		}
		cout << " type is [" << type << "]" << endl;
		if (calls.find(type) != calls.end()) {
			XFtpTask *t = calls[type];
			t->cmdTask = this; //用来处理回复命令和目录
			t->ip = ip;
			t->port = port;
			t->base = base;
			//t->base = base;
			
			t->Parse(type, data);
			if (type == "PORT") {
				ip = t->ip;
				port = t->port;
			}
		} else {
			string msg = "200 OK\r\n";
			bufferevent_write(bev, msg.c_str(), msg.size());
		}
	}
}
	
void XFtpServerCmd::Event(struct bufferevent *bev, short what) {
	

	//如果对方网络断掉，或者机器死机有可能收不到BEV_EVENT_EOF数据
	if (what & (BEV_EVENT_ERROR | BEV_EVENT_EOF | BEV_EVENT_TIMEOUT)) {
		cout << "BEV_EVENT_ERROR | BEV_EVENT_EOF|BEV_EVENT_TIMEOUT" << endl;
		bufferevent_free(bev);
		delete this;
	}
}

//初始化任务 运行在子线程中
bool XFtpServerCmd::Init() {

	cout << "XFtpServerCmd::Init()" << endl;
	//使用bufferevent来监听socket
	//base sock 
	bufferevent *bev = bufferevent_socket_new(base,sock,BEV_OPT_CLOSE_ON_FREE);
	this->bev = bev;
	this->SetCallback(bev);

	timeval t1 = {60, 0};
	bufferevent_set_timeouts(bev, &t1, 0);
	string msg = "220 welcome to libevent XFtpServer\r\n";
	bufferevent_write(bev, msg.c_str(), msg.size());


	return true;
 }