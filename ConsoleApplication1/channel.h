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
		//�ŵ��Ĺ��캯��
		Channel(T &t, V &v) {
			//���ñ��ŵ���Ŀ���������Դ
			unique_lock<mutex> pointerTargetLock(mutexPointerTarget);
			pointerToTarget = &t;
			pointerTargetLock.unlock();
			unique_lock<mutex> pointerResourseLock(mutexPointerResource);
			pointerToResource = &v;
			pointerResourseLock.unlock();

			delay = 20;

			cout << typeid(pointerToResource).name() << pointerToResource->getNumber() << "    ��    "
				<< typeid(pointerToTarget).name() << pointerToTarget->getNumber() << "   �����ŵ�����" << endl;

			//�����ŵ���������߳�
			thread inTaskThread(&Channel::inTask, this);
			thread outTaskThread(&Channel::outTask,this);
			inTaskThread.detach();
			outTaskThread.detach();

		}
		//��������
		~Channel() {

		}

		//���ñ��ŵ���������Դ�����Ŀ�����
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


		//ͨ���ն˻��߷����������������ŵ�
		void inTask() {
			//�ŵ���������
			while (true) {

				if (pointerToResource->getQueueTranState() == true) continue;
				else if (pointerToResource->getQueueTranState() == false) {
					//��������ŵ�����
					unique_lock<mutex> channelLock1(mutexQueueChannelTran, defer_lock);
					if (pointerToResource->getQueueTranState() == false) {
						//����������Դ����ĳ�Ա����
						queueChannelTran.push(pointerToResource->taskTransport());
						queueEmpty.notify_one();
					}
				}

				//cout << "���������ŵ�" << endl;
				

				/*
				//��������ŵ�����
				unique_lock<mutex> channelLock(mutexQueueChannelTran);
				//��������Դ�ķ����ŵ�Ϊ�գ���ͣ���������߳�
				while (pointerToResource->getQueueTranState()) {
					queueEmpty.wait(channelLock);
				}

				//����������Դ����ĳ�Ա����
				queueChannelTran.push(pointerToResource->taskTransport());
				queueEmpty.notify_one();
				
				cout << "���������ŵ�" << endl;*/

			}//end while

			
		}

		//�����̶���ʱ�ӽ����������ǰ��������ȡ���������Ӧ�Ĵ���Ŀ�괦
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

				//ȡ������ͷ��������
				Task temp = std::ref(queueChannelTran.front());
				queueChannelTran.pop();

				//�߳���ͣ ģ�⴫��ʱ��
				std::chrono::milliseconds dura(delay);
				std::this_thread::sleep_for(dura);

				//������ͷ����������Ŀ��Ľ��ܶ�����
				mutexPointerTarget.lock();
				pointerToTarget->taskReceive(temp);
				mutexPointerTarget.unlock();

				//cout << "�����뿪�ŵ�" << endl;

			}//end while
		}

		/***************************************************************************************************************/

		//���ø��ŵ��Ĵ���ʱ��
		void setDelay(int del) {
			delay = del;
		}
		//��ȡ���ŵ��Ĵ���ʱ��
		int getDelay() {
			return delay;
		}

		//�����ŵ��Ĵ���
		void setBandWidth(int band) {
			bandWidth = band;
		}
		//��ȡ�ŵ��Ĵ���
		int getBandWidth() {
			return bandWidth;
		}




		/***************************************************************************************************************/

	private:
		//�ŵ���ʱ��
		int delay;
		//�ŵ��Ĵ���
		int bandWidth;
		//�ŵ��Ĵ���ͨ��
		queue<Task> queueChannelTran;

		//ָ����Ŀ���ָ�� 
		T* pointerToTarget;
		//ָ��������ԴĿ���ָ��
		V* pointerToResource;


		//�����ŵ��Ļ�����
		mutex mutexQueueChannelTran;
		//����Ŀ��ָ��Ļ�����
		mutex  mutexPointerTarget;
		//������Դ����ָ��Ļ�����
		mutex mutexPointerResource;

		//������Ϊ��ʱ��������߳���ͣ
		condition_variable queueEmpty;
		//��������Դ�ķ��Ͷ���Ϊ��ʱ����ͣ
		condition_variable queueResourceEmpty;

};
