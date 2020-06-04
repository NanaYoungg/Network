#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "ws2_32")

#include <WinSock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <ws2tcpip.h> //add_memebeship -->멀티캐스트 함수 
#include "resource.h"
#define MULTICASTIP "235.7.8.9"  //224~239 범위 안에서
//브로드 : 255.255.255.255
//TCP/UDP : 루프백(127.0.0.1)
#define REMOTEPORT 9000 
#define LOCALPORT 9000
//포트번호(서버)
#define BUFSIZE 512 //데이터 저장공간 크기
//함수 원형-> gui client복사해서 넣기
BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);
//2>출력함수

void DisplayText(char *fmt, ...);

//3>오류출력

void err_quit(char *msg) {

	//오류나면종료

}

void err_display(char *msg) {

	//오휴나면 메세지 출력

}

//스레드 생성 관련함수

DWORD WINAPI ClientMain(LPVOID arg);



SOCKET sock;

char buf[BUFSIZE + 1];

HANDLE hReadEvent, hWriteEvent;

//스레드 동기화

HWND hSendBUtton;

HWND hEdit1, hEdit2;



int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hPrevinstance,

	LPSTR IpCmdLine, int nCmdShow) {





	//이벤트 생성

	hReadEvent = CreateEvent(NULL, FALSE, TRUE, NULL);

	hWriteEvent = CreateEvent(NULL, FALSE, FALSE, NULL);





	//스래드 생성

	CreateThread(NULL, 0, ClientMain, NULL, 0, NULL);

	//대화상자 띄우기





	DialogBox(hinstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc);





	//1> 핸들러

	//2> 템플릿 ID번호

	//3> 부모 윈도우

	//4> 처리할 프로시저 이름



	//마무리

	CloseHandle(hReadEvent);

	CloseHandle(hWriteEvent);



	return 0;



}



BOOL CALLBACK DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)

{



	switch (uMsg) {

	case WM_INITDIALOG: //대화상자 초기화

		hEdit1 = GetDlgItem(hDlg, IDC_EDIT1);

		hEdit2 = GetDlgItem(hDlg, IDC_EDIT2);

		SendMessage(hEdit1, EM_SETLIMITTEXT, BUFSIZE, 0);

		//컨트롤에 최대 글자수

		return TRUE;



	case WM_COMMAND: //버튼 명령

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



void DisplayText(char *fmt, ...) //가변매개변수

{

	va_list arg; //va_list : 가변 매개변수 리스트 선언 ==> 전달되는 매개변수의 값의 위치를 기억할 변수

	va_start(arg, fmt); //va_start(가변매개변수, 포인터) : 읽기위한 준비



	char cbuf[256];

	vsprintf(cbuf, fmt, arg); //버퍼, 시작위치, 읽어올 위치 ==> 가변매개변수에 저장된 값을 읽어서 버퍼에 넣기



							  //화면출력 ==> 편집컨트롤화면에 

	int nLength = GetWindowTextLength(hEdit2); //핸들러안에 있는 값의 길이

	SendMessage(hEdit2, EM_SETSEL, nLength, nLength);

	//핸들, 타입, 길이 (EM_SETSEL : 새로운 값 설정[문자 범위 선택해서 컨트롤에 보냄])

	SendMessage(hEdit2, EM_REPLACESEL, FALSE, (LPARAM)cbuf);

	//핸들, EM_REPLACESEL : 교체 ==> 버퍼에 저장



	va_end(arg); //가변매개변수 값 정리

}







DWORD WINAPI ClientMain(LPVOID) {

	int retval;



	WSADATA wsa;

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {

		return 1;

	}



	// 1단계 : 소켓생성 : socket (주소체계, 소켓방식, 프로토콜)



	SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);

	// tcp : SOCK_STREAM, udp : SOCK_DGRAM 57쪽 참고



	if (sock == INVALID_SOCKET) {

		err_quit("socket()");

	}



	// broad와 다른 부분-> 수정-->멀티캐스트 TTL설정



	int ttl = 2;

	retval = setsockopt(sock, IPPROTO_IP, IP_MULTICAST_TTL, (char *)&ttl, sizeof(ttl));

	//소켓 옵션주체(IP), 옵션이름(TTL), 버퍼주소, 크기

	if (retval == SOCKET_ERROR) {

		err_display("sendto()");

	}



	SOCKADDR_IN serveraddr;

	ZeroMemory(&serveraddr, sizeof(serveraddr));

	serveraddr.sin_family = AF_INET;

	serveraddr.sin_addr.s_addr = inet_addr(MULTICASTIP);

	//inet_addr("문자열 ip") : 문자열을 숫자(32bit)

	serveraddr.sin_port = htons(REMOTEPORT);



	//서버와의 통신

	while (1) {

		WaitForSingleObject(hWriteEvent, INFINITE);



		//버퍼에 값이 0인 경우

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



		// 보낸 메시지 출력

		DisplayText("udp클라이언트 %d바이트를 보냈습니다.\n", retval);



		EnableWindow(hSendBUtton, TRUE);

		SetEvent(hReadEvent);

	}





	closesocket(sock);//클라이언트 소켓

	WSACleanup();//윈속 종료



}
