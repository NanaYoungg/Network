#pragma comment(lib, "ws2_32")
#include <WinSock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <ws2bth.h> //블루투스
#include <initGuid.h> //GUID -->식별자

#define BUFSIZE 512

//블루투스 포트번호 : BT_PORT_ANY : 자동할당(1~30)

//GUID할당 -> 도구->GUID만들기 ->복사 

DEFINE_GUID(BthServer_Service ,
	0x99b580c6, 0x4acc, 0x4888, 0xb4, 0x61, 0x22, 0xe4, 0x6d, 0x53, 0xba, 0x5f);

//오류출력함수

void err_quit(char *msg) {
	//오류나면 종료
}
void err_display(char *msg) {
	//오류나면 메시지 출력
}

int main()  //소켓(블루투스)->bind(블루투스)->listen
{
	int retval;

	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		return 1;
	}

	//bind ->주소정보 변경 . 블루투스로


	//소켓생성
	SOCKET listen_sock = socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);
	if (listen_sock == INVALID_SOCKET) {
		err_quit("socket()");
	}


	//2단계 : ip주소, 포트설정 ==> bind(소켓, 주소정보, 주소길이)
	SOCKADDR_BTH serveraddr; //p65참조
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.addressFamily = AF_BTH;
	serveraddr.btAddr = 0; //블루투스 장치주소
	serveraddr.port = BT_PORT_ANY; //자동할당(1-30)

	retval = bind(listen_sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	//bind(소켓, 주소,주소길이)
	if (retval == SOCKET_ERROR) {
		err_quit("bind()");
	}

	//서버 포트 번호 출력(자동할당인데 보고싶을때)
	int addrlen = sizeof(serveraddr);
	retval = getsockname(listen_sock, (SOCKADDR *)&serveraddr,&addrlen);
	printf("블루투스 포트번호 %d\n", serveraddr.port);


	//서버정보 등록
	CSADDR_INFO addrinfo;
	addrinfo.LocalAddr.lpSockaddr = (SOCKADDR *)&serveraddr;
	addrinfo.LocalAddr.iSockaddrLength = sizeof(serveraddr);

	addrinfo.RemoteAddr.lpSockaddr = (SOCKADDR *)&serveraddr;
	addrinfo.RemoteAddr.iSockaddrLength = sizeof(serveraddr);
	addrinfo.iSocketType = SOCK_STREAM;
	addrinfo.iProtocol = BTHPROTO_RFCOMM;

	//블루투스 장치 정보

	WSAQUERYSET qset;
	ZeroMemory(&qset, sizeof(qset));

	qset.dwSize = sizeof(qset);
	qset.dwNameSpace = NS_BTH;
	qset.lpServiceClassId = (GUID*)&BthServer_Service; //gui이름
	qset.dwNumberOfCsAddrs = 1;
	qset.lpcsaBuffer = &addrinfo;
	retval = WSASetService(&qset, RNRSERVICE_REGISTER,0); //서버등록

	//listen

	retval = listen(listen_sock, 1);
	if (retval == SOCKET_ERROR) {
		err_quit("listen()");
	}

	//데이터 통신 사용할 변수

	SOCKET client_sock; 
	SOCKADDR_BTH clientaddr;

	char buf[BUFSIZE + 1];




	while (1) {
		addrlen = sizeof(clientaddr);
		//4단계
		client_sock = accept(listen_sock,
			(SOCKADDR *)&clientaddr, &addrlen);
		//accept(소켓,클라이언트주소,길이)
		if (client_sock == INVALID_SOCKET) {
			err_display("accept()");
			break;//무한반복 종료
		}

		//접속한 클라이언트 정보출력
	

		while (1) {
	
			retval = recv(client_sock, buf, BUFSIZE, 0);
			if (retval == SOCKET_ERROR) {
				err_display("recv()");
				break;
			}
			else if (retval == 0) { //종료가 된 경우
				break;
			}

			buf[retval] = '\0';
			printf(" 블루투스 서버 %d \n", buf);
			}

		//7단계 : 소켓종료
		closesocket(client_sock);
		printf("클라이언트 종료");


		}


	closesocket(listen_sock);
	WSACleanup();


}