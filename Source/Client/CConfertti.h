#pragma once

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#include <iostream>
#include <string>
#include <vector>
#include "RCSocket.h"

using namespace std;

class CConfetti;

class CConfetti
{
private:
	RCSocket* pSocket;
	static CConfetti* Seleton;
public:

	void Login();
	void StartGame();

	static DWORD WINAPI ReadCin(LPVOID);

	CConfetti();
	CConfetti(string IP, int PORT);
	
	void Release();

	static CConfetti* GetInstance(string IP, int PORT);
};