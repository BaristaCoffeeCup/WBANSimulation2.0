#pragma once
#include <iostream>
#include <thread>
#include <mutex>
#include <queue>
#include <stack>
#include <Windows.h>
#include <string>
#include <map>
#include <list>
#include <windows.h>
#include "channel.h"


using namespace std;

class MobilePhone {

	public:
		//构造函数
		MobilePhone(int number,int priority,int velocity) :number(number),priority(priority),movingVelocity(velocity) {
			
			printf("创建移动终端用户：编号为%d，移动速度为%d，优先级为%d\n", number, movingVelocity, priority);

			//初始化移动设备电量为5000J
			energyLeft = 5000;
			
			//建立任务生成线程
			thread threadGenerate(&MobilePhone::taskGenerate,this);
			threadGenerate.detach();
			thread threadAllocate(&MobilePhone::taskAllocate, this);
			threadAllocate.detach();
		}

		//析构函数 
		~MobilePhone() {
			 
		}

		//固定时间内生成任务并送入任务池	默认全部发送至服务器
		void taskGenerate() {
			cout << "编号" << number << "用户开始生成任务" << endl;
			while (true) {
				std::chrono::milliseconds dura(500);		//间隔为1秒
				std::this_thread::sleep_for(dura);

				//互斥访问任务池
				unique_lock<mutex> bufferLock(mutexBufferTasks);
				bufferOfTasks.push(Task(number, 200, 5, 1));	//	生成一个任务对象 200bits P5
				bufferLock.unlock();
				cout << "任务送入任务池" << endl;
			}
		}

		//对任务池中的任务按照任务决策送入对应的队列
		void taskAllocate() {
			cout << "用户" << number << "开始分配任务" << endl;
			while (true) {
				//当任务池中有任务时
				if (!bufferOfTasks.empty()) {
					//该任务决定上传执行
					if (bufferOfTasks.front().getTaskState() == 1) {
						//互斥 j将当前任务送入发送队列
						unique_lock<mutex> queueTranLock(mutexQueueTran);
						queueTran.push(std::ref(bufferOfTasks.front()));
						queueTranLock.unlock();
						//唤醒等待线程
						queueTranEmpty.notify_all();

						cout << "任务送入发送队列" << endl;

						//互斥 将当前任务从任务池中移除
						unique_lock<mutex> bufferLock(mutexBufferTasks);
						bufferOfTasks.pop();
						bufferLock.unlock();
					}//if state == 1

					//该任务决定本地执行
					else if (bufferOfTasks.front().getTaskState() == 0) {
						//互斥 将当前任务送入本地队列
						unique_lock<mutex> queueLocalLock(mutexQueueLocal);
						queueLocal.push(std::ref(bufferOfTasks.front()));
						queueLocalLock.unlock();
						cout << "任务送入本地队列" << endl;

						//互斥 将当前任务从任务池中移除
						unique_lock<mutex> bufferLock(mutexBufferTasks);
						bufferOfTasks.pop();
						bufferLock.unlock();

					} // else if
				}//if empty
			}//end while
		}
		 

		//移动终端任务发送函数 供信道调用
		Task taskTransport() {
			//互斥访问发送队列
			//unique_lock<mutex> queueTranLock(mutexQueueTran, defer_lock);
			mutexQueueTran.lock();
			//获取队首任务并return
			Task temp = std::ref(queueTran.front());
			queueTran.pop();
			//queueTranLock.unlock();
			mutexQueueTran.unlock();
			
			cout << "任务离开移动终端" << endl;
			return temp;
		}

		//返回当前发送信道的负载状况，供信道对象使用
		bool getQueueTranState() {
			return queueTran.empty();
		}


		//移动终端接受信道传来的任务 这一函数用于信道调用
		void taskReceive(Task &task) {
			//互斥 访问接收队列
			//unique_lock<mutex> receiveQueueLock(mutexQueueReceive, defer_lock);
			mutexQueueReceive.lock();
			queueReceive.push(std::ref(task));
			//receiveQueueLock.unlock();
			mutexQueueReceive.unlock();
			cout << "mobilephone" << number << "接收到数据" << endl;
		}

		//移动终端本地任务处理
		void taskProcess() {

		}

		//用户移动函数，根据不同的速度和移动模式，实现用户的实时移动
		void moveUser() {
			//简单的直线运动

		}

		/*************************************************************************************************/

		//设置用户编号
		void setNumber(int num) {
			number = num;
		}
		//获取用户编号
		int getNumber() {
			return number;
		}

		//设置用户终端剩余电量
		void setEnergyLeft(int left) {
			energyLeft = left;
		}
		//获取用户终端电量
		int getEnergyLeft() {
			return energyLeft;
		}


		
		/*************************************************************************************************/

		void test(Task& task) {
			for (int i = 0;i < 300;i++) {
				queueTran.push(std::ref(task));
				queueTranEmpty.notify_all();
				Sleep(200);
			}
		}

	private:
		//用户编号
		int number;
		//移动用户优先级
		int priority;
		//移动终端剩余电量
		int energyLeft;
		//移动终端的移动速度
		int movingVelocity;


		//移动终端的发送队列
		queue<Task> queueTran;
		//移动终端本地处理队列
		queue<Task> queueLocal;
		//移动终端的接受队列
		queue<Task> queueReceive;

		//移动设备的坐标
		std::pair<int, int> coordinate;
		//本地任务池
		queue<Task> bufferOfTasks; 

		 
		//互斥量组
		std::mutex mutexQueueTran;		//发送队列互斥量
		std::mutex mutexQueueLocal;		//本地队列互斥量
		std::mutex mutexQueueReceive;//接收队列互斥量
		std::mutex mutexBufferTasks;		//任务池互斥量

		//当发送信道不再为空，唤醒信道堵塞的线程
		condition_variable queueTranEmpty;


};
