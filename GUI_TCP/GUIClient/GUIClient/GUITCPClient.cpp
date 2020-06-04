#define _CRT_SECURE_NO_WARNINGS

#include<stdio.h>
#include <WinSock2.h>
#include <stdlib.h>
#include"resource.h"

#define SERVERIP "127.0.0.1" //���� ip�ּ�
#define SERVERPORT 9000 //��Ʈ��ȣ(����)
#define BUFSIZE 512 //������ ������� ũ��


//�Լ� ������ ����

//1>��ȭ�������ν���
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
//4>������ ���� �Լ�
int recvn(SOCKET s, char *buf, int len, int flags);

//������ ���� �����Լ�
DWORD WINAPI ClientMain(LPVOID arg);

SOCKET sock;
char buf[BUFSIZE + 1];
HANDLE hReadEvent, hWriteEvent;
//������ ����ȭ
HWND hSendBUtton;
HWND hEdit1, hEdit2;

//������ ����
int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hPrevinstance,
	LPSTR IpCmdLine, int nCmdShow){


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


//��ȭ���� ���ν���
BOOL CALLBACK DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	
	switch (uMsg) {
	case WM_INITDIALOG : //��ȭ���� �ʱ�ȭ
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



//���� ��Ʈ�� ��� �Լ�
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


//������ �����Լ� 
int recvn(SOCKET s, char *buf, int len, int flags) {
	int received;//���� �����Ͱ�
	char *ptr = buf; //ptr�� �����ϸ� �����͸� ����
	int left = len; //������ �����Ͱ�

	while (left > 0) {
		received = recv(s, ptr, left, flags);
		if (received == SOCKET_ERROR) {
			return SOCKET_ERROR;
		}
		else if (received == 0) {
			break;
		}
		left -= received;
		ptr += received;
	
	}
	return(len - left); //��ü ������������ - > ���������ͷ� 
}

DWORD WINAPI ClientMain(LPVOID arg) {

	int retval;
	//�����ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		return 1;
	}
		SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
		if (sock == INVALID_SOCKET) {
			err_quit("socket()");

		}

		SOCKADDR_IN serveraddr;
		ZeroMemory(&serveraddr, sizeof(serveraddr));
		serveraddr.sin_family = AF_INET;
		serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
		//inet_Addr("���ڿ� ������"): ���ڿ��� ���� (32��Ʈ)
		serveraddr.sin_port = htons(SERVERPORT);
		retval = connect(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));


		//����
		if (retval == SOCKET_ERROR) {
			err_quit("connect()");
		}



		//������ ����϶� �ణ ����
		while (1) {
			WaitForSingleObject(hWriteEvent, INFINITE); //�����ȣ �ٲ𶧱��� ��ٸ���
			if (strlen(buf) == 0) {
				EnableWindow(hSendBUtton, TRUE);
				SetEvent(hReadEvent);
				continue;
			}

			//������ ������ ������
			retval = send(sock, buf, strlen(buf), 0);

			if (retval == SOCKET_ERROR) {
				err_display("send()");
				break;
			}
			DisplayText("TCPŬ���̾�Ʈ %d����Ʈ�� ���½��ϴ�\n", retval);

			//�����͹ޱ�
			retval = recvn(sock, buf, retval, 0);
			if (retval == SOCKET_ERROR) {
				err_display("recvn()");
				break;
			}
			else if (retval == 0) {
				break;
			}

			//���������� ���
			buf[retval] = '\0';
			DisplayText("TCPŬ���̾�Ʈ %d����Ʈ�� ���½��ϴ�\n", retval);
			DisplayText("����������%s\n", buf);;

			EnableWindow(hSendBUtton, TRUE);
			SetEvent(hReadEvent);
		}
		return 0;
	}


