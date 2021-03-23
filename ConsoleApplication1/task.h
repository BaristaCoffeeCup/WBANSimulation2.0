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
		//构造函数
		Task(int taskResource, int dataSize,int taskPriority,int taskState): taskResource(taskResource), dataSIze(dataSize),taskPriority(taskPriority),taskState(taskState) {
			//cout << "已创建一个任务，数据大小为：" << dataSIze << "任务优先级为：" << taskPriority << endl;
			//设置当前任务的生成时间
			setTimeGenerate();
		}
		//析构函数
		~Task() {

		}

		/*************************************************************************************/

		//设置该任务的数据源头设备编号
		void setTaskResource(int _taskResource) {
			taskResource = _taskResource;
		}
		//返回该任务的数据源头设备编号
		int getTaskResource() {
			return taskResource;
		}

		//设置该任务的决策变量值
		void setTaskState(int taskstate) {
			taskState = taskstate;
		}
		//返回任务对象的决策变量值
		int getTaskState() {
			return taskState;
		}

		//设置该任务的数据量值
		void setDataSize(int datasize) {
			dataSIze = datasize;
		}
		//返回任务对象的数据量
		int getDataSize() {
			return dataSIze;
		}
		
		//设置任务生成时间点
		void setTimeGenerate() {
			timeGenerate = GetTickCount();
		}
		//获取任务的生成时间
		DWORD getTimeGenerate() {
			return timeGenerate;
		}

		//设置任务的传输时延
		void setTimeTran(int time) {
			timeTran = time;
		}
		//获取任务的传输时延
		int getTimeTran() {
			return timeTran;
		}

		//设置任务的处理时延
		void setTimeProcess(int time) {
			timeProcess = time;
		}
		//获取任务的处理时延
		int getTimeProcess() {
			return timeProcess;
		}



		/*************************************************************************************/

	private:

		int taskResource;		//	创造该任务的用户编号
		int taskKind;				//任务类型 3
		int dataSIze;		//任务数据量 2
		int taskPriority;	//任务优先级 1
		int taskState;	//任务的决策，本地执行或发送至云端

		//任务生成时间点
		DWORD timeGenerate;
		//任务处理结束时间点
		DWORD timeFinish;

		//任务进入队列时间
		DWORD timeInQueue;
		//任务出队列时间
		DWORD timeOutQueue;

		//任务传输时延 4
		int timeTran; 
		//任务处理时延 5
		int timeProcess;
		//任务传输能耗 6
		int energyTran;
		//任务处理能耗 7
		int energyProcess;



};
