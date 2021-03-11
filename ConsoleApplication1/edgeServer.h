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
		//���캯��
		EdgeServer(int num):number(num) {

			//���ó�ʼ������
			initEdgeServer();
			//�����߳�
			/*thread taskAllocateThread(&EdgeServer::taskAllocate, this);
			taskAllocateThread.detach();*/
			
		}

		//��������
		~EdgeServer() {

		}

		
		//��������ʼ������
		void initEdgeServer() {
			for (int i = 0;i < 4;i++) {
				//��ʼ���ĸ��������������
				queue<Task> queueTemp;
				serverQueueSet.push_back(queueTemp);
			}
		}

		
		//������з��亯��  ���ݵ�ǰ�ĸ�������еĸ��������ѡ����̵ķ������
		void taskAllocate() {
			while (true) {
				//�������
				unique_lock<mutex> queueReceiveLock(queueReceiveMutex);
				receiveQueueEmpty.wait(queueReceiveLock, [this] {
					if (!queueReceive.empty()) {
						return true;
					}
					else return false;
				});
				//ȡ����ǰ���еĶ�������
				Task temp = queueReceive.front();
				queueReceive.pop();
				queueReceiveLock.unlock();

				//�жϵ�ǰ������С�ķ������
				int minLoadQueue = -1;
				int minLoad = MAXINT;
				for (unsigned int i = 0; i < serverQueueSet.size();i++) {
					//ɸѡ������С�Ķ���
					if (serverQueueSet[i].size() < minLoad) {
						minLoadQueue = i;
						minLoad = serverQueueSet[i].size();  
					}
				}

				//����ѡ������ ������ʶ�Ӧ�������
				serverQueueSetMutex.lock();
				serverQueueSet[minLoadQueue].push(temp);
				serverQueueSetMutex.unlock();

				//���Ѷ�Ӧ����������
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

		//�������������񣬸ú������ŵ��������
		void taskReceive(Task &task) {
			//����������񻺳����
			queueReceiveMutex.lock();
			queueReceive.push(task);
			queueReceiveMutex.unlock();
			cout << "�������������" <<queueReceive.size()<< endl;
		}

		//���������ĸ��ĸ���������������������
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
				//������ʷ������Ĵ������
				unique_lock<mutex> queueServerLock(serverQueueSetMutex);
				contionalNow->wait(queueServerLock, [this,numberOfCPU] {
					if (!serverQueueSet[numberOfCPU].empty()) {
						return true;
					}
					else return false;
				});

				//ȡ��������еĶ���
				Task temp = std::ref(serverQueueSet[numberOfCPU].front());
				serverQueueSet[numberOfCPU].pop();

				//���㴦���������Ҫ�Ĵ���ʱ�ӣ����Ը�����Ĵ���ʱ�ӽ��и�ֵ
				/*
				
				��������ʱ��û��ȷ��

				*/
				

			}//end while
		}


		//���÷������ı��
		void setNumber(int num) {
			number = num;
		}
		//���ط������ı��
		int getNumber() {
			return number;
		}


		

	private:
		//���������
		int number;
		//����������뾶
		int serverRadius;


		//��������������Ļ������
		queue<Task> queueReceive;
		//��������������� �ݶ��ĸ��������
		vector<queue<Task>> serverQueueSet;



		//������л�����
		mutex queueReceiveMutex;
		//��������黥����
		mutex serverQueueSetMutex;


		//��������
		condition_variable receiveQueueEmpty;
		condition_variable processQueueEmpty0;
		condition_variable processQueueEmpty1;
		condition_variable processQueueEmpty2;
		condition_variable processQueueEmpty3;
		

		

};
