#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "ws2_32")

#include <WinSock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <ws2tcpip.h> //add_memebeship -->��Ƽĳ��Ʈ �Լ� 
#include "resource.h"
#define MULTICASTIP "235.7.8.9"  //224~239 ���� �ȿ���
//��ε� : 255.255.255.255
//TCP/UDP : ������(127.0.0.1)
#define REMOTEPORT 9000 
#define LOCALPORT 9000
//��Ʈ��ȣ(����)
#define BUFSIZE 512 //������ ������� ũ��
//�Լ� ����-> gui client�����ؼ� �ֱ�
BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);
//2>����Լ�

void DisplayText(char *fmt, ...);

//3>�������

void err_quit(char *msg) {

	//������������

}

void err_display(char *msg) {

	//���޳��� �޼��� ���

}

//������ ���� �����Լ�

DWORD WINAPI ClientMain(LPVOID arg);



SOCKET sock;

char buf[BUFSIZE + 1];

HANDLE hReadEvent, hWriteEvent;

//������ ����ȭ

HWND hSendBUtton;

HWND hEdit1, hEdit2;



int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hPrevinstance,

	LPSTR IpCmdLine, int nCmdShow) {





	//�̺�Ʈ ����

	hReadEvent = CreateEvent(NULL, FALSE, TRUE, NULL);

	hWriteEvent = CreateEvent(NULL, FALSE, FALSE, NULL);





	//������ ����

	CreateThread(NULL, 0, ClientMain, NULL, 0, NULL);

	//��ȭ���� ����





	DialogBox(hinstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc);





	//1> �ڵ鷯

	//2> ���ø� ID��ȣ

	//3> �θ� ������

	//4> ó���� ���ν��� �̸�



	//������

	CloseHandle(hReadEvent);

	CloseHandle(hWriteEvent);



	return 0;



}



BOOL CALLBACK DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)

{



	switch (uMsg) {

	case WM_INITDIALOG: //��ȭ���� �ʱ�ȭ

		hEdit1 = GetDlgItem(hDlg, IDC_EDIT1);

		hEdit2 = GetDlgItem(hDlg, IDC_EDIT2);

		SendMessage(hEdit1, EM_SETLIMITTEXT, BUFSIZE, 0);

		//��Ʈ�ѿ� �ִ� ���ڼ�

		return TRUE;



	case WM_COMMAND: //��ư ���

		switch (LOWORD(wParam)) {

		case IDOK:

			EnableWindow(hSendBUtton, FALSE);

			WaitForSingleObject(hReadEvent, INFINITE);

			GetDlgItemText(hDlg, IDC_EDIT1, buf, BUFSIZE + 1);

			//edit1->buf

			SetEvent(hWriteEvent);

			//DisplayText

			SendMessage(hEdit1, EM_SETSEL, 0, -1);

			return TRUE;

		case IDCANCEL:

			EndDialog(hDlg, IDCANCEL);

			return TRUE;

		}

		return FALSE;

	}

	return FALSE;

}



void DisplayText(char *fmt, ...) //�����Ű�����

{

	va_list arg; //va_list : ���� �Ű����� ����Ʈ ���� ==> ���޵Ǵ� �Ű������� ���� ��ġ�� ����� ����

	va_start(arg, fmt); //va_start(�����Ű�����, ������) : �б����� �غ�



	char cbuf[256];

	vsprintf(cbuf, fmt, arg); //����, ������ġ, �о�� ��ġ ==> �����Ű������� ����� ���� �о ���ۿ� �ֱ�



							  //ȭ����� ==> ������Ʈ��ȭ�鿡 

	int nLength = GetWindowTextLength(hEdit2); //�ڵ鷯�ȿ� �ִ� ���� ����

	SendMessage(hEdit2, EM_SETSEL, nLength, nLength);

	//�ڵ�, Ÿ��, ���� (EM_SETSEL : ���ο� �� ����[���� ���� �����ؼ� ��Ʈ�ѿ� ����])

	SendMessage(hEdit2, EM_REPLACESEL, FALSE, (LPARAM)cbuf);

	//�ڵ�, EM_REPLACESEL : ��ü ==> ���ۿ� ����



	va_end(arg); //�����Ű����� �� ����

}







DWORD WINAPI ClientMain(LPVOID) {

	int retval;



	WSADATA wsa;

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {

		return 1;

	}



	// 1�ܰ� : ���ϻ��� : socket (�ּ�ü��, ���Ϲ��, ��������)



	SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);

	// tcp : SOCK_STREAM, udp : SOCK_DGRAM 57�� ����



	if (sock == INVALID_SOCKET) {

		err_quit("socket()");

	}



	// broad�� �ٸ� �κ�-> ����-->��Ƽĳ��Ʈ TTL����



	int ttl = 2;

	retval = setsockopt(sock, IPPROTO_IP, IP_MULTICAST_TTL, (char *)&ttl, sizeof(ttl));

	//���� �ɼ���ü(IP), �ɼ��̸�(TTL), �����ּ�, ũ��

	if (retval == SOCKET_ERROR) {

		err_display("sendto()");

	}



	SOCKADDR_IN serveraddr;

	ZeroMemory(&serveraddr, sizeof(serveraddr));

	serveraddr.sin_family = AF_INET;

	serveraddr.sin_addr.s_addr = inet_addr(MULTICASTIP);

	//inet_addr("���ڿ� ip") : ���ڿ��� ����(32bit)

	serveraddr.sin_port = htons(REMOTEPORT);



	//�������� ���

	while (1) {

		WaitForSingleObject(hWriteEvent, INFINITE);



		//���ۿ� ���� 0�� ���

		if (strlen(buf) == 0) {

			EnableWindow(hSendBUtton, TRUE);

			SetEvent(hReadEvent);

			continue;

		}



		retval = sendto(sock, buf, strlen(buf), 0,

			(SOCKADDR *)&serveraddr, sizeof(serveraddr));



		//send+ connect

		if (retval == SOCKET_ERROR) {

			err_display("sendto()");

		}



		// ���� �޽��� ���

		DisplayText("udpŬ���̾�Ʈ %d����Ʈ�� ���½��ϴ�.\n", retval);



		EnableWindow(hSendBUtton, TRUE);

		SetEvent(hReadEvent);

	}





	closesocket(sock);//Ŭ���̾�Ʈ ����

	WSACleanup();//���� ����



}
