//318
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <Windows.h>

#define SERVERPORT 9000
#define BUFSIZE    512

//������ ���ν���-> �޽��� ó���Լ�
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
//�ڵ�, �޽�������, �޽��� �ΰ�����

void DisplayText(char *fmf, ...); //�����ŰԺ����� ���� ����Լ�

void err_quit(char *msg) {
	//���� ���� ����
	MessageBox(NULL, msg, "�˸�", MB_OKCANCEL);
}

void err_display(char *msg) {
	//���� ���� �޼��� �ѷ��ֱ� �ѷ� ����><
	LPVOID lpMsgBuf; //�����޽��� ������ ������ ����
	FormatMessage(
		//dwFlags ==> �Լ� ȣ�� �ɼ�
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, //LP�ҽ�
		WSAGetLastError(), // dwMessageid : �����ڵ�
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		//���� �޽����� ǥ���� ���
		(LPTSTR)&lpMsgBuf, //�����޽����� ������ ����
		0, // ���� �⺻����Ʈ
		NULL);
	//MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	//�޽��� ��� => ����
	printf("[%s]%s", msg, (char *)lpMsgBuf);
	//�޸� �����
	LocalFree(lpMsgBuf);
}

//���� ��� ������ �Լ�
DWORD WINAPI ServerMain(LPVOID arg); //������������
DWORD WINAPI ProcessClient(LPVOID arg);//Ŭ���̾�Ʈ���� ���


HINSTANCE hInst; //������ üũ�� �ڵ� ����
HWND hEdit; //���� ��Ʈ��
CRITICAL_SECTION cs;  //����ȭ��Ű�� ���� �Ӱ迵��üũ����


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	hInst = hInstance;

	//�߰�:
	InitializeCriticalSection(&cs); //������ �ʱ�ȭ ����
									//�۾�: �ʱ�ȭ -> ���������(�Ӱ迵���ȿ�--) --> ����

									// ������ Ŭ���� ���
	WNDCLASS wndclass;
	wndclass.style = CS_HREDRAW | CS_VREDRAW;  //������ ��Ÿ��
	wndclass.lpfnWndProc = WndProc; //�޽��� ó���Լ� (�߿�)
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0; //���࿵�� ��뿩�� -> 0(������)
	wndclass.hInstance = hInstance; //�ڵ�
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION); //������
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);//ī��
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); //������
	wndclass.lpszMenuName = NULL; //�޴���� ���� (�� ������)
	wndclass.lpszClassName = "MyWndClass";//â���� ����� Ŭ���� �̸� 
	if (!RegisterClass(&wndclass)) return 1;

	// ������ ����(2�ܰ�)
	HWND hWnd = CreateWindow("MyWndClass", "WinApp", WS_OVERLAPPEDWINDOW,
		0, 0, 600, 200, NULL, NULL, hInstance, NULL);
	//Ŭ�����̸�, â����, â���(��Ÿ��), x, y, w, h
	if (hWnd == NULL) return 1;
	ShowWindow(hWnd, nCmdShow); //â �����ֱ�
	UpdateWindow(hWnd);  //â ������Ʈ �ϱ�

						 //���� ��� ������ ���� ==> ���� ���� �ڵ�
	CreateThread(NULL, 0, ServerMain, NULL, 0, NULL);

	// �޽��� ����
	MSG msg;
	while (GetMessage(&msg, 0, 0, 0) > 0) { //���ۿ��� �о����
		TranslateMessage(&msg); //�о�� �޽��� ����
		DispatchMessage(&msg); //�޽��� ó�� �Լ��� �����ϱ�
	}
	DeleteCriticalSection(&cs); //�Ӱ迵�� ����
	return msg.wParam;
}

//�޽��� ó�� �Լ� ==> ������ ���ν���
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_CREATE:
		hEdit = CreateWindow("edit", NULL,
			WS_CHILD | WS_VISIBLE | WS_HSCROLL |
			WS_VSCROLL | ES_AUTOHSCROLL |
			ES_AUTOVSCROLL | ES_MULTILINE | ES_READONLY,
			0, 0, 0, 0, hWnd, (HMENU)100, hInst, NULL);
		return 0;
	case WM_SIZE:
		MoveWindow(hEdit, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
		return 0;
	case WM_SETFOCUS:
		SetFocus(hEdit);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

//���� ��Ʈ�� ��� �Լ�
void DisplayText(char *fmt, ...) //�����Ű�����
{
	va_list arg; //va_list : ���� �Ű����� ����Ʈ ���� ==> ���޵Ǵ� �Ű������� ���� ��ġ�� ����� ����
	va_start(arg, fmt); //va_start(�����Ű�����, ������) : �б����� �غ�

	char cbuf[256];
	vsprintf(cbuf, fmt, arg); //����, ������ġ, �о�� ��ġ ==> �����Ű������� ����� ���� �о ���ۿ� �ֱ�

							  //�߰� : �Ӱ迵���� ��ư�
	EnterCriticalSection(&cs);

	//ȭ����� ==> ������Ʈ��ȭ�鿡 
	int nLength = GetWindowTextLength(hEdit); //�ڵ鷯�ȿ� �ִ� ���� ����
	SendMessage(hEdit, EM_SETSEL, nLength, nLength);
	//�ڵ�, Ÿ��, ���� (EM_SETSEL : ���ο� �� ����[���� ���� �����ؼ� ��Ʈ�ѿ� ����])
	SendMessage(hEdit, EM_REPLACESEL, FALSE, (LPARAM)cbuf);
	//�ڵ�, EM_REPLACESEL : ��ü ==> ���ۿ� ����

	LeaveCriticalSection(&cs); //�Ӱ迵�� ���

	va_end(arg); //�����Ű����� �� ����
}

//���� ���� ���� �Լ�... ==> TCPServer�ʿ� �ִ� �ڵ�
//���� �Լ� ����


//�Լ� �и� ==> �������� ���� ==> servermain
DWORD WINAPI ServerMain(LPVOID arg) {

	int retval; //������ ���¸� üũ�� ���� 

				//���� �ʱ�ȭ 
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		return 1;
	}

	//���� ����  socket(�ּ�ü��, ���Ϲ��, ��������)
	//���� : TCP : SOCK_STREAM, UDP : SOCK_DGRAM
	//�������� : 0

	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET) { //���� ������ ������ ������
										 //  err_display("socket()");
	}

	//bind() : ip�ּҿ� ��Ʈ��ȣ�����ϴ� �ܰ�
	SOCKADDR_IN serveraddr;//�����ּ�ü�迡 ������ ���� 
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	//ZeroMemory(��ġ, ũ��) : �ش� ������ �ʱ�ȭ 
	serveraddr.sin_family = AF_INET; //ipv4 ��� 
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	//htonl : ȣ��Ʈ --> ��Ʈ��ũ��� 
	//INADDR_ANY : ���� ��ǻ�Ϳ� �ִ� Ŭ���̾�Ʈ�� ������ ���Ӱ���
	serveraddr.sin_port = htons(SERVERPORT);
	//��Ʈ ��ȣ ���� 

	retval = bind(listen_sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	//bind(��������, �ּ�����, �ּ������� ����)
	if (retval == SOCKET_ERROR) {
		err_display("bind()");
	}
	//listen() : ������==> bind������ �̷���� �� Ŭ���̾�Ʈ�� ����(=����)�� ���� �� �ִ� �غ�ܰ�
	//IP �ּ� Ȯ���Ҷ� ==> ���� ���� �� (LISTENING )

	retval = listen(listen_sock, SOMAXCONN); //listen(��������, ���᰹��)
	if (retval == SOCKET_ERROR) {
		err_display("listen()");
	}

	//accept() �� ���� �غ� ==> Ŭ���̾�Ʈ���� ��û�� ������ �޾Ƶ��� �غ�

	SOCKET client_sock; //Ŭ���̾�Ʈ ���� ���� (Ŭ���̾�Ʈ ����)
	SOCKADDR_IN clientaddr; //Ŭ���̾�Ʈ�� ip�� ��Ʈ��ȣ 
	int addrlen; //Ŭ���̾�Ʈ �ּ� �� ���� 
				 //�߰� (165 LINE)

	HANDLE hThread; //������ �������� ����

	while (1) {
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR *)&clientaddr, &addrlen);
		//accept(��������, Ŭ���̾�Ʈ�ּ�, ����)
		if (client_sock == INVALID_SOCKET) {
			err_display("accept()");
			break;
		}

		//���� : ������ Ŭ���̾�Ʈ ������ ==> �������α׷��� ��� 
		DisplayText("\r\n [TCP ����] Ŭ���̾�Ʈ ����: IP�ּ�=%s, ��Ʈ ��ȣ=%d\r\n",
			inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

		//������ ����
		hThread = CreateThread(NULL, 0, ProcessClient, (LPVOID)client_sock, 0, NULL);

		//������ ���� ����
		if (hThread == NULL) {
			closesocket(client_sock); //Ŭ���̾�Ʈ ����
		}
		else {
			CloseHandle(hThread);
		}

	}
	closesocket(listen_sock); //��������
	WSACleanup();//���� ���� 

	return 0;
}

//�Լ� �и� ==> ������ Ŭ���̾�Ʈ���� ��� ==> processclient
DWORD WINAPI ProcessClient(LPVOID arg) {
	SOCKET client_sock = (SOCKET)arg;
	int retval;
	SOCKADDR_IN clientaddr;
	int addrlen;
	char buf[BUFSIZE + 1];

	//Ŭ���̾�Ʈ ���� ���
	addrlen = sizeof(clientaddr);
	getpeername(client_sock, (SOCKADDR *)&clientaddr, &addrlen);

	while (1) {
		//������ �ޱ�
		retval = recv(client_sock, buf, BUFSIZE, 0);
		if (retval == SOCKET_ERROR) {
			err_display("recv()");
			break;
		}
		else if (retval == 0)
			break;

		//���� ������ ���
		buf[retval] = '\0';
		DisplayText("[TCP/%s:%d] %s\r\n",
			inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port), buf);

		//������ ������
		retval = send(client_sock, buf, retval, 0);
		//send(����, �����ּ�, �������̱���, �������� ==>�۽Ź���
		if (retval == SOCKET_ERROR) {
			err_display("send()");
			break;
		}
	}

	closesocket(client_sock); //��������
	DisplayText("[TCP����] Ŭ���̾�Ʈ ���� : IP �ּ�=%s, ��Ʈ��ȣ =%d\r\n",
		inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

	return 0;
}