#include "XFtpUSER.h"
#include <iostream>
using namespace std;

//����Э��
void XFtpUSER::Parse(std::string type, std::string msg)
{
	cout << "XFtpUSER::Parse " << type << " " << msg << endl;
	ResponseToCmd("230 Login successful.\r\n");
}
