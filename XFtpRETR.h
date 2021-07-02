#pragma once
#include "XFtpTask.h"
class XFtpRETR : public XFtpTask {
public:
	//Ω‚Œˆ–≠“È
	virtual void Parse(std::string type, std::string msg);
	virtual void Write(bufferevent *bev);
	virtual void Event(bufferevent *bev, short what);


private:
	FILE *fp = 0;
	char buf[1024] = {0};
};
