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
			thread taskAllocateThread(&EdgeServer::taskAllocate, this);
			taskAllocateThread.detach();
			thread taskProcessThread0(&EdgeServer::taskProcess, this, 0);
			taskProcessThread0.detach();
			thread taskProcessThread1(&EdgeServer::taskProcess, this, 1);
			taskProcessThread1.detach();
			thread taskProcessThread2(&EdgeServer::taskProcess, this, 2);
			taskProcessThread2.detach();
			thread taskProcessThread3(&EdgeServer::taskProcess, this, 3);
			taskProcessThread3.detach();

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

				int minLoadQueue = -1;
				int minLoad = MAXINT;
				//判断当前负载最小的服务队列
				for (unsigned int i = 0; i < serverQueueSet.size();i++) {
					//筛选负载最小的队列
					if (serverQueueSet[i].size() < minLoad) {
						minLoadQueue = i;
						minLoad = serverQueueSet[i].size();  
					}
				}

				//cout << "选择处理队列为：" << minLoadQueue << endl;

				//放入选定队列 互斥访问对应服务队列
				serverQueueSetMutex.lock();
				serverQueueSet[minLoadQueue].push(temp);
				serverQueueSetMutex.unlock();

				//唤醒对应的阻塞队列
				switch (minLoadQueue)
				{
				case 0:
					processQueueEmpty0.notify_all();
					//cout << "任务送入0号处理队列" << endl;
					break;
				case 1:
					processQueueEmpty1.notify_all();
					//cout << "任务送入1号处理队列" << endl;
					break;
				case 2:
					processQueueEmpty2.notify_all();
					//cout << "任务送入2号处理队列" << endl;
					break;
				case 3:
					processQueueEmpty3.notify_all();
					//cout << "任务送入3号处理队列" << endl;
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

			receiveQueueEmpty.notify_one();
			cout << "任务送入服务器" <<task.getTaskResource()<< endl;
			
		}

		//服务器的四个四个处理器单独工作
		void taskProcess(int numberOfCPU) {
			
			int curLength = serverQueueSet[numberOfCPU].size();

			condition_variable* contionalNow = NULL;
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
			cout << "处理队列开始工作：" << numberOfCPU << endl;

			
			while (true) {
				//互斥访问服务器的处理队列
				unique_lock<mutex> queueServerLock(serverQueueSetMutex);
				contionalNow->wait(queueServerLock, [this,numberOfCPU] {
					if (!serverQueueSet[numberOfCPU].empty()) {
						return true;
					}
					else return false;
				});

				//线程暂停 模拟传输时延
				std::chrono::milliseconds dura(100);
				std::this_thread::sleep_for(dura);
				  
				
				
			}//end while
		}


		/*************************************************************************************************/

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
		//服务器的坐标
		pair<double, double> coordinate;

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
