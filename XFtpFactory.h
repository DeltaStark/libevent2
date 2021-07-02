#pragma once
#include "XTask.h"

//×÷·Ö·¢
class XFtpFactory {

public:
	static XFtpFactory*Get() {
		static XFtpFactory f;
		return &f;
	}
	XTask *CreateTask();

private:
	XFtpFactory();
};
