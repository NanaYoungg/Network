#define _CRT_SECURE_NO_WARNINGS

#include<stdio.h>
#include <WinSock2.h>
#include <stdlib.h>
#include"resource.h"

#define SERVERIP "127.0.0.1" //서버 ip주소
#define SERVERPORT 9000 //포트번호(서버)
#define BUFSIZE 512 //데이터 저장공간 크기


//함수 원형들 정의

//1>대화상자프로시저
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
//4>데이터 수신 함수
int recvn(SOCKET s, char *buf, int len, int flags);

//스레드 생성 관련함수
DWORD WINAPI ClientMain(LPVOID arg);

SOCKET sock;
char buf[BUFSIZE + 1];
HANDLE hReadEvent, hWriteEvent;
//스레드 동기화
HWND hSendBUtton;
HWND hEdit1, hEdit2;

//윈도우 메인
int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hPrevinstance,
	LPSTR IpCmdLine, int nCmdShow){


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


//대화상자 프로시저
BOOL CALLBACK DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	
	switch (uMsg) {
	case WM_INITDIALOG : //대화상자 초기화
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



//편집 컨트롤 출력 함수
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


//데이터 수신함수 
int recvn(SOCKET s, char *buf, int len, int flags) {
	int received;//읽은 데이터값
	char *ptr = buf; //ptr은 증가하면 데이터를 읽음
	int left = len; //안읽은 데이터값

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
	return(len - left); //전체 안읽은데이터 - > 읽은데이터로 
}

DWORD WINAPI ClientMain(LPVOID arg) {

	int retval;
	//윈속초기화
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
		//inet_Addr("문자열 아이피"): 문자열을 숫자 (32비트)
		serveraddr.sin_port = htons(SERVERPORT);
		retval = connect(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));


		//에러
		if (retval == SOCKET_ERROR) {
			err_quit("connect()");
		}



		//서버와 통신일때 약간 수정
		while (1) {
			WaitForSingleObject(hWriteEvent, INFINITE); //쓰기신호 바뀔때까지 기다리기
			if (strlen(buf) == 0) {
				EnableWindow(hSendBUtton, TRUE);
				SetEvent(hReadEvent);
				continue;
			}

			//서버에 데이터 보내기
			retval = send(sock, buf, strlen(buf), 0);

			if (retval == SOCKET_ERROR) {
				err_display("send()");
				break;
			}
			DisplayText("TCP클라이언트 %d바이트를 보냈습니다\n", retval);

			//데이터받기
			retval = recvn(sock, buf, retval, 0);
			if (retval == SOCKET_ERROR) {
				err_display("recvn()");
				break;
			}
			else if (retval == 0) {
				break;
			}

			//받은데이터 출력
			buf[retval] = '\0';
			DisplayText("TCP클라이언트 %d바이트를 보냈습니다\n", retval);
			DisplayText("받은데이터%s\n", buf);;

			EnableWindow(hSendBUtton, TRUE);
			SetEvent(hReadEvent);
		}
		return 0;
	}


