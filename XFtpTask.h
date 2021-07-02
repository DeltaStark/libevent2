#pragma once
#include "XTask.h"
#include <string>
class XFtpTask : public XTask {
public:
	

	std::string curDir = "/";
	std::string rootDir = ".";
	//命令通道
	XFtpTask *cmdTask = 0;

	// PORT 数据通道的IP和端口；
	std::string ip = "";
	int port = 0;

	//连接数据通道
	void ConnectPORT();
	void Close();

	//解析协议
	virtual void Parse(std::string type, std::string msg){}
	//回复cmd消息
	void ResponseToCmd(std::string mgs);
	//用来发送建立了连接的数据通道
	void Send(std::string data);
	void Send(const char*data, int datasize);

	virtual	void Read(struct bufferevent *bev){}
	virtual	void Write(struct bufferevent *bev){}
	virtual	void Event(struct bufferevent *bev,short what){}
	void SetCallback(struct bufferevent *bev);
	bool Init(){return true;}

protected:
	static void read_cb(struct bufferevent *bev, void *ctx);
	static void write_cb(struct bufferevent *bev, void *ctx);
	static void event_cb(struct bufferevent *bev, short what, void *ctx);
	//命令bev
	struct bufferevent *bev = 0;
	
};
