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
		MobilePhone(int number,int priority,int velocity,int patterns) :number(number),priority(priority),movingVelocity(velocity),userBehaviorPatterns(patterns) {
			
			printf("�����ƶ��ն��û������Ϊ%d���ƶ��ٶ�Ϊ%d�����ȼ�Ϊ%d\n", number, movingVelocity, priority);

			//��ʼ���ƶ��豸����Ϊ5000J
			energyLeft = 5000;

			////�������������߳�
			thread threadGenerate(&MobilePhone::taskGenerate,this);
			threadGenerate.detach();
			//thread threadAllocate(&MobilePhone::taskAllocate, this);
			//threadAllocate.detach();
			/*thread threadMoving(&MobilePhone::moveUser, this);
			threadMoving.detach();*/
		}

		//�������� 
		~MobilePhone() {
			 
		}

		//�̶�ʱ���������������������	Ĭ��ȫ��������������
		void taskGenerate() {
			//��ȡ�û���Ϊģ��
			vector<int> pattern = userBehaviorPatternsSet[userBehaviorPatterns];

			int kindNow = pattern[0];			//��ǰʱ����û���Ҫ���ɵ�����
			int index = 0;
			int timeCount = 0;		//�ۼ�������������ʱ��
			int timeNow = 0;			//�ۼƳ�������ִ��ʱ��
			srand((unsigned)time(NULL));
			int randomTaskKind = rand() % 4;
			int randomTime = rand() % 5 + 10;


			while (true) {
				//���Ϊ1��
				std::chrono::milliseconds dura(100);		
				std::this_thread::sleep_for(dura);

				if (timeCount == randomTime) {
					//�������һ����������
					randomTaskKind = rand() % 4;
					//������������
					unique_lock<mutex> bufferLock(mutexBufferTasks);
					bufferOfTasks.push(Task(number, 200, 5, 1, randomTaskKind));	//	����һ��������� 200bits P5
					bufferLock.unlock();
					cout << endl << "������������Ϊ��" << randomTaskKind << endl << endl;
					//�ۼ�ʱ������
					timeCount = 0;
					//�����ǰʱ���Ѿ�������֮ǰһ���漴������ʱ�ӣ����������һ�α������
					randomTime = rand() % 5 + 10;
					continue;
				}

				//������������
				unique_lock<mutex> bufferLock(mutexBufferTasks);
				bufferOfTasks.push(Task(number, 200, 5, 1,kindNow));	//	����һ��������� 200bits P5
				cout << "������������Ϊ��" << kindNow << endl;
				bufferLock.unlock();
				
				if (timeNow == 300) break;
				//ÿһ�����л�һ������
				else if (timeNow % 60 == 0 && timeNow < 300 && timeNow > 0) {
					index = (index + 1) % 4;
					kindNow = pattern[index];
				}

				timeNow++;
				timeCount++;
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

						//cout << "�������뷢�Ͷ���" << endl;

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
			
			//cout << "�����뿪�ƶ��ն�" << endl;
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
			//�����������ģ��
			while (true) {
				srand((unsigned)time(NULL));//��ʼ�������
				//ѡ��һ���ٶȣ�ѡȡ��Χ��1 - 5 m/s
				movingVelocity = rand() % 5 +1;
				//ѡȡһ������ѡȡ������0 - 2��
				movinDirection = (rand() % (180+1)) - 90 + 1;
				//û�ӳ�100�����룬����һ���û�������
				for (int i = 0;i < 100;i++) {
					std::chrono::milliseconds dura(100);	//ÿ10���û�����ѡ���ٶȺͷ���
					std::this_thread::sleep_for(dura);
					//���µ�ǰ�û�������
					coordinate.first += (double)(0.1 * movingVelocity * sin(movinDirection) );
					coordinate.second += (double)(0.1 * movingVelocity * cos(movinDirection) );
					cout << coordinate.first << "   "<<coordinate.second << endl;
				}
				
				//�ƶ�10��ֹͣ1��
				std::chrono::milliseconds dura1(1000);	
				std::this_thread::sleep_for(dura1);
			}

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

	private:
		//�û����
		int number;
		//�ƶ��û����ȼ�
		int priority;
		//�ƶ��ն�ʣ�����
		int energyLeft;
		//�ƶ��ն˵��ƶ��ٶ�
		int movingVelocity;
		//�ƶ��ն˵��ƶ�����
		int movinDirection;
		//��ǰ�ն˵Ĵ���
		int bandWidth;
		//��ǰ�ն˵ķ��书��
		int transmitPower;

		//�û�������
		pair<double, double> coordinate;

		//�û���Ϊģʽ
		int userBehaviorPatterns;
		vector<vector<int>> userBehaviorPatternsSet = {
			{0,1,2,3},
			{1,0,2,3},
			{2,0,1,3},
			{3,2,0,1},
			{3,1,2,0}
		};


		//�ƶ��ն˵ķ��Ͷ���
		queue<Task> queueTran;
		//�ƶ��ն˱��ش������
		queue<Task> queueLocal;
		//�ƶ��ն˵Ľ��ܶ���
		queue<Task> queueReceive;

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
