#pragma comment(lib, "ws2_32")
#include <WinSock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <ws2bth.h> //블루투스
#include <initGuid.h> //GUID -->식별자


#define SERVERADDR "50:77:05:77:83:00 " //블루투스 장치 검색할때의 물리적 주소
#define BUFSIZE 512

//블루투스 포트번호 : BT_PORT_ANY : 자동할당(1~30)

//GUID할당 -> 도구->GUID만들기 ->복사 

DEFINE_GUID(BthServer_Service,
	0x99b580c6, 0x4acc, 0x4888, 0xb4, 0x61, 0x22, 0xe4, 0x6d, 0x53, 0xba, 0x5f);

void err_quit(char *msg) {
	//오류나면 종료
}
void err_display(char *msg) {
	//오류나면 메시지 출력
}
int main()
{
	int retval;

	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		return 1;
	}

	//장치검색 -> 카페참조 (p555 49-101라인 생략) 생략.


	//소켓생성
	SOCKET sock = socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);
	if (sock == INVALID_SOCKET) {
		err_quit("socket()");
	}

	//CONNECT
	int serverport = 0;

	SOCKADDR_BTH serveraddr; //p65참조
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	int addrlen = sizeof(serveraddr);
	WSAStringToAddress(SERVERADDR, AF_BTH, NULL, (SOCKADDR *)&serveraddr, &addrlen);
	serveraddr.port = BT_PORT_ANY; //자동할당(1-30)

	retval = connect(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));

	if (retval == SOCKET_ERROR) {
		err_quit("connect()");
	}

	char buf[BUFSIZE + 1];
	int len;


	while (1) {
		//사용자로부터 입력받기 
		printf("\n[보낼 데이터] :");
		if (fgets(buf, BUFSIZE + 1, stdin) == NULL) {
			break;
		}
		
		len = strlen(buf); 
		if (buf[len - 1] == '\n') {
			buf[len - 1] = '\0';
		}
		if (strlen(buf) == 0) {
			break;
		}
		
		retval = send(sock, buf, strlen(buf), 0);
		if (retval == SOCKET_ERROR) {
			err_display("send()");
			break;
		}

		printf("블루투스 %d 바이트를 보냈습니다\n", retval);
	}
	closesocket(sock);
	WSACleanup();
}

