#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "ws2_32")
#include <WinSock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <ws2tcpip.h> //add_memebeship -->멀티캐스트 함수 

#define MULTICASTIP "237.10.19.10"  //224~239 범위 안에서
//브로드 : 255.255.255.255
//TCP/UDP : 루프백(127.0.0.1)

#define REMOTEPORT 9000 
#define LOCALPORT 9000
//포트번호(서버)
#define BUFSIZE 512 //데이터 저장공간 크기
void err_quit(char *msg) {
}
void err_display(char *msg) {
}
//sender함수정의


DWORD WINAPI SenderThread(LPVOID arg) {

	int retval; //소켓 상태를 체크할 변수

				//윈속 초기화

	
	//소켓
	SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
	//tcp : SOCK_STRAM , udp : SCOK_DGRAM
	if (sock == INVALID_SOCKET) {
		err_quit("socket()");
	}

	//브로드 캐스트 와 다른부분 수정  --> 멀티캐스트 TTL설정

	int ttl = 2;

	retval = setsockopt(sock, IPPROTO_IP, IP_MULTICAST_TTL, (char *)&ttl, sizeof(ttl));
	//소켓,옵션주체ip,옵션이름ttl,버퍼주소,크기

	if (retval == SOCKET_ERROR) {
		err_quit("sendto()");
	}

	//소켓 주소 체계 초기화 ----> recvfrom,sendto
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(MULTICASTIP);
	serveraddr.sin_port = htons(REMOTEPORT);

	//데이터 통신에 사용할 변수

	char buf[BUFSIZE + 1];
	int len;

	//브로드 캐스트 데이터 보내기 

	while (1) {
		//사용자로부터 입력받기 
		printf("\n 보낼 데이터 :");
		if (fgets(buf, BUFSIZE + 1, stdin) == NULL) {
			break;
		}
		//줄바꿈이 들어간 경우 엔터를 제거
		len = strlen(buf); //입력한 문자열 길이 
		if (buf[len - 1] == '\n') {
			buf[len - 1] = '\0';
		}
		if (strlen(buf) == 0) {
			break;
		}

		//데이터 보내기 (UDP에서 수정되는 부분)

		retval = sendto(sock, buf, strlen(buf), 0, (SOCKADDR *)&serveraddr, sizeof(serveraddr));

		if (retval == SOCKET_ERROR) {
			err_quit("sendto()");
			
		}

		printf("멀티 %d 바이트를 보냈습니다\n", retval);

	}
	closesocket(sock);

}



// Reciver함수 정의
DWORD WINAPI ReciverThread(LPVOID arg) {
	int retval; //소켓 상태를 체크할 변수


	
	//1단계: 소켓생성 : socket(주소체계,소켓방식,프로토콜)
	SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
	//tcp : SOCK_STRAM , udp : SCOK_DGRAM
	if (sock == INVALID_SOCKET) {
		err_quit("socket()");
	}

	//재사용여부 추가 (소켓옵션)

	BOOL optval = TRUE;
	retval = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *)&optval, sizeof(optval));
	if (retval == SOCKET_ERROR) {
		err_quit("socketopt()");
	}



	//bind

	//2단계 : ip주소, 포트설정 ==> bind(소켓, 주소정보, 주소길이)
	SOCKADDR_IN serveraddr; //p65참조
	ZeroMemory(&serveraddr, sizeof(serveraddr));

	//zeromemory(위치,크기) : 0으로 초기화 
	serveraddr.sin_family = AF_INET;//주소체계(Ipv4)
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);//서버ip주소
	serveraddr.sin_port = htons(LOCALPORT);
	// 포트번호 ==> 호스트 -> 네트워크 방식으로

	retval = bind(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	//bind(소켓, 주소,주소길이)
	if (retval == SOCKET_ERROR) {
		err_quit("bind()");
	}

	//멀티캐스트 그룹 가입
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

}

	int main() {




		//윈속 초기화
		int retval;

		WSADATA wsa;
		if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
			return 1;
		}

		SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
		//tcp : SOCK_STRAM , udp : SCOK_DGRAM
		if (sock == INVALID_SOCKET) {
			err_quit("socket()");
		}

		HANDLE hThread[2];
		hThread[0] = CreateThread(NULL, 0, SenderThread, NULL, 0, NULL);
		hThread[1] = CreateThread(NULL, 0, ReciverThread, NULL, 0, NULL);

		WaitForMultipleObjects(2, hThread, TRUE, INFINITE);
		
		closesocket(sock);
		WSACleanup();
		return 0;



	}





