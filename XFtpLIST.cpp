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
	//-rwxrwxrwx 1 root group 64463 Mar 14 09��53 101.jpg\r\n
	string data = "";
	//�洢�ļ���Ϣ
	_finddata_t file;
	//Ŀ¼������
	path += "/*.*";
	intptr_t dir = _findfirst(path.c_str(), &file);
	if (dir < 0)
		return data;

	do {
		string temp = "";
		//�Ƿ���Ŀ¼ȥ�� . ..
		if (file.attrib & _A_SUBDIR) {
			if (strcmp(file.name, ".") == 0 || strcmp(file.name, "..") == 0) {
				continue;
			}
			temp = "drwxrwxrwx 1 root group "; //Ŀ¼

		} else {
			temp = "-rwxrwxrwx 1 root group "; //��ͨ�ļ�
		}
		//�ļ���С
		char buf[1024];
		sprintf(buf, "%u", file.size);
		temp += buf;

		//����ʱ��
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
	// 3 226 Transfer complete�������
	ResponseToCmd("226 Transfer complete\r\n");

	// 4�ر�����
	Close();
}
void
XFtpLIST::Event(bufferevent *bev, short what)
{
	//����Է�����ϵ������߻��������п����ղ���BEV_EVENT_EOF����
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
		resmsg = "257 \""; //\ת��
		resmsg += cmdTask->curDir;
		resmsg += "\" is current dir.\r\n";

		ResponseToCmd(resmsg);
	} else if (type == "LIST") {
		// 0����������ͨ�� 1��150��Ӧ  2������Ŀ¼����ͨ�� 3��������ɵĻ�Ӧ226
		// 4���ر����� ����ͨ���ظ���Ϣ ʹ������ͨ������Ŀ¼ -rwxrwxrwx 1 root
		// group 64463 Mar 14 09��53 101.jpg\r\n ��������ͨ��
		ConnectPORT();
		// 150��Ӧ
		ResponseToCmd("150 Here comes the directory listing.\r\n");
		// string listdata="-rwxrwxrwx 1 root group 64463 Mar 14 09��53
		// 105.jpg\r\n";

		string listdata = GetListData(cmdTask->rootDir + cmdTask->curDir);

		//Ŀ¼����ͨ������
		Send(listdata);

		// bufferevent_write(bev, listdata.c_str(), listdata.size());
	}else if(type=="CWD"){//�л�Ŀ¼								//ȡ�������е�·��
		// CWD test\r\n
		int pos = msg.rfind(" ") + 1;
		//ȥ����β \r\n -2;
		string path = msg.substr(pos, msg.size() - pos - 2);

		if (path[0] == '/') { //����·��
			cmdTask->curDir = path;
		} else { //���·��
			if (cmdTask->curDir[cmdTask->curDir.size() - 1] != '/') {
				cmdTask->curDir += "/";
			}
			cmdTask->curDir += path + "/";
		}
		// /test/
		ResponseToCmd("250 Directory success changed.\r\n");
	} else if (type == "CDUP") { //�ص��ϲ�Ŀ¼
		//     /Debug/test_ser.A3C61E95.tlog /Debug  /Debug/
		string path = cmdTask->curDir;
		//ͳһȥ����β��/
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