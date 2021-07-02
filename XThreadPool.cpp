#include "XThreadPool.h"
#include "XThread.h"
#include <thread>
#include <iostream>
#include "XTask.h"
using namespace std;

void XThreadPool::Init(int count)
{
	threadCount=count;
	lastThread = -1;
	for (int i = 0; i < threadCount; i++) {
		XThread *t = new XThread();
		cout << "create thread" << " "<< i << endl;
		t->id = i;
		//�����߳�
		t->Start();


		threads.push_back(t);
		this_thread::sleep_for(10ms);
	}
}
//�ַ��߳�
void XThreadPool::Dispatch(XTask *task){
	//��ѯ
	if (!task)
		return;
	int tid = (lastThread + 1) % threadCount;
	lastThread=tid;
	XThread *t = threads[tid];

	//�߳��������
	t->AddTask(task);

	//�����߳�
	t->Activate();

}



