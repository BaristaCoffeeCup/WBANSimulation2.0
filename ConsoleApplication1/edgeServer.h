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

				int minLoadQueue = -1;
				int minLoad = MAXINT;
				//�жϵ�ǰ������С�ķ������
				for (unsigned int i = 0; i < serverQueueSet.size();i++) {
					//ɸѡ������С�Ķ���
					if (serverQueueSet[i].size() < minLoad) {
						minLoadQueue = i;
						minLoad = serverQueueSet[i].size();  
					}
				}

				//cout << "ѡ�������Ϊ��" << minLoadQueue << endl;

				//����ѡ������ ������ʶ�Ӧ�������
				serverQueueSetMutex.lock();
				serverQueueSet[minLoadQueue].push(temp);
				serverQueueSetMutex.unlock();

				//���Ѷ�Ӧ����������
				switch (minLoadQueue)
				{
				case 0:
					processQueueEmpty0.notify_all();
					//cout << "��������0�Ŵ������" << endl;
					break;
				case 1:
					processQueueEmpty1.notify_all();
					//cout << "��������1�Ŵ������" << endl;
					break;
				case 2:
					processQueueEmpty2.notify_all();
					//cout << "��������2�Ŵ������" << endl;
					break;
				case 3:
					processQueueEmpty3.notify_all();
					//cout << "��������3�Ŵ������" << endl;
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

			receiveQueueEmpty.notify_one();
			cout << "�������������" <<task.getTaskResource()<< endl;
			
		}

		//���������ĸ��ĸ���������������
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
			cout << "������п�ʼ������" << numberOfCPU << endl;

			
			while (true) {
				//������ʷ������Ĵ������
				unique_lock<mutex> queueServerLock(serverQueueSetMutex);
				contionalNow->wait(queueServerLock, [this,numberOfCPU] {
					if (!serverQueueSet[numberOfCPU].empty()) {
						return true;
					}
					else return false;
				});

				//�߳���ͣ ģ�⴫��ʱ��
				std::chrono::milliseconds dura(100);
				std::this_thread::sleep_for(dura);
				  
				
				
			}//end while
		}


		/*************************************************************************************************/

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
		//������������
		pair<double, double> coordinate;

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
