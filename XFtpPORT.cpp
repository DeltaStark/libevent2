#include "XFtpPORT.h"
#include <vector>
#include <iostream>
using namespace std;

void XFtpPORT::Parse(std::string type, std::string msg) {
	// PORT 127,0,0,1,70,96\r\n
	// PORT n1,n2,n3,n4,n5,n6\r\n
	// PORT=n5*256+n6

	//只获取ip和端口 不连接
	//取出ip
	vector<string> vals;
	string temp = "";
	for (int i = 5; i < msg.size(); i++) {
		if (msg[i] == ',' || msg[i] == '\r') {
			vals.push_back(temp);
			temp = "";
			continue;
		}
		temp += msg[i];
	}
	if (vals.size() != 6) {
		// PORT 格式有误
		ResponseToCmd("501 Syntax error in parameters or argument.");
		return;
	}

	ip = vals[0] + "." + vals[1] + "." + vals[2] + "." + vals[3];

	// PORT=n5*256+n6
	port = atoi(vals[4].c_str()) * 256 + atoi(vals[5].c_str());
	cout << "PORT ip is " << ip << endl;
	cout << "PORT port is " << port << endl;
	ResponseToCmd("200 PORT command successful.\r\n");
}
