#include "XFtpSTOR.h"
#include <string>
#include <iostream>
#include <event2/event.h>
#include <event2/bufferevent.h>
using namespace std;

//解析协议
void
XFtpSTOR::Parse(std::string type, std::string msg)
{

	//取出文件名
	int pos = msg.rfind(" ") + 1;
	string filename = msg.substr(pos, msg.size() - pos - 2);
	string path = cmdTask->rootDir;
	path += cmdTask->curDir;
	path += filename;
	fp = fopen(path.c_str(), "wb");
	if (fp) {
		//连接数据通道
		ConnectPORT();
		//发送开始接受文件上传的指令
		ResponseToCmd("125 File OK\r\n");
		//触发读取事件
		bufferevent_trigger(bev, EV_READ, 0);
	} else {

		ResponseToCmd("450 File open failed!\r\n");
	}
}

void
XFtpSTOR::Event(bufferevent *bev, short what)
{

	//如果对方网络断掉，或者机器死机有可能收不到BEV_EVENT_EOF数据
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
