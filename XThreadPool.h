#pragma once
#include <vector>
//#include "XThread.h"
class XThread;
class XTask;
class XThreadPool {
public:
	//����ģʽ
	static XThreadPool* Get(){
		static XThreadPool p;
		return &p;

	}
	//��ʼ�������̲߳������̣߳�
	void Init(int count);

	//�ַ��߳�
	void Dispatch(XTask *task);
	
	

private:
	//�߳�����
	int threadCount = 0;

	int lastThread = -1;//��һ���߳�
	//�̳߳��߳�
	std::vector<XThread *> threads;
	XThreadPool(){};
};
