#include "XFtpSTOR.h"
#include <string>
#include <iostream>
#include <event2/event.h>
#include <event2/bufferevent.h>
using namespace std;

//����Э��
void
XFtpSTOR::Parse(std::string type, std::string msg)
{

	//ȡ���ļ���
	int pos = msg.rfind(" ") + 1;
	string filename = msg.substr(pos, msg.size() - pos - 2);
	string path = cmdTask->rootDir;
	path += cmdTask->curDir;
	path += filename;
	fp = fopen(path.c_str(), "wb");
	if (fp) {
		//��������ͨ��
		ConnectPORT();
		//���Ϳ�ʼ�����ļ��ϴ���ָ��
		ResponseToCmd("125 File OK\r\n");
		//������ȡ�¼�
		bufferevent_trigger(bev, EV_READ, 0);
	} else {

		ResponseToCmd("450 File open failed!\r\n");
	}
}

void
XFtpSTOR::Event(bufferevent *bev, short what)
{

	//����Է�����ϵ������߻��������п����ղ���BEV_EVENT_EOF����
	if (what & (BEV_EVENT_ERROR | BEV_EVENT_EOF | BEV_EVENT_TIMEOUT)) {
		cout << "XFtpSTOR BEV_EVENT_ERROR | BEV_EVENT_EOF|BEV_EVENT_TIMEOUT"
			 << endl;
		Close();

		ResponseToCmd("226 Transfer complete\r\n");
	} else if (what & BEV_EVENT_CONNECTED) {
		cout << "XFtpSTOR::BEV_EVENT_CONNECTED" << endl;
	}
}
void
XFtpSTOR::Read(bufferevent *bev)
{
	if (!fp)
		return;

	for (;;) {
		int len = bufferevent_read(bev, buf, sizeof(buf));
		if (len <= 0)
			return;
		int size = fwrite(buf, 1, len, fp);
		cout << "<" << len << ":" << size << ">" << flush;
	}
}
