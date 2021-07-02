#pragma once
#include "XFtpTask.h"
//上传文件
class XFtpSTOR : public XFtpTask {
public:
	//解析协议
	virtual void Parse(std::string type, std::string msg);
	virtual void Read(bufferevent *bev);
	virtual void Event(bufferevent *bev, short what);

private:
	FILE *fp = 0;
	char buf[1024] = {0};
};
