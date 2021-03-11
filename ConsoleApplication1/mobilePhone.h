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
		//���캯��
		MobilePhone(int number,int priority,int velocity) :number(number),priority(priority),movingVelocity(velocity) {
			
			printf("�����ƶ��ն��û������Ϊ%d���ƶ��ٶ�Ϊ%d�����ȼ�Ϊ%d\n", number, movingVelocity, priority);

			//��ʼ���ƶ��豸����Ϊ5000J
			energyLeft = 5000;
			
			//�������������߳�
			thread threadGenerate(&MobilePhone::taskGenerate,this);
			threadGenerate.detach();
			thread threadAllocate(&MobilePhone::taskAllocate, this);
			threadAllocate.detach();
		}

		//�������� 
		~MobilePhone() {
			 
		}

		//�̶�ʱ���������������������	Ĭ��ȫ��������������
		void taskGenerate() {
			cout << "���" << number << "�û���ʼ��������" << endl;
			while (true) {
				std::chrono::milliseconds dura(500);		//���Ϊ1��
				std::this_thread::sleep_for(dura);

				//������������
				unique_lock<mutex> bufferLock(mutexBufferTasks);
				bufferOfTasks.push(Task(number, 200, 5, 1));	//	����һ��������� 200bits P5
				bufferLock.unlock();
				cout << "�������������" << endl;
			}
		}

		//��������е�������������������Ӧ�Ķ���
		void taskAllocate() {
			cout << "�û�" << number << "��ʼ��������" << endl;
			while (true) {
				//���������������ʱ
				if (!bufferOfTasks.empty()) {
					//����������ϴ�ִ��
					if (bufferOfTasks.front().getTaskState() == 1) {
						//���� j����ǰ�������뷢�Ͷ���
						unique_lock<mutex> queueTranLock(mutexQueueTran);
						queueTran.push(std::ref(bufferOfTasks.front()));
						queueTranLock.unlock();
						//���ѵȴ��߳�
						queueTranEmpty.notify_all();

						cout << "�������뷢�Ͷ���" << endl;

						//���� ����ǰ�������������Ƴ�
						unique_lock<mutex> bufferLock(mutexBufferTasks);
						bufferOfTasks.pop();
						bufferLock.unlock();
					}//if state == 1

					//�������������ִ��
					else if (bufferOfTasks.front().getTaskState() == 0) {
						//���� ����ǰ�������뱾�ض���
						unique_lock<mutex> queueLocalLock(mutexQueueLocal);
						queueLocal.push(std::ref(bufferOfTasks.front()));
						queueLocalLock.unlock();
						cout << "�������뱾�ض���" << endl;

						//���� ����ǰ�������������Ƴ�
						unique_lock<mutex> bufferLock(mutexBufferTasks);
						bufferOfTasks.pop();
						bufferLock.unlock();

					} // else if
				}//if empty
			}//end while
		}
		 

		//�ƶ��ն������ͺ��� ���ŵ�����
		Task taskTransport() {
			//������ʷ��Ͷ���
			//unique_lock<mutex> queueTranLock(mutexQueueTran, defer_lock);
			mutexQueueTran.lock();
			//��ȡ��������return
			Task temp = std::ref(queueTran.front());
			queueTran.pop();
			//queueTranLock.unlock();
			mutexQueueTran.unlock();
			
			cout << "�����뿪�ƶ��ն�" << endl;
			return temp;
		}

		//���ص�ǰ�����ŵ��ĸ���״�������ŵ�����ʹ��
		bool getQueueTranState() {
			return queueTran.empty();
		}


		//�ƶ��ն˽����ŵ����������� ��һ���������ŵ�����
		void taskReceive(Task &task) {
			//���� ���ʽ��ն���
			//unique_lock<mutex> receiveQueueLock(mutexQueueReceive, defer_lock);
			mutexQueueReceive.lock();
			queueReceive.push(std::ref(task));
			//receiveQueueLock.unlock();
			mutexQueueReceive.unlock();
			cout << "mobilephone" << number << "���յ�����" << endl;
		}

		//�ƶ��ն˱���������
		void taskProcess() {

		}

		//�û��ƶ����������ݲ�ͬ���ٶȺ��ƶ�ģʽ��ʵ���û���ʵʱ�ƶ�
		void moveUser() {
			//�򵥵�ֱ���˶�

		}

		/*************************************************************************************************/

		//�����û����
		void setNumber(int num) {
			number = num;
		}
		//��ȡ�û����
		int getNumber() {
			return number;
		}

		//�����û��ն�ʣ�����
		void setEnergyLeft(int left) {
			energyLeft = left;
		}
		//��ȡ�û��ն˵���
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
		//�û����
		int number;
		//�ƶ��û����ȼ�
		int priority;
		//�ƶ��ն�ʣ�����
		int energyLeft;
		//�ƶ��ն˵��ƶ��ٶ�
		int movingVelocity;


		//�ƶ��ն˵ķ��Ͷ���
		queue<Task> queueTran;
		//�ƶ��ն˱��ش������
		queue<Task> queueLocal;
		//�ƶ��ն˵Ľ��ܶ���
		queue<Task> queueReceive;

		//�ƶ��豸������
		std::pair<int, int> coordinate;
		//���������
		queue<Task> bufferOfTasks; 

		 
		//��������
		std::mutex mutexQueueTran;		//���Ͷ��л�����
		std::mutex mutexQueueLocal;		//���ض��л�����
		std::mutex mutexQueueReceive;//���ն��л�����
		std::mutex mutexBufferTasks;		//����ػ�����

		//�������ŵ�����Ϊ�գ������ŵ��������߳�
		condition_variable queueTranEmpty;


};
