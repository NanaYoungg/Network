#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <WinSock2.h>
#include <stdlib.h>
#include <stdio.h>

#define SERVERIP "255.255.255.255" //������ε�ĳ��Ʈ �ּ�
#define SERVERPORT 9000 //��Ʈ��ȣ(����)
#define BUFSIZE 512 //������ ������� ũ��
void err_quit(char *msg) {
}
void err_display(char *msg) {
}
//main: ����->����->sndto->recvfrom->(ip�ּ�üũ)->close
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
		err_display("socket()");
	}

	//��ε� ĳ��Ʈ Ȱ��ȭ �ɼ� �߰�
	BOOL bEnable = TRUE;
	retval = setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char *)&bEnable, sizeof(bEnable));
						//��������,�ɼ���ü,�ɼǰ�,�����ּ�,ũ��

	if (sock == SOCKET_ERROR) {
		err_display("setsocket()");
	}

		//���� �ּ� ü�� �ʱ�ȭ ----> recvfrom,sendto
		SOCKADDR_IN serveraddr;
		ZeroMemory(&serveraddr, sizeof(serveraddr));
		serveraddr.sin_family = AF_INET;
		serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
		serveraddr.sin_port = htons(SERVERPORT);

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
				err_display("sendto()");
				continue;
			}

			printf("udpŬ���̾�Ʈ %d ����Ʈ�� ���½��ϴ�\n", retval);

		}
		closesocket(sock);
		WSACleanup();
	
	}
