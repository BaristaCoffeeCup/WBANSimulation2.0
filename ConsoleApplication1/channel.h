#pragma once
#include <iostream>
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <queue>
#include <stack>
#include <Windows.h>
#include <string>
#include <map>
#include <list>
#include <fstream>
#include "task.h"

template <class T,class V>
class Channel {
	public:
		//信道的构造函数
		Channel(T &t, V &v) {
			//设置本信道的目标和数据来源
			unique_lock<mutex> pointerTargetLock(mutexPointerTarget);
			pointerToTarget = &t;
			pointerTargetLock.unlock();
			unique_lock<mutex> pointerResourseLock(mutexPointerResource);
			pointerToResource = &v;
			pointerResourseLock.unlock();

			delay = 20;

			cout << typeid(pointerToResource).name() << pointerToResource->getNumber() << "    与    "
				<< typeid(pointerToTarget).name() << pointerToTarget->getNumber() << "   建立信道链接" << endl;

			//开启信道输出函数线程
			thread inTaskThread(&Channel::inTask, this);
			thread outTaskThread(&Channel::outTask,this);
			inTaskThread.detach();
			outTaskThread.detach();

		}
		//析构函数
		~Channel() {

		}

		//设置本信道的数据来源对象和目标对象
		void setTarget(T &t) {
			unique_lock<mutex> pointerLock(mutexPointerTarget);
			pointerToTarget = &t;
			pointerLock.unlock();
		}
		void setResource(V& v) {
			unique_lock<mutex> pointerLock(mutexPointerResource);
			pointerToResource = &v;
			pointerLock.unlock();
		}


		//通过终端或者服务器将任务送入信道
		void inTask() {
			//信道持续访问
			while (true) {

				if (pointerToResource->getQueueTranState() == true) continue;
				else if (pointerToResource->getQueueTranState() == false) {
					//互斥访问信道队列
					unique_lock<mutex> channelLock1(mutexQueueChannelTran, defer_lock);
					if (pointerToResource->getQueueTranState() == false) {
						//调用任务来源对象的成员函数
						queueChannelTran.push(pointerToResource->taskTransport());
						queueEmpty.notify_one();
					}
				}

				//cout << "任务送入信道" << endl;
				

				/*
				//互斥访问信道队列
				unique_lock<mutex> channelLock(mutexQueueChannelTran);
				//当数据来源的发送信道为空，暂停任务输入线程
				while (pointerToResource->getQueueTranState()) {
					queueEmpty.wait(channelLock);
				}

				//调用任务来源对象的成员函数
				queueChannelTran.push(pointerToResource->taskTransport());
				queueEmpty.notify_one();
				
				cout << "任务送入信道" << endl;*/

			}//end while

			
		}

		//经过固定的时延将传输队列最前方的任务取出并放入对应的传输目标处
		void outTask() {

			while (true) {

				unique_lock<mutex> channelLock(mutexQueueChannelTran);
				queueEmpty.wait(channelLock, [this] {
					if (!queueChannelTran.empty()) {
						return true;
					}
					else return false;
				});
				/*while (queueChannelTran.empty()) {
					queueEmpty.wait(channelLock);
				}*/

				//取出队列头部任务函数
				Task temp = std::ref(queueChannelTran.front());
				queueChannelTran.pop();

				//线程暂停 模拟传输时延
				std::chrono::milliseconds dura(delay);
				std::this_thread::sleep_for(dura);

				//将队列头部任务送入目标的接受队列中
				mutexPointerTarget.lock();
				pointerToTarget->taskReceive(temp);
				mutexPointerTarget.unlock();

				//cout << "任务离开信道" << endl;

			}//end while
		}

		/***************************************************************************************************************/

		//设置该信道的传输时延
		void setDelay(int del) {
			delay = del;
		}
		//获取该信道的传输时延
		int getDelay() {
			return delay;
		}

		//设置信道的带宽
		void setBandWidth(int band) {
			bandWidth = band;
		}
		//获取信道的带宽
		int getBandWidth() {
			return bandWidth;
		}




		/***************************************************************************************************************/

	private:
		//信道的时延
		int delay;
		//信道的带宽
		int bandWidth;
		//信道的传输通道
		queue<Task> queueChannelTran;

		//指向传输目标的指针 
		T* pointerToTarget;
		//指向数据来源目标的指针
		V* pointerToResource;


		//传输信道的互斥量
		mutex mutexQueueChannelTran;
		//传输目标指针的互斥量
		mutex  mutexPointerTarget;
		//数据来源对象指针的互斥量
		mutex mutexPointerResource;

		//当队列为空时，将输出线程暂停
		condition_variable queueEmpty;
		//当数据来源的发送队列为空时，暂停
		condition_variable queueResourceEmpty;

};
