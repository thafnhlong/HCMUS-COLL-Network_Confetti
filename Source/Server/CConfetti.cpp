#include "CConfetti.h"
#include <algorithm>
#include <random>
#include <fstream>

CConfetti* CConfetti::Seleton = NULL;

CConfetti::CConfetti()
{
	pSocket = new RCSocket();
	pSocket->MakeServer(INADDR_ANY, 1760);
}

CConfetti::CConfetti(string IP, int PORT)
{
	pSocket = new RCSocket();
	pSocket->MakeServer(IP.c_str(), PORT);
}

void CConfetti::Release()
{
	GameThread = NULL;

	for (int i = ClientList.size() - 1; i >= 0; i--)
	{
		ClientResource* ClientSock = ClientList[i];
		ClientSock->Destructor();
		ClientList.erase(ClientList.begin() + i);
	}

	delete pSocket;
}

void CConfetti::StartLogin(int maxclient)
{
	MaxClient = maxclient;
	GameThread = CreateThread(0, 0, AcceptThread, this, 0, 0);
};

void CConfetti::StopLogin()
{
	EndLogin = 1;
	while (WaitForSingleObject(GameThread, 0) != WAIT_OBJECT_0)
	{
		Sleep(100);
	}
}

void CConfetti::StartGame()
{
	loadingquestion();
	cout << "\n Co tong cong: " << DatabaseQuestion.size() << " cau hoi.\n Moi ban cho so luong cau hoi: ";
	int count = 0;
	cin >> count;
	DatabaseQuestion.resize(count);
	GameThread = CreateThread(0, 0, StartThread, this, 0, 0);
}

void CConfetti::StopGame()
{
	//tao co dong game
	while (WaitForSingleObject(GameThread, 0) != WAIT_OBJECT_0)
	{
		Sleep(100);
	}
	cout << "\n Stop game!";
}

bool CConfetti::issamename(ClientResource* client)
{
	for each (ClientResource * CR in ClientList)
	{
		if (CR->player.nickname == client->player.nickname && CR != client) return true;
	}
	return false;
}

void CConfetti::loadingquestion()
{
	fstream fsfile;
	fsfile.open("data.txt", ios::in);
	while (!fsfile.eof()) {
		ques X;
		getline(fsfile, X.ask);
		getline(fsfile, X.ans);
		DatabaseQuestion.push_back(X);
	}
	auto rng = default_random_engine{};
	shuffle(begin(DatabaseQuestion), end(DatabaseQuestion), rng);
}

DWORD __stdcall CConfetti::AcceptThread(LPVOID Params)
{
	CConfetti* pConfetti = (CConfetti*)Params;
	bool checkingclientlogin = false;
	while (!pConfetti->EndLogin && !checkingclientlogin)
	{
		if (pConfetti->ClientList.size() == 0 || pConfetti->ClientList.back()->isinit)
		{
			RequestResource* RR = new RequestResource;
			RR->ccon = pConfetti;
			ClientResource *CR = new ClientResource;
			RR->cr = CR;
			CR->threading = CreateThread(0, 0, pConfetti->LoginThread, RR, 0, 0);

			pConfetti->ClientList.push_back(CR);
		}
		Sleep(100);

		int count = 0;
		for each (ClientResource* cr in pConfetti->ClientList)
			if (cr->islogin) count++;
		if (count == pConfetti->MaxClient) checkingclientlogin = true;
	}

	if (pConfetti->ClientList.size() > pConfetti->MaxClient)
	{
		if (!pConfetti->EndLogin)
			cout << "\n So client trong server da du, an enter de bat dau tro choi.";
		pConfetti->EndLogin = 1; // chac chan da dung 
	}

	for (int i = pConfetti->ClientList.size() - 1; i >= 0; i--)
	{
		ClientResource* ClientSock = pConfetti->ClientList[i];
		if (!ClientSock->islogin)
		{
			ClientSock->Destructor();
			pConfetti->ClientList.erase(pConfetti->ClientList.begin() + i);
		}
	}

	return 0;
}

DWORD __stdcall CConfetti::LoginThread(LPVOID Params)
{
	RequestResource* RR = (RequestResource*)Params;

	sockaddr_in address;
	int addrlen = sizeof(sockaddr_in);

	SOCKET socketclient = accept(RR->ccon->pSocket->mysocket, (struct sockaddr*) & address, (int*)& addrlen);
	RR->cr->isinit = 1;

	if (socketclient < 0)
	{
		cout << "\n Khong the accept client";
		exit(EXIT_FAILURE);
	}
	RR->cr->client = socketclient;
	cout << "\n Da tiep nhan client:" << ntohs(address.sin_port);

	char* title = " Chao ban den voi confetti server \n Vui long nhap nickname: ";
	int lenbyte = 0;
	char* buff = new char[1024];

	lenbyte = send(socketclient, title, strlen(title), 0);
	if (lenbyte == strlen(title))
	{
		while (1)
		{
			int sizename = recv(socketclient, buff, 512, 0);
			buff[sizename] = '\0';
			if (sizename > 0)
			{
				RR->cr->player.nickname = buff;
				if (RR->ccon->issamename(RR->cr))
					send(socketclient, "name_error", 10, 0);
				else
				{
					send(socketclient, "name_ok", 7, 0);
					RR->cr->islogin = 1;
					cout << "\n Client :" << ntohs(address.sin_port) << " nick name : @" << buff << " da dang nhap";
					break;
				}
			}
			else
			{
				//client out
				cout << "\n Client :" << ntohs(address.sin_port) << " da ngat ket noi";

				RR->ccon->ClientList.erase(remove(RR->ccon->ClientList.begin(), RR->ccon->ClientList.end()
					, RR->cr), RR->ccon->ClientList.end());
				RR->cr->Destructor();
				break;
			}
			Sleep(100);
		}
	}

	return 0;
}

DWORD __stdcall CConfetti::StartThread(LPVOID Params)
{
	CConfetti* pConfetti = (CConfetti*)Params;

	for each (ClientResource* cr in pConfetti->ClientList)
	{
		RequestResource *RR = new RequestResource;
		RR->ccon = pConfetti;
		RR->cr = cr;
		cr->threading = CreateThread(0, 0, PlayThread, RR, 0, 0);
	}
	cout << "\n Gui thong bao bat dau tro choi";

	auto oldtime = time(0) - 65;
	pConfetti->BeginAsk = 1;
	while (1)
	{
		if (time(0) - oldtime >= 65) // 65
		{
			pConfetti->QuestionIndex++;
			oldtime = time(0);
			cout << "\n Luot chay cau hoi so " << pConfetti->QuestionIndex;
		}
		if (pConfetti->QuestionIndex == pConfetti->DatabaseQuestion.size()) break;
		Sleep(10);
	}

	Sleep(1500);
	cout << "\n Dang phan tich ket qua";
	//layketquaphantich
	int maxsoc = 0;
	for each (ClientResource* cr in pConfetti->ClientList)
	{
		if (cr->player.Soccer > maxsoc) maxsoc = cr->player.Soccer;
	}
	int index = 0;
	for each (ClientResource* cr in pConfetti->ClientList)
	{
		if (cr->player.Soccer == maxsoc) pConfetti->playerwinindex += cr->player.nickname + ",@";
		index++;
	}
	//gancothongbao
	pConfetti->Final = 1;
	cout << "\n Gui ket qua win / lose cho client";

	while (pConfetti->PlayerHasGone != pConfetti->ClientList.size()) Sleep(100);

	cout << "\n Tat ca client da nhan duoc ket qua";

	return 0;
}

DWORD CConfetti::PlayThread(LPVOID Params)
{
	RequestResource* RR = (RequestResource*)Params;

	//guisoluong cau hoi
	int numq = RR->ccon->DatabaseQuestion.size();
	int lenbyte = 0;
	char* buff = new char[1024];

	lenbyte = send(RR->cr->client, (char*)&numq, sizeof(numq), 0);

	if (lenbyte <= 0)		goto releaseresource;


	//cho den khi bat dau hoi
	while (!RR->ccon->BeginAsk)
	{
		Sleep(10);
	}

	for (int i = 0; i < RR->ccon->DatabaseQuestion.size(); i++)
	{
		while (RR->ccon->QuestionIndex != i) Sleep(10);
		//guicauhoi
		if (send(RR->cr->client, RR->ccon->DatabaseQuestion[i].ask.c_str(), RR->ccon->DatabaseQuestion[i].ask.length(), 0) <= 0) goto releaseresource;
		//nhancautraloi
		lenbyte = recv(RR->cr->client, buff, 1024, 0);
		buff[lenbyte] = '\0';
		if (lenbyte <= 0) goto releaseresource;

		cout << "\n client :@" << RR->cr->player.nickname << " : gui dap an: " << buff;

		//dungtangdiem
		if (string(buff) == RR->ccon->DatabaseQuestion[i].ans) RR->cr->player.Soccer++;
		//guidapandung
		if (send(RR->cr->client, RR->ccon->DatabaseQuestion[i].ans.c_str(), RR->ccon->DatabaseQuestion[i].ans.length(), 0) <= 0) goto releaseresource;
	}

	//cho congboketqua
	while (!RR->ccon->Final) Sleep(10);

	if (RR->ccon->playerwinindex.find("@" + RR->cr->player.nickname) != string::npos)
	{
		if (send(RR->cr->client, "win", 3, 0) <= 0) goto releaseresource;
	}
	else if (send(RR->cr->client, "lose", 4, 0) <= 0) goto releaseresource;

	int num = RR->cr->player.Soccer;
	if (send(RR->cr->client, (char*)&num, sizeof(num), 0) <= 0) goto releaseresource;

	goto returnnow;
releaseresource:
	cout << "\n Client nickname: @" << RR->cr->player.nickname << " da ngat ket noi";
	RR->ccon->ClientList.erase(remove(RR->ccon->ClientList.begin(), RR->ccon->ClientList.end()
		, RR->cr), RR->ccon->ClientList.end());
	RR->cr->Destructor();
	return 0;
returnnow:
	RR->ccon->PlayerHasGone++;
	return 0;
}

CConfetti* CConfetti::GetInstance(string IP, int PORT)
{
	if (Seleton == NULL) Seleton = new CConfetti(IP, PORT);
	return Seleton;
}
