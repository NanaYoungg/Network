#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <WinSock2.h>
#include <stdlib.h>
#include <stdio.h>

#define SERVERIP "127.0.0.1" //서버 ip주소
#define SERVERPORT 9000 //포트번호(서버)
#define BUFSIZE 512 //데이터 저장공간 크기

//소켓 오류관련 함수
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

	//소켓 주소 체계 초기화 ----> recvfrom,sendto
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	//inet_Addr("문자열 아이피"): 문자열을 숫자 (32비트)
	serveraddr.sin_port = htons(SERVERPORT);
	

	//데이터 통신에 사용되는 변수 p227

	SOCKADDR_IN peeraddr; //받을때 사용되는 IP주소 포트번호
	int addrlen;
	char buf[BUFSIZE + 1];
	int len;

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
		//보낸메세지 출력
		printf("udp클라이언트 %d 바이트를 보냈습니다\n", retval);

		//데이터 받기

		addrlen = sizeof(peeraddr);
		retval = recvfrom(sock, buf, BUFSIZE, 0, (SOCKADDR *)&peeraddr, &addrlen);

		if (retval == SOCKET_ERROR) {
			err_display("recv()");
			continue;
		}
		//송신자 ip주소 같은지 체크

		if (memcmp(&peeraddr, &serveraddr, sizeof(peeraddr))) {
			//memcmp(주소1 주소2 주소바이트수)
			//주소 1 이랑 2 같으면 0 다르면 0이아닌숫자
			printf("오류 주소 잘못");
			continue;
		}

		//받은결과 출력
		buf[retval] = '\0';
		printf("udp클라이언트 %d 바이트를 보냈습니다\n", retval);
		printf("받은데이터 %s\n", buf);

			}

	//소켓마무리
	closesocket(sock);
	WSACleanup();
}