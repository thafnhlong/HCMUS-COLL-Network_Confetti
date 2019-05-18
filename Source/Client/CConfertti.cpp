#include "CConfertti.h"

CConfetti* CConfetti::Seleton = NULL;

void CConfetti::Login()
{
	char* buff = new char[1024];
	int lenbyte = 0;

	if ((lenbyte = recv(pSocket->mysocket, buff, 1024, 0)) > 0)
	{
		buff[lenbyte] = '\0';
		cout << buff;
	}
	string nickname;

inputname:
	getline(cin, nickname);
	if (send(pSocket->mysocket, nickname.c_str(), nickname.length(), 0) == nickname.length())
	{
		if ((lenbyte = recv(pSocket->mysocket, buff, 1024, 0)) > 0)
		{
			buff[lenbyte] = '\0';
			if (string(buff) == "name_error")
			{
				cout << " Ten nickname cua ban da ton tai, vui long nhap lai: ";
				goto inputname;
			}
		}
	}
	cout << "\n Chao mung ban: @" << nickname << " da dang nhap thanh cong";
}

void CConfetti::StartGame()
{
	char* buff = new char[1024];
	int lenbyte = 0;

	int QuestionCount = 0;

	if ((lenbyte = recv(pSocket->mysocket, buff, 1024, 0)) > 0)
	{
		buff[lenbyte] = '\0';
		QuestionCount = *buff;
	}
	else return;

	cout << "\n Vui long doi cau hoi tu server\n Ban co 1 phut de tra loi moi cau hoi";

	string Answer;
	HANDLE readcinthread = NULL;

	for (size_t i = 0; i < QuestionCount; i++)
	{
		if ((lenbyte = recv(pSocket->mysocket, buff, 1024, 0)) > 0)
		{
			buff[lenbyte] = '\0';
			cout << "\n Cau " << i << ": " << buff << "\n ";

			if (readcinthread == NULL || WaitForSingleObject(readcinthread, 0) == WAIT_OBJECT_0)
				readcinthread = CreateThread(0, 0, ReadCin, &Answer, 0, 0);

			if (WaitForSingleObject(readcinthread, 61 * 1000) != WAIT_OBJECT_0) // 61
			{
				if (empty(Answer)) Answer = "#)@_#)_@)@+_$)@%)(@"; // error
			}

			if (send(pSocket->mysocket, Answer.c_str(), Answer.length(), 0) <= 0) return;

			Answer = "";

			if ((lenbyte = recv(pSocket->mysocket, buff, 1024, 0)) > 0)
			{
				buff[lenbyte] = '\0';
				cout << "\n Dap an dung cho cau hoi nay la: " << buff;
			}
			else return;
			cout << "\n Vui long doi cau hoi tiep theo";
		}
		else return;
	}

	if ((lenbyte = recv(pSocket->mysocket, buff, 1024, 0)) > 0)
	{
		buff[lenbyte] = '\0';
	}
	else return;

	if (string(buff) == "win")
	{
		cout << "\n Chuc mung ban da chien thang trong luot choi nay voi so cau tra loi dung la: ";
	}
	else
	{
		cout << "\n Chuc ban may man lan sau, so cau tra loi dung la: ";
	}

	if ((lenbyte = recv(pSocket->mysocket, buff, 1024, 0)) > 0)
	{
		buff[lenbyte] = '\0';
		cout << (int)*buff << endl;
	}
	else return;
}

DWORD WINAPI CConfetti::ReadCin(LPVOID param)
{
	string * data = (string*)param;
	string getlinestring = "";
	getline(cin, getlinestring);
	(*data).assign(getlinestring);
	return 0;
}

CConfetti::CConfetti()
{
	pSocket = new RCSocket();
	pSocket->Connect("127.0.0.1", 1760);
}

CConfetti::CConfetti(string IP, int PORT)
{
	pSocket = new RCSocket();
	pSocket->Connect(IP.c_str(), PORT);
}

void CConfetti::Release()
{
	delete pSocket;
}

CConfetti* CConfetti::GetInstance(string IP, int PORT)
{
	if (Seleton == NULL) Seleton = new CConfetti(IP, PORT);
	return Seleton;
}
