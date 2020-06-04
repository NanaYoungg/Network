//p223
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <stdlib.h>
#include <stdio.h>

#define SERVERPORT 9000 //서버포트 번호
#define BUFSIZE 512 //데이터저장공간 크기

//소켓 오류함수 준비
void err_quit(char *msg) {
}
void err_display(char *msg) {
}

//main --> 윈속-> 소켓 -> bind -> recvfrom -> sendto -> close
int main() {
	int retval; //소켓 상태를 체크할 변수

				//윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		return 1;
	}
		//1단계: 소켓생성 : socket(주소체계,소켓방식,프로토콜)
		SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
		//tcp : SOCK_STRAM , udp : SCOK_DGRAM
		if (sock == INVALID_SOCKET) {
			err_display("socket()");
		}


		//bind

		//2단계 : ip주소, 포트설정 ==> bind(소켓, 주소정보, 주소길이)
		SOCKADDR_IN serveraddr; //p65참조
		ZeroMemory(&serveraddr, sizeof(serveraddr));

		//zeromemory(위치,크기) : 0으로 초기화 
		serveraddr.sin_family = AF_INET;//주소체계(Ipv4)
		serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);//서버ip주소
		serveraddr.sin_port = htons(SERVERPORT);
		// 포트번호 ==> 호스트 -> 네트워크 방식으로

		retval = bind(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
		//bind(소켓, 주소,주소길이)
		if (retval == SOCKET_ERROR) {
			err_quit("bind()");
		}

		//데이터 통신에 사용할 변수 (소켓생성x)

		SOCKADDR_IN clientaddr; //클라이언트주소정보
		int addrlen;
		char buf[BUFSIZE + 1]; //데이터 저장 공간 선언

		while (1) {
			addrlen = sizeof(clientaddr);
			retval = recvfrom(sock, buf, BUFSIZE, 0, (SOCKADDR *)&clientaddr, &addrlen);
								//소켓정보,버퍼주소,크기,상태(0),ip주소포트번호,길이
		
			if (sock == SOCKET_ERROR) {
				err_display("recvfrom()");
				continue;   //여러명꺼를 받아야하니까 컨티뉴
			}

			//받은데이터 출력
			buf[retval] = '\0';//실제 데이터 끝에 \0을 추가
			printf("[udp%s:%d] : %s\n", inet_ntoa(clientaddr.sin_addr),
										ntohs(clientaddr.sin_family),buf);

		//데이터 보내기
			retval == sendto(sock, buf, retval, 0, (SOCKADDR *)&clientaddr, sizeof(clientaddr));
			if (sock == SOCKET_ERROR) {
				err_display("sendto()"); 
				continue;
			}
			
	}

		//소켓마무리
		closesocket(sock);
		WSACleanup(); 

}
