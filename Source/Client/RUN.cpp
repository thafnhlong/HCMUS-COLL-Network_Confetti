#include "CConfertti.h"

int main()
{
	string IP;
	int PORT;
	cout << "\n Nhap ip may chu: ";
	getline(cin, IP);
	cout << "\n Nhap port: ";
	cin >> PORT;
	cin.ignore();

	CConfetti *pConfetti = CConfetti::GetInstance(IP, PORT);
	pConfetti->Login();
	pConfetti->StartGame();
	pConfetti->Release();

	system("pause");
	return 0;
}