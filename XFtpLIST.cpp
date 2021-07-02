#include "XFtpLIST.h"
#include <string>

#include <iostream>
#include <event2/bufferevent.h>
#include <event2/event.h>
#include <io.h>
#include <string.h>
using namespace std;

string XFtpLIST::GetListData(std::string path)
{
	//-rwxrwxrwx 1 root group 64463 Mar 14 09；53 101.jpg\r\n
	string data = "";
	//存储文件信息
	_finddata_t file;
	//目录上下文
	path += "/*.*";
	intptr_t dir = _findfirst(path.c_str(), &file);
	if (dir < 0)
		return data;

	do {
		string temp = "";
		//是否是目录去掉 . ..
		if (file.attrib & _A_SUBDIR) {
			if (strcmp(file.name, ".") == 0 || strcmp(file.name, "..") == 0) {
				continue;
			}
			temp = "drwxrwxrwx 1 root group "; //目录

		} else {
			temp = "-rwxrwxrwx 1 root group "; //普通文件
		}
		//文件大小
		char buf[1024];
		sprintf(buf, "%u", file.size);
		temp += buf;

		//日期时间
		strftime(buf, sizeof(buf), "%b %d %H:%M ", localtime(&file.time_write));
		temp += buf;
		temp += file.name;
		temp += "\r\n";
		data += temp;


	} while (_findnext(dir, &file) == 0);

	return data;
}

void
XFtpLIST::Write(bufferevent *bev)
{
	// 3 226 Transfer complete发送完成
	ResponseToCmd("226 Transfer complete\r\n");

	// 4关闭连接
	Close();
}
void
XFtpLIST::Event(bufferevent *bev, short what)
{
	//如果对方网络断掉，或者机器死机有可能收不到BEV_EVENT_EOF数据
	if (what & (BEV_EVENT_ERROR | BEV_EVENT_EOF | BEV_EVENT_TIMEOUT)) {
		cout << "BEV_EVENT_ERROR | BEV_EVENT_EOF|BEV_EVENT_TIMEOUT" << endl;
		Close();


	} else if (what & BEV_EVENT_CONNECTED) {
		cout << "XFtpLIST::BEV_EVENT_CONNECTED" << endl;
	}
}

void
XFtpLIST::Parse(std::string type, std::string msg)
{
	string resmsg = "";
	if (type == "PWD") {
		// 257 "/" is current directory.
		resmsg = "257 \""; //\转义
		resmsg += cmdTask->curDir;
		resmsg += "\" is current dir.\r\n";

		ResponseToCmd(resmsg);
	} else if (type == "LIST") {
		// 0、连接数据通道 1、150回应  2、发送目录数据通道 3、发送完成的回应226
		// 4、关闭连接 命令通道回复消息 使用数据通道发送目录 -rwxrwxrwx 1 root
		// group 64463 Mar 14 09；53 101.jpg\r\n 连接数据通道
		ConnectPORT();
		// 150回应
		ResponseToCmd("150 Here comes the directory listing.\r\n");
		// string listdata="-rwxrwxrwx 1 root group 64463 Mar 14 09；53
		// 105.jpg\r\n";

		string listdata = GetListData(cmdTask->rootDir + cmdTask->curDir);

		//目录数据通道发送
		Send(listdata);

		// bufferevent_write(bev, listdata.c_str(), listdata.size());
	}else if(type=="CWD"){//切换目录								//取出命令中的路径
		// CWD test\r\n
		int pos = msg.rfind(" ") + 1;
		//去掉结尾 \r\n -2;
		string path = msg.substr(pos, msg.size() - pos - 2);

		if (path[0] == '/') { //绝对路径
			cmdTask->curDir = path;
		} else { //相对路径
			if (cmdTask->curDir[cmdTask->curDir.size() - 1] != '/') {
				cmdTask->curDir += "/";
			}
			cmdTask->curDir += path + "/";
		}
		// /test/
		ResponseToCmd("250 Directory success changed.\r\n");
	} else if (type == "CDUP") { //回到上层目录
		//     /Debug/test_ser.A3C61E95.tlog /Debug  /Debug/
		string path = cmdTask->curDir;
		//统一去掉结尾的/
		//   /Debug/test_ser.A3C61E95.tlog /Debug
		if (path[path.size() - 1] == '/') {
			path = path.substr(0, path.size() - 1);
		}
		int pos = path.rfind("/");
		path = path.substr(0, pos);
		cmdTask->curDir = path;
		ResponseToCmd("250 Directory success changed.\r\n");
	}
}