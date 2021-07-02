#pragma once
#include "XFtpTask.h"
class XFtpUSER : public XFtpTask {
public:
	//½âÎöĞ­Òé
	virtual void Parse(std::string type, std::string msg);
};
