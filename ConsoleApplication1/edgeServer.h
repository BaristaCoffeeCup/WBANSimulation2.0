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
#include <fstream>
#include <vector> 
#include "channel.h"

class EdgeServer {
	public:
		//构造函数
		EdgeServer(int num):number(num) {

			//调用初始化函数
			initEdgeServer();
			//开启线程
			/*thread taskAllocateThread(&EdgeServer::taskAllocate, this);
			taskAllocateThread.detach();*/
			
		}

		//析构函数
		~EdgeServer() {

		}

		
		//服务器初始化函数
		void initEdgeServer() {
			for (int i = 0;i < 4;i++) {
				//初始化四个服务器服务队列
				queue<Task> queueTemp;
				serverQueueSet.push_back(queueTemp);
			}
		}

		
		//缓冲队列分配函数  根据当前四个服务队列的负载情况，选择最短的服务队列
		void taskAllocate() {
			while (true) {
				//互斥访问
				unique_lock<mutex> queueReceiveLock(queueReceiveMutex);
				receiveQueueEmpty.wait(queueReceiveLock, [this] {
					if (!queueReceive.empty()) {
						return true;
					}
					else return false;
				});
				//取出当前队列的队首任务
				Task temp = queueReceive.front();
				queueReceive.pop();
				queueReceiveLock.unlock();

				//判断当前负载最小的服务队列
				int minLoadQueue = -1;
				int minLoad = MAXINT;
				for (unsigned int i = 0; i < serverQueueSet.size();i++) {
					//筛选负载最小的队列
					if (serverQueueSet[i].size() < minLoad) {
						minLoadQueue = i;
						minLoad = serverQueueSet[i].size();  
					}
				}

				//放入选定队列 互斥访问对应服务队列
				serverQueueSetMutex.lock();
				serverQueueSet[minLoadQueue].push(temp);
				serverQueueSetMutex.unlock();

				//唤醒对应的阻塞队列
				switch (minLoadQueue)
				{
				case 0:
					processQueueEmpty0.notify_one();
					break;
				case 1:
					processQueueEmpty1.notify_one();
					break;
				case 2:
					processQueueEmpty2.notify_one();
					break;
				case 3:
					processQueueEmpty3.notify_one();
					break;
				default:
					break;
				}//end switch
				
			}//end while
		}

		//服务器接收任务，该函数供信道对象调用
		void taskReceive(Task &task) {
			//互斥访问任务缓冲队列
			queueReceiveMutex.lock();
			queueReceive.push(task);
			queueReceiveMutex.unlock();
			cout << "任务送入服务器" <<queueReceive.size()<< endl;
		}

		//服务器的四个四个处理器单独工作，根据
		void taskProcess(int numberOfCPU) {
			condition_variable* contionalNow;
			switch (numberOfCPU)
			{
			case 0:
				contionalNow = &processQueueEmpty0;
				break;
			case 1:
				contionalNow = &processQueueEmpty1;
				break;
			case 2:
				contionalNow = &processQueueEmpty2;
				break;
			case 3:
				contionalNow = &processQueueEmpty3;
				break;
			default:
				break;
			}//end switch
			
			while (true) {
				//互斥访问服务器的处理队列
				unique_lock<mutex> queueServerLock(serverQueueSetMutex);
				contionalNow->wait(queueServerLock, [this,numberOfCPU] {
					if (!serverQueueSet[numberOfCPU].empty()) {
						return true;
					}
					else return false;
				});

				//取出处理队列的队首
				Task temp = std::ref(serverQueueSet[numberOfCPU].front());
				serverQueueSet[numberOfCPU].pop();

				//计算处理该任务需要的处理时延，并对该任务的处理时延进行赋值
				/*
				
				处理部分暂时还没有确定

				*/
				

			}//end while
		}


		//设置服务器的编号
		void setNumber(int num) {
			number = num;
		}
		//返回服务器的编号
		int getNumber() {
			return number;
		}


		

	private:
		//服务器编号
		int number;
		//服务器服务半径
		int serverRadius;


		//服务器接收任务的缓冲队列
		queue<Task> queueReceive;
		//服务器服务队列组 暂定四个服务队列
		vector<queue<Task>> serverQueueSet;



		//缓冲队列互斥锁
		mutex queueReceiveMutex;
		//服务队列组互斥锁
		mutex serverQueueSetMutex;


		//互斥条件
		condition_variable receiveQueueEmpty;
		condition_variable processQueueEmpty0;
		condition_variable processQueueEmpty1;
		condition_variable processQueueEmpty2;
		condition_variable processQueueEmpty3;
		

		

};
