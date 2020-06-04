#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "ws2_32")
#include <WinSock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <ws2tcpip.h> //add_memebeship -->��Ƽĳ��Ʈ �Լ� 

#define MULTICASTIP "235.7.8.9"  //224~239 ���� �ȿ���
//��ε� : 255.255.255.255
//TCP/UDP : ������(127.0.0.1)

#define REMOTEPORT 9000 //��Ʈ��ȣ(����)
#define BUFSIZE 512 //������ ������� ũ��
void err_quit(char *msg) {
}
void err_display(char *msg) {
}
//main: main->����->TTL-> sendto 

int main() {
	int retval; //���� ���¸� üũ�� ����

				//���� �ʱ�ȭ

	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		return 1;
	}
	//����
	SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
	//tcp : SOCK_STRAM , udp : SCOK_DGRAM
	if (sock == INVALID_SOCKET) {
		err_quit("socket()");
	}

	//��ε� ĳ��Ʈ �� �ٸ��κ� ����  --> ��Ƽĳ��Ʈ TTL����

	int ttl = 2;

	retval = setsockopt(sock, IPPROTO_IP, IP_MULTICAST_TTL, (char *)&ttl, sizeof(ttl));
	//����,�ɼ���üip,�ɼ��̸�ttl,�����ּ�,ũ��

	if (retval == SOCKET_ERROR) {
		err_quit("sendto()");
	}

		//���� �ּ� ü�� �ʱ�ȭ ----> recvfrom,sendto
		SOCKADDR_IN serveraddr;
		ZeroMemory(&serveraddr, sizeof(serveraddr));
		serveraddr.sin_family = AF_INET;
		serveraddr.sin_addr.s_addr = inet_addr(MULTICASTIP);
		serveraddr.sin_port = htons(REMOTEPORT);

	//������ ��ſ� ����� ����

		char buf[BUFSIZE + 1];
		int len;

	//��ε� ĳ��Ʈ ������ ������ 

		while (1) {
			//����ڷκ��� �Է¹ޱ� 
			printf("\n ���� ������ :");
			if (fgets(buf, BUFSIZE + 1, stdin) == NULL) {
				break;
			}
			//�ٹٲ��� �� ��� ���͸� ����
			len = strlen(buf); //�Է��� ���ڿ� ���� 
			if (buf[len - 1] == '\n') {
				buf[len - 1] = '\0';
			}
			if (strlen(buf) == 0) {
				break;
			}

			//������ ������ (UDP���� �����Ǵ� �κ�)

			retval = sendto(sock, buf, strlen(buf), 0, (SOCKADDR *)&serveraddr, sizeof(serveraddr));

			if (retval == SOCKET_ERROR) {
				err_quit("sendto()");
				continue;
			}

			printf("��Ƽ %d ����Ʈ�� ���½��ϴ�\n", retval);

		}
		closesocket(sock);
		WSACleanup();
	
	}
