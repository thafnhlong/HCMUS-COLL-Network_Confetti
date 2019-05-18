#pragma once

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

class RCSocket
{
public:
	static bool initialize;

	SOCKET mysocket = 0;
	sockaddr_in mysockaddr;

	RCSocket();
	~RCSocket();

	void MakeServer(unsigned long Server, short Port);
	void MakeServer(char* Server, short Port);

	void Connect(unsigned long Server, short Port);
	void Connect(const char* Server, short Port);
};

