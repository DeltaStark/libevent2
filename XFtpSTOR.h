#pragma once
#include "XFtpTask.h"
//�ϴ��ļ�
class XFtpSTOR : public XFtpTask {
public:
	//����Э��
	virtual void Parse(std::string type, std::string msg);
	virtual void Read(bufferevent *bev);
	virtual void Event(bufferevent *bev, short what);

private:
	FILE *fp = 0;
	char buf[1024] = {0};
};
