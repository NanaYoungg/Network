#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "ws2_32")
#include <WinSock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <ws2tcpip.h> //add_memebeship -->멀티캐스트 함수 

#define MULTICASTIP "235.7.8.9"  //224~239 범위 안에서
//브로드 : 255.255.255.255
//TCP/UDP : 루프백(127.0.0.1)

#define REMOTEPORT 9000 //포트번호(서버)
#define BUFSIZE 512 //데이터 저장공간 크기
void err_quit(char *msg) {
}
void err_display(char *msg) {
}
//main: main->소켓->TTL-> sendto 

int main() {
	int retval; //소켓 상태를 체크할 변수

				//윈속 초기화

	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		return 1;
	}
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
				continue;
			}

			printf("멀티 %d 바이트를 보냈습니다\n", retval);

		}
		closesocket(sock);
		WSACleanup();
	
	}
