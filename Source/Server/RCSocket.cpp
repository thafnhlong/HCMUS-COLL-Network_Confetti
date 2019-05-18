#include "RCSocket.h"

bool RCSocket::initialize = 1;

RCSocket::RCSocket()
{
	if (initialize)
	{
		initialize = 0;
		WSADATA wsa;
		if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		{
			cout << "\n Phien ban socket khong ho tro";
			exit(EXIT_FAILURE);
		}
	}
	if ((mysocket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		cout << "\n Khong the khoi tao socket";
		exit(EXIT_FAILURE);
	}
}

RCSocket::~RCSocket()
{
	closesocket(mysocket);
	WSACleanup();
}

void RCSocket::MakeServer(unsigned long Server, short Port)
{
	mysockaddr.sin_family = AF_INET;
	mysockaddr.sin_addr.s_addr = Server;
	mysockaddr.sin_port = htons(Port);
	if (bind(mysocket, (sockaddr *)&mysockaddr, sizeof(mysockaddr)) != 0)
	{
		cout << "\n Bind loi.";
		exit(EXIT_FAILURE);
	}
	if (listen(mysocket, 3) != 0)
	{
		cout << "\n Listen loi.";
		exit(EXIT_FAILURE);
	}
}

void RCSocket::MakeServer(const char* Server, short Port)
{
	unsigned long ServerInT;
	inet_pton(AF_INET, Server, &ServerInT);
	MakeServer(ServerInT, Port);
}

void RCSocket::Connect(unsigned long Server, short Port)
{
	mysockaddr.sin_family = AF_INET;
	mysockaddr.sin_addr.s_addr = Server;
	mysockaddr.sin_port = htons(Port);
	if (connect(mysocket, (sockaddr *)&mysockaddr, sizeof(mysockaddr)) != 0)
	{
		cout << "\n Connect loi.";
		exit(EXIT_FAILURE);
	}
}

void RCSocket::Connect(char * Server, short Port)
{
	unsigned long ServerInT;
	inet_pton(AF_INET, Server, &ServerInT);
	Connect(ServerInT, Port);
}
