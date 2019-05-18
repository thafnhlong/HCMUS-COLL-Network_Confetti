#pragma once

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include "RCSocket.h"


using namespace std;

class CConfetti;

struct ques {
	string ask;
	string ans;
};

struct PlayerResource
{
	string nickname;

	int Soccer = 0;
};
struct ClientResource
{
	PlayerResource player;
	bool isinit = 0;
	bool islogin = 0;
	HANDLE threading = NULL;
	SOCKET client = NULL;
	void Destructor()
	{
		closesocket(client);
		TerminateThread(threading, 0);
		client = NULL;
		threading = NULL;
	}
};
struct RequestResource
{
	ClientResource* cr;
	CConfetti* ccon;
};


class CConfetti
{
private:
	RCSocket* pSocket;
	static CConfetti* Seleton;
public:
	CConfetti();
	CConfetti(string IP,int PORT);
	void Release();

	void StartLogin(int);
	void StopLogin();
	void StartGame();
	void StopGame();

	//
	bool issamename(ClientResource* client);
	void loadingquestion();
	//

	static DWORD WINAPI AcceptThread(LPVOID);
	static DWORD WINAPI LoginThread(LPVOID);
	static DWORD WINAPI StartThread(LPVOID);
	static DWORD WINAPI PlayThread(LPVOID);


	vector<ClientResource*> ClientList;
	vector<ques> DatabaseQuestion;

	int MaxClient = 0;
	int QuestionIndex = -1;
	string playerwinindex = ",@";
	int PlayerHasGone = 0;

	bool EndLogin = 0;
	bool BeginAsk = 0;
	bool Final = 0;

	HANDLE GameThread = NULL;

	static CConfetti* GetInstance(string IP, int PORT);
};