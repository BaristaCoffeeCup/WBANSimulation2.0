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
		//构造函数
		EdgeGlobalManager() {

		}
		//析构函数
		~EdgeGlobalManager() {

		}

		//绑定服务器指针指向的服务器
		void setPointerToEdge(EdgeServer &edgeserver) {
			pointerToEdge = &edgeserver;
		}
		EdgeServer* getPointerToEdge() {
			return pointerToEdge;
		}
		 
		//信道建立并绑定通信双方
		template <typename T>
		void buildChannel(T &user) {
			Channel<EdgeServer, T> channelTemp(*pointerToEdge, std::ref(user));
		}


	private:
		//服务器指针 指向边缘服务器的指针
		EdgeServer* pointerToEdge;

};


extern int value;
extern vector<int> globalQueue;