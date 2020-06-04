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

//윈도우 프로시저-> 메시지 처리함수
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
//핸들, 메시지유형, 메시지 부가정보

void DisplayText(char *fmf, ...); //가변매게변수를 가진 출력함수

void err_quit(char *msg) {
	//오류 나면 종료
	MessageBox(NULL, msg, "알림", MB_OKCANCEL);
}

void err_display(char *msg) {
	//오류 나면 메세지 뿌려주기 뿌려 버령><
	LPVOID lpMsgBuf; //오류메시지 정보를 저장할 변수
	FormatMessage(
		//dwFlags ==> 함수 호출 옵션
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, //LP소스
		WSAGetLastError(), // dwMessageid : 오류코드
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		//오류 메시지를 표시할 언어
		(LPTSTR)&lpMsgBuf, //오류메시지를 저장할 공간
		0, // 버퍼 기본사이트
		NULL);
	//MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	//메시지 출력 => 선택
	printf("[%s]%s", msg, (char *)lpMsgBuf);
	//메모리 지우기
	LocalFree(lpMsgBuf);
}

//소켓 통신 스레드 함수
DWORD WINAPI ServerMain(LPVOID arg); //서버연결정보
DWORD WINAPI ProcessClient(LPVOID arg);//클라이언트롸의 통신


HINSTANCE hInst; //윈도우 체크할 핸들 변수
HWND hEdit; //편집 컨트롤
CRITICAL_SECTION cs;  //동기화시키기 위한 임계영역체크변수


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	hInst = hInstance;

	//추가:
	InitializeCriticalSection(&cs); //스레드 초기화 시작
									//작업: 초기화 -> 스레드실행(임계영역안에--) --> 종료

									// 윈도우 클래스 등록
	WNDCLASS wndclass;
	wndclass.style = CS_HREDRAW | CS_VREDRAW;  //윈도우 스타일
	wndclass.lpfnWndProc = WndProc; //메시지 처리함수 (중요)
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0; //예약영역 사용여부 -> 0(사용안함)
	wndclass.hInstance = hInstance; //핸들
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION); //아이콘
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);//카사
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); //배경색상
	wndclass.lpszMenuName = NULL; //메뉴사용 여부 (널 사용안함)
	wndclass.lpszClassName = "MyWndClass";//창에서 사용할 클래스 이름 
	if (!RegisterClass(&wndclass)) return 1;

	// 윈도우 생성(2단계)
	HWND hWnd = CreateWindow("MyWndClass", "WinApp", WS_OVERLAPPEDWINDOW,
		0, 0, 600, 200, NULL, NULL, hInstance, NULL);
	//클래스이름, 창제목, 창모양(스타일), x, y, w, h
	if (hWnd == NULL) return 1;
	ShowWindow(hWnd, nCmdShow); //창 보여주기
	UpdateWindow(hWnd);  //창 업데이트 하기

						 //소켓 통신 스레드 생성 ==> 서버 연결 코드
	CreateThread(NULL, 0, ServerMain, NULL, 0, NULL);

	// 메시지 루프
	MSG msg;
	while (GetMessage(&msg, 0, 0, 0) > 0) { //버퍼에서 읽어오기
		TranslateMessage(&msg); //읽어온 메시지 가공
		DispatchMessage(&msg); //메시지 처리 함수에 전달하기
	}
	DeleteCriticalSection(&cs); //임계영역 종료
	return msg.wParam;
}

//메시지 처리 함수 ==> 윈도우 프로시저
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

//편집 컨트롤 출력 함수
void DisplayText(char *fmt, ...) //가변매개변수
{
	va_list arg; //va_list : 가변 매개변수 리스트 선언 ==> 전달되는 매개변수의 값의 위치를 기억할 변수
	va_start(arg, fmt); //va_start(가변매개변수, 포인터) : 읽기위한 준비

	char cbuf[256];
	vsprintf(cbuf, fmt, arg); //버퍼, 시작위치, 읽어올 위치 ==> 가변매개변수에 저장된 값을 읽어서 버퍼에 넣기

							  //추가 : 임계영역에 들아감
	EnterCriticalSection(&cs);

	//화면출력 ==> 편집컨트롤화면에 
	int nLength = GetWindowTextLength(hEdit); //핸들러안에 있는 값의 길이
	SendMessage(hEdit, EM_SETSEL, nLength, nLength);
	//핸들, 타입, 길이 (EM_SETSEL : 새로운 값 설정[문자 범위 선택해서 컨트롤에 보냄])
	SendMessage(hEdit, EM_REPLACESEL, FALSE, (LPARAM)cbuf);
	//핸들, EM_REPLACESEL : 교체 ==> 버퍼에 저장

	LeaveCriticalSection(&cs); //임계영역 벗어남

	va_end(arg); //가변매개변수 값 정리
}

//서버 소켓 관련 함수... ==> TCPServer쪽에 있는 코드
//소켓 함수 오류


//함수 분리 ==> 서버소켓 연결 ==> servermain
DWORD WINAPI ServerMain(LPVOID arg) {

	int retval; //서버의 상태를 체크할 변수 

				//윈속 초기화 
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		return 1;
	}

	//소켓 생성  socket(주소체계, 소켓방식, 프로토콜)
	//소켓 : TCP : SOCK_STREAM, UDP : SOCK_DGRAM
	//프로토콜 : 0

	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET) { //소켓 생성이 오류가 났을때
										 //  err_display("socket()");
	}

	//bind() : ip주소와 포트번호설정하는 단계
	SOCKADDR_IN serveraddr;//서버주소체계에 접근할 변수 
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	//ZeroMemory(위치, 크기) : 해당 영역을 초기화 
	serveraddr.sin_family = AF_INET; //ipv4 방식 
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	//htonl : 호스트 --> 네트워크방식 
	//INADDR_ANY : 현재 컴퓨터에 있는 클라이언트는 누구나 접속가능
	serveraddr.sin_port = htons(SERVERPORT);
	//포트 번호 설정 

	retval = bind(listen_sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	//bind(소켓정보, 주소정보, 주소정보의 길이)
	if (retval == SOCKET_ERROR) {
		err_display("bind()");
	}
	//listen() : 대기상태==> bind설정이 이루어진 후 클라이언트의 정보(=접속)를 받을 수 있는 준비단계
	//IP 주소 확인할때 ==> 상태 정보 중 (LISTENING )

	retval = listen(listen_sock, SOMAXCONN); //listen(소켓정보, 연결갯수)
	if (retval == SOCKET_ERROR) {
		err_display("listen()");
	}

	//accept() 을 위한 준비 ==> 클라이언트연결 요청이 들어오면 받아들일 준비

	SOCKET client_sock; //클라이언트 소켓 정보 (클라이언트 구분)
	SOCKADDR_IN clientaddr; //클라이언트의 ip와 포트번호 
	int addrlen; //클라이언트 주소 의 길이 
				 //추가 (165 LINE)

	HANDLE hThread; //스레드 관리해줄 변수

	while (1) {
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR *)&clientaddr, &addrlen);
		//accept(소켓정보, 클라이언트주소, 길이)
		if (client_sock == INVALID_SOCKET) {
			err_display("accept()");
			break;
		}

		//수정 : 접속한 클라이언트 정보를 ==> 서버프로그램에 출력 
		DisplayText("\r\n [TCP 서버] 클라이언트 접속: IP주소=%s, 포트 번호=%d\r\n",
			inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

		//스레드 생성
		hThread = CreateThread(NULL, 0, ProcessClient, (LPVOID)client_sock, 0, NULL);

		//스레드 생성 오류
		if (hThread == NULL) {
			closesocket(client_sock); //클라이언트 소켓
		}
		else {
			CloseHandle(hThread);
		}

	}
	closesocket(listen_sock); //서버소켓
	WSACleanup();//윈속 종료 

	return 0;
}

//함수 분리 ==> 서버와 클라이언트와의 통신 ==> processclient
DWORD WINAPI ProcessClient(LPVOID arg) {
	SOCKET client_sock = (SOCKET)arg;
	int retval;
	SOCKADDR_IN clientaddr;
	int addrlen;
	char buf[BUFSIZE + 1];

	//클라이언트 정보 얻기
	addrlen = sizeof(clientaddr);
	getpeername(client_sock, (SOCKADDR *)&clientaddr, &addrlen);

	while (1) {
		//데이터 받기
		retval = recv(client_sock, buf, BUFSIZE, 0);
		if (retval == SOCKET_ERROR) {
			err_display("recv()");
			break;
		}
		else if (retval == 0)
			break;

		//받은 데이터 출력
		buf[retval] = '\0';
		DisplayText("[TCP/%s:%d] %s\r\n",
			inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port), buf);

		//데이터 보내기
		retval = send(client_sock, buf, retval, 0);
		//send(소켓, 버퍼주소, 보낼데이길이, 상태정보 ==>송신버퍼
		if (retval == SOCKET_ERROR) {
			err_display("send()");
			break;
		}
	}

	closesocket(client_sock); //서버소켓
	DisplayText("[TCP서버] 클라이언트 종료 : IP 주소=%s, 포트번호 =%d\r\n",
		inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

	return 0;
}