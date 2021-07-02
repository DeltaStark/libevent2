#pragma once
#include "XFtpTask.h"
#include <map>
class XFtpServerCmd : public XFtpTask {
public:
	//��ʼ������
	virtual bool Init();

	virtual void Read(struct bufferevent *bev);
	
	virtual void Event(struct bufferevent *bev, short what);
	//ע���������� ����Ҫ�����̰߳�ȫ������ʱ��δ�ַ����߳�
	void Register(std::string, XFtpTask *call);

private:
	//ע��Ĵ����¼�
	std::map<std::string, XFtpTask *> calls;
};
