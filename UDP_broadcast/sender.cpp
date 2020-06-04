#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <WinSock2.h>
#include <stdlib.h>
#include <stdio.h>

#define SERVERIP "255.255.255.255" //지역브로드캐스트 주소
#define SERVERPORT 9000 //포트번호(서버)
#define BUFSIZE 512 //데이터 저장공간 크기
void err_quit(char *msg) {
}
void err_display(char *msg) {
}
//main: 윈속->소켓->sndto->recvfrom->(ip주소체크)->close
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
		err_display("socket()");
	}

	//브로드 캐스트 활성화 옵션 추가
	BOOL bEnable = TRUE;
	retval = setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char *)&bEnable, sizeof(bEnable));
						//소켓정보,옵션주체,옵션값,버퍼주소,크기

	if (sock == SOCKET_ERROR) {
		err_display("setsocket()");
	}

		//소켓 주소 체계 초기화 ----> recvfrom,sendto
		SOCKADDR_IN serveraddr;
		ZeroMemory(&serveraddr, sizeof(serveraddr));
		serveraddr.sin_family = AF_INET;
		serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
		serveraddr.sin_port = htons(SERVERPORT);

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
				err_display("sendto()");
				continue;
			}

			printf("udp클라이언트 %d 바이트를 보냈습니다\n", retval);

		}
		closesocket(sock);
		WSACleanup();
	
	}
