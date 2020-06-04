//guitcpserver ����(�պκ�
//multiudpreceiver ����
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "ws2_32")

#include<stdio.h>
#include <WinSock2.h>
#include <stdlib.h>
#include <WS2tcpip.h>
#define MULTICASTIP "235.7.8.9"

#define LOCALPORT 9000 //��Ʈ��ȣ(����)

#define BUFSIZE 512 //������ ������� ũ��







LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//�ڵ�, �޽��� ����, �޽��� �ΰ����� (�޽��� �б�, �б⿡ ���� �̺�Ʈ)



//���� ��Ʈ�� ���

void Displaytext(char *fmt, ...);

//����

void error_quit(char *msg);

void error_display(char *msg);

//���� ��� ������ �Լ�

DWORD WINAPI ServerMain(LPVOID arg); // �������� ���

DWORD WINAPI ProcessClient(LPVOID arg); //Ŭ���̾�Ʈ���� ���



HINSTANCE hInst; // ������ üũ�� �ڵ� ����

HWND hEdit; // ���� ��Ʈ��

CRITICAL_SECTION cs; // ����ȭ��Ű�� ���� �Ӱ迵�� üũ����



int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,

	LPSTR lpCmdLine, int nCmdShow)

{

	hInst = hInstance;





	//�߰�

	InitializeCriticalSection(&cs); // ������ ����

									//�۾�: �ʱ�ȭ-> ������ ����(�Ӱ迵���ȿ�)->����



									// ������ Ŭ���� ��� (1�ܰ�)

	WNDCLASS wndclass;

	wndclass.style = CS_HREDRAW | CS_VREDRAW; // ������ ��Ÿ�� 

	wndclass.lpfnWndProc = WndProc; // �޽��� ó���Լ�(�߿�)

	wndclass.cbClsExtra = 0;

	wndclass.cbWndExtra = 0; // ���࿵�� ��뿩��==> 0(������)

	wndclass.hInstance = hInstance; // �ڵ�

	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION); // ������

	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW); // Ŀ��

	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); // ������

	wndclass.lpszMenuName = NULL; //�޴���� ����

	wndclass.lpszClassName = "MyWndClass2"; // â���� ����� Ŭ���� �̸� 

	if (!RegisterClass(&wndclass)) return 1;



	// ������ ����

	HWND hWnd = CreateWindow("MyWndClass2", "��Ƽĳ��Ʈ", WS_OVERLAPPEDWINDOW,

		0, 0, 600, 200, NULL, NULL, hInstance, NULL);

	//Ŭ���� �̸�, â ����, â ���(��Ÿ��) ,x,y,w,h

	if (hWnd == NULL) return 1;

	ShowWindow(hWnd, nCmdShow); // â �����ֱ�

	UpdateWindow(hWnd); // â ������Ʈ �ϱ�





						//���� ���� ==> ���� ���� �ڵ�

	CreateThread(NULL, 0, ServerMain, NULL, 0, NULL);



	//�޽��� ����

	MSG msg;

	while (GetMessage(&msg, 0, 0, 0) > 0) { // ���ۿ��� �о����

		TranslateMessage(&msg); // �о�� �޽��� ����

		DispatchMessage(&msg); // �޽��� ó�� �Լ��� �����ϱ�

	}

	DeleteCriticalSection(&cs); // �Ӱ迵�� ����



	return msg.wParam;

}



// �޽��� ó�� �Լ�===>������ ���ν���

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

void DisplayText(char *fmt, ...) {  //���� �Ű�����

	va_list arg; // va_list ���� �Ű����� ����Ʈ ����==> ���޵Ǵ� �Ű������� ���� ��ġ�� ����� ����

	va_start(arg, fmt); // va_start(�����Ű�����, ������) : �б� ���� �غ�



	char cbuf[256];

	vsprintf(cbuf, fmt, arg); // ���� ��ġ, ������ġ, �о�� ��ġ==> �����Ű������� ����� ���� �о ���ۿ� �ֱ�



							  // �߰�: �Ӱ迵���� ��

	EnterCriticalSection(&cs);



	// ȭ�� ���==>���� ��Ʈ�� ȭ�鿡 

	int nLength = GetWindowTextLength(hEdit); // �ڵ��ȿ� �ִ� ���� ����

	SendMessage(hEdit, EM_SETSEL, nLength, nLength);

	// �ڵ�, Ÿ��, ���� (EM_SETSEL : ���ο� �� ����(���� ���� ������ ��Ʈ�ѿ� ����))

	SendMessage(hEdit, EM_REPLACESEL, FALSE, (LPARAM)cbuf);

	// �ڵ� , EM_REPLACESEL :��ü->���ۿ� ����



	LeaveCriticalSection(&cs); // �Ӱ迵�� ���



	va_end(arg); // ���� �Ű����� �� ����

}





//���� ���� ���� �Լ� --> Ƽ���� ������ �ִ� �Լ�

//���� �Լ� ����

void err_quit(char *msg) {



}

void err_display(char *msg) {



}



DWORD WINAPI ServerMain(LPVOID arg) {

	//==>��Ƽĳ��Ʈ ���ù�



	int retval; //������ �¸� üũ�� ����

				//���� �ʱ�ȭ

	WSADATA wsa;

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {



		return 1;

	}



	SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);

	if (sock == INVALID_SOCKET) { //���� ������ ������ ���� ��

		err_display("socket()");

	}



	//ip�ּ� ���� ����

	BOOL optval = TRUE;

	retval = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *)&optval, sizeof(optval));

	if (retval == SOCKET_ERROR) {

		err_quit("socketopt()");

	}









	SOCKADDR_IN serveraddr; //p.65 ����



	ZeroMemory(&serveraddr, sizeof(serveraddr));



	//zeromemory(��ġ, ũ��) : 0���� �ʱ�ȭ



	serveraddr.sin_family = AF_INET; //�ּ�ü��(IPv4)

	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY); //����ip �ּ�

	serveraddr.sin_port = htons(LOCALPORT);

	// ��Ʈ��ȣ==>ȣ��Ʈ->��Ʈ��ũ �������



	retval = bind(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));

	// bind (����,�ּ�,�ּұ���)



	if (retval == SOCKET_ERROR) {



		err_quit("bind()");



	}



	//�׷� �߰�

	struct ip_mreq mreq;

	mreq.imr_multiaddr.s_addr = inet_addr(MULTICASTIP); //�׷�IP

	mreq.imr_interface.s_addr = htonl(INADDR_ANY);//�׷�ȿ� ���� IP

	retval = setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&mreq, sizeof(mreq));

	if (retval == SOCKET_ERROR) {

		err_quit("setsockopt()");

	}





	//������ ��ſ� ����� ���� (���ϻ���x)



	SOCKADDR_IN peeraddr; // ip�� ��Ʈ��ȣ

	int addrlen;

	char buf[BUFSIZE + 1]; //������ ���� ���� ����



	while (1) {

		addrlen = sizeof(peeraddr);

		retval = recvfrom(sock, buf, BUFSIZE, 0, (SOCKADDR *)&peeraddr, &addrlen);

		//��������,�����ּ�,ũ��,����(0),ip�ּ���Ʈ��ȣ,����



		if (retval == SOCKET_ERROR) {

			err_display("recvfrom()");

			continue;   //�������� �޾ƾ��ϴϱ� ��Ƽ��

		}



		//���������� ���

		buf[retval] = '\0';//���� ������ ���� \0�� �߰�

		printf("[��Ƽ%s:%d] %s\n", inet_ntoa(peeraddr.sin_addr),

			ntohs(peeraddr.sin_family), buf);



		//Ż��

		retval == setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&mreq, sizeof(mreq));



		if (retval == SOCKET_ERROR) {

			err_quit("setsockopt()");



		}




	}

	closesocket(sock);

	WSACleanup();

}

