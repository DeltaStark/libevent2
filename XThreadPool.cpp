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
		//启动线程
		t->Start();


		threads.push_back(t);
		this_thread::sleep_for(10ms);
	}
}
//分发线程
void XThreadPool::Dispatch(XTask *task){
	//轮询
	if (!task)
		return;
	int tid = (lastThread + 1) % threadCount;
	lastThread=tid;
	XThread *t = threads[tid];

	//线程添加任务
	t->AddTask(task);

	//激活线程
	t->Activate();

}



