#include "XFtpTask.h"
#include <event2/event.h>
#include <event2/bufferevent.h>
#include <string>
#include <iostream>
using namespace std;

void XFtpTask::Close() {
	if (bev) {
		bufferevent_free(bev);
		bev = 0;	
	}
	//if (fp) {

		//fclose(fp);
		//fp = 0;
	//}
}
void XFtpTask::Send(std::string data) {
	Send(data.c_str(), data.size());
}
void XFtpTask::Send(const char*data, int datasize) {
	if (!bev) return;
	bufferevent_write(bev, data, datasize);
}
//连接数据通道
void XFtpTask::ConnectPORT(){
	if (ip.empty() || port <= 0 || !base) {
		cout << "ConnectPORT failed ip or port or base is null" << endl;
		return;
	}
	Close();
	bev = bufferevent_socket_new(base, -1, BEV_OPT_CLOSE_ON_FREE);

	sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);
	evutil_inet_pton(AF_INET, ip.c_str(), &sin.sin_addr.s_addr);
	//设置回调和权限；
	SetCallback(bev);
	//添加超时
	timeval t1 = {60, 0};
	bufferevent_set_timeouts(bev, &t1, 0);
	bufferevent_socket_connect(bev, (sockaddr *)&sin, sizeof(sin));
}
	//回复cmd消息
void XFtpTask::ResponseToCmd(string msg) {
	if (!cmdTask || !cmdTask->bev)
		return;
	cout << "rescmd:" << msg << endl;
	if (msg[msg.size()- 1] != '\n')
		msg += "\r\n";
	
	bufferevent_write(cmdTask->bev, msg.c_str(), msg.size());
}
void XFtpTask::SetCallback(struct bufferevent *bev) {
	bufferevent_setcb(bev, read_cb, write_cb, event_cb, this);
	bufferevent_enable(bev, EV_READ | EV_WRITE);
}
void XFtpTask::read_cb(struct bufferevent *bev, void *ctx) {
	XFtpTask *t = (XFtpTask *)ctx;
	t->Read(bev);
}
void XFtpTask::write_cb(struct bufferevent *bev, void *ctx) {
	XFtpTask *t = (XFtpTask *)ctx;
	t->Write(bev);
}
void XFtpTask::event_cb(struct bufferevent *bev, short what, void *ctx) {
	XFtpTask *t = (XFtpTask *)ctx;
	t->Event(bev,what);
}
