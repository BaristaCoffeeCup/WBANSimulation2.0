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
using namespace std;

class Task {
	public:
		//���캯��
		Task(int taskResource, int dataSize,int taskPriority,int taskState): taskResource(taskResource), dataSIze(dataSize),taskPriority(taskPriority),taskState(taskState) {
			//cout << "�Ѵ���һ���������ݴ�СΪ��" << dataSIze << "�������ȼ�Ϊ��" << taskPriority << endl;
			//���õ�ǰ���������ʱ��
			setTimeGenerate();
		}
		//��������
		~Task() {

		}

		/*************************************************************************************/

		//���ø����������Դͷ�豸���
		void setTaskResource(int _taskResource) {
			taskResource = _taskResource;
		}
		//���ظ����������Դͷ�豸���
		int getTaskResource() {
			return taskResource;
		}

		//���ø�����ľ��߱���ֵ
		void setTaskState(int taskstate) {
			taskState = taskstate;
		}
		//�����������ľ��߱���ֵ
		int getTaskState() {
			return taskState;
		}

		//���ø������������ֵ
		void setDataSize(int datasize) {
			dataSIze = datasize;
		}
		//������������������
		int getDataSize() {
			return dataSIze;
		}
		
		//������������ʱ���
		void setTimeGenerate() {
			timeGenerate = GetTickCount();
		}
		//��ȡ���������ʱ��
		DWORD getTimeGenerate() {
			return timeGenerate;
		}

		//��������Ĵ���ʱ��
		void setTimeTran(int time) {
			timeTran = time;
		}
		//��ȡ����Ĵ���ʱ��
		int getTimeTran() {
			return timeTran;
		}

		//��������Ĵ���ʱ��
		void setTimeProcess(int time) {
			timeProcess = time;
		}
		//��ȡ����Ĵ���ʱ��
		int getTimeProcess() {
			return timeProcess;
		}



		/*************************************************************************************/

	private:

		int taskResource;		//	�����������û����
		int taskKind;				//�������� 3
		int dataSIze;		//���������� 2
		int taskPriority;	//�������ȼ� 1
		int taskState;	//����ľ��ߣ�����ִ�л������ƶ�

		//��������ʱ���
		DWORD timeGenerate;
		//���������ʱ���
		DWORD timeFinish;

		//����������ʱ��
		DWORD timeInQueue;
		//���������ʱ��
		DWORD timeOutQueue;

		//������ʱ�� 4
		int timeTran; 
		//������ʱ�� 5
		int timeProcess;
		//�������ܺ� 6
		int energyTran;
		//�������ܺ� 7
		int energyProcess;



};
