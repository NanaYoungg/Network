//guitcpserver 참조(앞부분
//multiudpreceiver 참조
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "ws2_32")

#include<stdio.h>
#include <WinSock2.h>
#include <stdlib.h>
#include <WS2tcpip.h>
#define MULTICASTIP "235.7.8.9"

#define LOCALPORT 9000 //포트번호(서버)

#define BUFSIZE 512 //데이터 저장공간 크기







LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//핸들, 메시지 유형, 메시지 부가정보 (메시지 읽기, 읽기에 사용된 이벤트)



//편집 컨트롤 출력

void Displaytext(char *fmt, ...);

//오류

void error_quit(char *msg);

void error_display(char *msg);

//소켓 통신 스레드 함수

DWORD WINAPI ServerMain(LPVOID arg); // 서버와의 통신

DWORD WINAPI ProcessClient(LPVOID arg); //클라이언트와의 통신



HINSTANCE hInst; // 윈도우 체크할 핸들 변수

HWND hEdit; // 편집 컨트롤

CRITICAL_SECTION cs; // 오기화시키기 위한 임계영역 체크변수



int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,

	LPSTR lpCmdLine, int nCmdShow)

{

	hInst = hInstance;





	//추가

	InitializeCriticalSection(&cs); // 스레드 시작

									//작업: 초기화-> 스레드 실행(임계영역안에)->종료



									// 윈도우 클래스 등록 (1단계)

	WNDCLASS wndclass;

	wndclass.style = CS_HREDRAW | CS_VREDRAW; // 윈도우 스타일 

	wndclass.lpfnWndProc = WndProc; // 메시지 처리함수(중요)

	wndclass.cbClsExtra = 0;

	wndclass.cbWndExtra = 0; // 예약영역 사용여부==> 0(사용안함)

	wndclass.hInstance = hInstance; // 핸들

	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION); // 아이콘

	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW); // 커서

	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); // 배경색상

	wndclass.lpszMenuName = NULL; //메뉴사용 여부

	wndclass.lpszClassName = "MyWndClass2"; // 창에서 사용할 클래스 이름 

	if (!RegisterClass(&wndclass)) return 1;



	// 윈도우 생성

	HWND hWnd = CreateWindow("MyWndClass2", "멀티캐스트", WS_OVERLAPPEDWINDOW,

		0, 0, 600, 200, NULL, NULL, hInstance, NULL);

	//클래스 이름, 창 제목, 창 모양(스타일) ,x,y,w,h

	if (hWnd == NULL) return 1;

	ShowWindow(hWnd, nCmdShow); // 창 보여주기

	UpdateWindow(hWnd); // 창 업데이트 하기





						//소켓 생성 ==> 서버 연결 코드

	CreateThread(NULL, 0, ServerMain, NULL, 0, NULL);



	//메시지 루프

	MSG msg;

	while (GetMessage(&msg, 0, 0, 0) > 0) { // 버퍼에서 읽어오기

		TranslateMessage(&msg); // 읽어온 메시지 가공

		DispatchMessage(&msg); // 메시지 처리 함수에 전달하기

	}

	DeleteCriticalSection(&cs); // 임계영역 종료



	return msg.wParam;

}



// 메시지 처리 함수===>윈도우 프로시저

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

void DisplayText(char *fmt, ...) {  //가변 매개변수

	va_list arg; // va_list 가변 매개변수 리스트 선언==> 전달되는 매개변수의 값의 위치를 기억할 변수

	va_start(arg, fmt); // va_start(가변매개변수, 포인터) : 읽기 위한 준비



	char cbuf[256];

	vsprintf(cbuf, fmt, arg); // 버퍼 위치, 시작위치, 읽어올 위치==> 가변매개변수에 저장된 값을 읽어서 버퍼에 넣기



							  // 추가: 임계영역에 들어감

	EnterCriticalSection(&cs);



	// 화면 출력==>편집 컨트롤 화면에 

	int nLength = GetWindowTextLength(hEdit); // 핸들어안에 있는 값의 길이

	SendMessage(hEdit, EM_SETSEL, nLength, nLength);

	// 핸들, 타입, 길이 (EM_SETSEL : 새로운 값 설정(문자 범위 선택해 컨트롤에 보냄))

	SendMessage(hEdit, EM_REPLACESEL, FALSE, (LPARAM)cbuf);

	// 핸들 , EM_REPLACESEL :교체->버퍼에 저장



	LeaveCriticalSection(&cs); // 임계영역 벗어남



	va_end(arg); // 가변 매개변수 값 정리

}





//서버 소켓 관련 함수 --> 티씨피 서버에 있는 함수

//소켓 함수 오류

void err_quit(char *msg) {



}

void err_display(char *msg) {



}



DWORD WINAPI ServerMain(LPVOID arg) {

	//==>멀티캐스트 리시버



	int retval; //서버의 태를 체크할 변수

				//윈속 초기화

	WSADATA wsa;

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {



		return 1;

	}



	SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);

	if (sock == INVALID_SOCKET) { //소켓 생성이 오류가 났을 때

		err_display("socket()");

	}



	//ip주소 재사용 여부

	BOOL optval = TRUE;

	retval = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *)&optval, sizeof(optval));

	if (retval == SOCKET_ERROR) {

		err_quit("socketopt()");

	}









	SOCKADDR_IN serveraddr; //p.65 참조



	ZeroMemory(&serveraddr, sizeof(serveraddr));



	//zeromemory(위치, 크기) : 0으로 초기화



	serveraddr.sin_family = AF_INET; //주소체계(IPv4)

	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY); //서버ip 주소

	serveraddr.sin_port = htons(LOCALPORT);

	// 포트번호==>호스트->네트워크 방식으로



	retval = bind(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));

	// bind (소켓,주소,주소길이)



	if (retval == SOCKET_ERROR) {



		err_quit("bind()");



	}



	//그룹 추가

	struct ip_mreq mreq;

	mreq.imr_multiaddr.s_addr = inet_addr(MULTICASTIP); //그룹IP

	mreq.imr_interface.s_addr = htonl(INADDR_ANY);//그룹안에 각각 IP

	retval = setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&mreq, sizeof(mreq));

	if (retval == SOCKET_ERROR) {

		err_quit("setsockopt()");

	}





	//데이터 통신에 사용할 변수 (소켓생성x)



	SOCKADDR_IN peeraddr; // ip와 포트번호

	int addrlen;

	char buf[BUFSIZE + 1]; //데이터 저장 공간 선언



	while (1) {

		addrlen = sizeof(peeraddr);

		retval = recvfrom(sock, buf, BUFSIZE, 0, (SOCKADDR *)&peeraddr, &addrlen);

		//소켓정보,버퍼주소,크기,상태(0),ip주소포트번호,길이



		if (retval == SOCKET_ERROR) {

			err_display("recvfrom()");

			continue;   //여러명꺼를 받아야하니까 컨티뉴

		}



		//받은데이터 출력

		buf[retval] = '\0';//실제 데이터 끝에 \0을 추가

		printf("[멀티%s:%d] %s\n", inet_ntoa(peeraddr.sin_addr),

			ntohs(peeraddr.sin_family), buf);



		//탈퇴

		retval == setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&mreq, sizeof(mreq));



		if (retval == SOCKET_ERROR) {

			err_quit("setsockopt()");



		}




	}

	closesocket(sock);

	WSACleanup();

}

