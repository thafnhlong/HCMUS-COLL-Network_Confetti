#include "CConfetti.h"

int main()
{
	cout << " Confetti Server";

	string IP;
	int PORT;
	cout << "\n Nhap ip may : ";
	getline(cin, IP);
	cout << "\n Nhap port: ";
	cin >> PORT;
	cin.ignore();

	cout << "\n Nhap so luong client toi da: ";
	int MaxClient = 0;
	cin >> MaxClient;
	CConfetti *pConfetti = CConfetti::GetInstance(IP, PORT);
	cout << "\n An enter de bat dau tro choi, hoac neu so nguoi choi = so client toi da";
	cout << "\n Danh sach may con ket noi: ";
	pConfetti->StartLogin(MaxClient);
	getchar(); getchar(); // 2lan
	pConfetti->StopLogin();

	pConfetti->StartGame();

	pConfetti->StopGame();

	pConfetti->Release();

	system("pause");
	return 0;
}