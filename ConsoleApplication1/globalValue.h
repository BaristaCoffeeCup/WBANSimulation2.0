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
#include "task.h"
#include "mobilePhone.h"
#include "channel.h"
#include "edgeServer.h"

class EdgeGlobalManager {
	public:
		//���캯��
		EdgeGlobalManager() {

		}
		//��������
		~EdgeGlobalManager() {

		}

		//�󶨷�����ָ��ָ��ķ�����
		void setPointerToEdge(EdgeServer &edgeserver) {
			pointerToEdge = &edgeserver;
		}
		EdgeServer* getPointerToEdge() {
			return pointerToEdge;
		}
		 
		//�ŵ���������ͨ��˫��
		template <typename T>
		void buildChannel(T &user) {
			Channel<EdgeServer, T> channelTemp(*pointerToEdge, std::ref(user));
		}


	private:
		//������ָ�� ָ���Ե��������ָ��
		EdgeServer* pointerToEdge;

};


extern int value;
extern vector<int> globalQueue;