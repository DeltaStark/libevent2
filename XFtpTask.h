#pragma once
#include "XTask.h"
#include <string>
class XFtpTask : public XTask {
public:
	

	std::string curDir = "/";
	std::string rootDir = ".";
	//����ͨ��
	XFtpTask *cmdTask = 0;

	// PORT ����ͨ����IP�Ͷ˿ڣ�
	std::string ip = "";
	int port = 0;

	//��������ͨ��
	void ConnectPORT();
	void Close();

	//����Э��
	virtual void Parse(std::string type, std::string msg){}
	//�ظ�cmd��Ϣ
	void ResponseToCmd(std::string mgs);
	//�������ͽ��������ӵ�����ͨ��
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
	//����bev
	struct bufferevent *bev = 0;
	
};
