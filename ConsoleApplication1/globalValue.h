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
#include<map>
#include "task.h"
#include "mobilePhone.h"
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
			std::cout << "tid=" << std::this_thread::get_id() << std::endl;
			Channel<EdgeServer,T>* channelTemp = new Channel<EdgeServer,T>(*pointerToEdge, std::ref(user));
		}

		void buildChannelBetweenServerAndMobile(EdgeServer& server, MobilePhone& user) {
			Channel<EdgeServer, MobilePhone>* channelNew = new Channel<EdgeServer,MobilePhone>(std::ref(server), std::ref(user));
		}


	private:
		//ȫ���ƿصı��
		int number;
		//������ָ�� ָ���Ե��������ָ��
		EdgeServer* pointerToEdge;

		//�ƶ��ն��б�,���ù�ϣ���ŵ�ǰ����������Χ�ڵ��ƶ��ն�
		map<int, MobilePhone> mobilePhoneSet;

};


extern int value;
extern vector<int> globalQueue;