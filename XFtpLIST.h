#pragma once
#include "XFtpTask.h"
class XFtpLIST : public XFtpTask {
public:
	//½âÎöĞ­Òé
	virtual void Parse(std::string type, std::string msg);
	virtual void Write(bufferevent *bev);
	virtual void Event(bufferevent *bev, short what);
private:
	std::string GetListData(std::string path);
};
