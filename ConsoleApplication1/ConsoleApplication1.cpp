// ConsoleApplication1.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
#include <iostream>
#include <thread>
#include <mutex>
#include <queue>
#include <stack>
#include "task.h"
#include "mobilePhone.h"
#include "channel.h"
#include "edgeServer.h"
#include "globalValue.h"
using namespace std;

int main()
{

	MobilePhone m1(250, 5, 5);
	MobilePhone m2(351,4,4);
	EdgeServer server1(241);

	EdgeGlobalManager manager1;
	manager1.setPointerToEdge(std::ref(server1));

	Channel<EdgeServer, MobilePhone> c1(std::ref(server1), std::ref(m1));
	Channel<EdgeServer, MobilePhone> c2(std::ref(server1), std::ref(m2));
	//manager1.buildChannel<MobilePhone>(std::ref(m2));




	while (true);

	return 0;
}


