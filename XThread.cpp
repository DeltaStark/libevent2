#include "XThread.h"
#include <iostream>
#include <thread>
#include <event.h>
#include "XTask.h"
#ifdef _WIN32

#else
    #include <unistd.h>

#endif
using namespace std;

//�����߳�����Ļص�����
static void notify_cb(evutil_socket_t fd, short which, void *arg) {
	XThread *t = (XThread *)arg;
	t->notify(fd, which);
	
}

//�̶߳��ܵ�����
void XThread::notify(evutil_socket_t fd, short which){
	//ˮƽ���� ֻҪû�н������ ���ٴν���
	char buf[2] = {0};
	int ret = recv(fd, buf, 1, 0);
	if (ret < 0)
		return;
	cout << id << " thread " << buf<<endl;
	XTask *task = NULL;
	//��ȡ���񣬲���ʼ������
	tasks_mutex.lock();
	if(tasks.empty()){
		tasks_mutex.unlock();
		return;
		}

	task = tasks.front();//�Ƚ��ȳ�
	tasks.pop_front();
	tasks_mutex.unlock();

	task->Init();
}
void XThread::Start() {
	SetUp();
	//�����߳�
	thread th(&XThread::Main, this);//�ص����� ����

	//�Ͽ������߳���ϵ
	th.detach();
}
void XThread::Main() {
	cout << id << " XThread::Main begin" << endl;
	//�߳���ں����������  base�ķַ�
	event_base_dispatch(base);



	event_base_free(base);

	cout << id << " XThread::Main end" << endl;
}

bool XThread::SetUp(){
	//windows �����socket  linux�ùܵ�
#ifdef _WIN32
	//����һ��socketpair ���Ի���ͨ�� fds[0]��  fds[1]д
	evutil_socket_t fds[2];
	if(evutil_socketpair(AF_INET,SOCK_STREAM,0,fds)<0){
		cout<<"evutil_socketpair failed"<<endl;
		return false;
	}
	evutil_make_socket_nonblocking(fds[0]);
	evutil_make_socket_nonblocking(fds[1]);//����д����Ϊ������
	
#else
	//linux �������ǹܵ� ֻ��read write ����send recv
	int fds[2];
	if (pipe(fds)==-1){
		cerr << "pipe failed!" << endl;
		return false;
	}

#endif
	//��ȡ�󶨵�event�¼���,д��Ҫ���棻
	notify_send_fd = fds[1];

	//����libevent������ ����
	event_config *ev_config = event_config_new();
	event_config_set_flag(ev_config, EVENT_BASE_FLAG_NOLOCK);

	this->base = event_base_new_with_config(ev_config);
	if (!base) 
   {
		cerr << "create libevent context without lock failed!" << endl;
		return false;
	}
	event_config_free(ev_config);

	

	//��ӹܵ������¼������ڼ����߳�ִ������
	event *ev=event_new(base, fds[0], EV_READ | EV_PERSIST, notify_cb, this);//���� this��ǰ����ĵ�ַ

	event_add(ev, 0);



	return true;
}
//�̼߳���
void XThread::Activate(){
#ifdef _WIN32
	int re=send(this->notify_send_fd, "c", 1, 0);
#else
	int write = send(this->notify_send_fd, "c", 1);
#endif
	if (re<=0){
		cerr << "XThred::activate() failed! " << endl;
	}


}
//��Ӵ��������
void XThread::AddTask(XTask *t){
	if (!t) return;
	t->base = this->base;
	tasks_mutex.lock();
	tasks.push_back(t);
	tasks_mutex.unlock();
}

XThread::XThread() {

}

XThread::~XThread() {

}
