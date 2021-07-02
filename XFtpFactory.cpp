#include "XFtpFactory.h"
#include "XFtpServerCmd.h"
#include "XFtpUSER.h"
#include "XFtpLIST.h"
#include "XFtpPORT.h"
#include "XFtpRETR.h"
#include "XFtpSTOR.h"

XTask *XFtpFactory::CreateTask() {
	XFtpServerCmd *x = new XFtpServerCmd();
	//注册ftp消息处理对象
	x->Register("USER", new XFtpUSER());
	XFtpLIST *list = new XFtpLIST();
	x->Register("PWD", list);
	x->Register("LIST", list);
	x->Register("CWD", list);
	x->Register("CDUP", list);
	x->Register("PORT", new XFtpPORT());
	x->Register("RETR", new XFtpRETR());
	x->Register("STOR", new XFtpSTOR());

	return x;
}




XFtpFactory::XFtpFactory() {

}