//p223
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <stdlib.h>
#include <stdio.h>

#define SERVERPORT 9000 //������Ʈ ��ȣ
#define BUFSIZE 512 //������������� ũ��

//���� �����Լ� �غ�
void err_quit(char *msg) {
}
void err_display(char *msg) {
}

//main --> ����-> ���� -> bind -> recvfrom -> sendto -> close
int main() {
	int retval; //���� ���¸� üũ�� ����

				//���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		return 1;
	}
		//1�ܰ�: ���ϻ��� : socket(�ּ�ü��,���Ϲ��,��������)
		SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
		//tcp : SOCK_STRAM , udp : SCOK_DGRAM
		if (sock == INVALID_SOCKET) {
			err_display("socket()");
		}


		//bind

		//2�ܰ� : ip�ּ�, ��Ʈ���� ==> bind(����, �ּ�����, �ּұ���)
		SOCKADDR_IN serveraddr; //p65����
		ZeroMemory(&serveraddr, sizeof(serveraddr));

		//zeromemory(��ġ,ũ��) : 0���� �ʱ�ȭ 
		serveraddr.sin_family = AF_INET;//�ּ�ü��(Ipv4)
		serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);//����ip�ּ�
		serveraddr.sin_port = htons(SERVERPORT);
		// ��Ʈ��ȣ ==> ȣ��Ʈ -> ��Ʈ��ũ �������

		retval = bind(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
		//bind(����, �ּ�,�ּұ���)
		if (retval == SOCKET_ERROR) {
			err_quit("bind()");
		}

		//������ ��ſ� ����� ���� (���ϻ���x)

		SOCKADDR_IN clientaddr; //Ŭ���̾�Ʈ�ּ�����
		int addrlen;
		char buf[BUFSIZE + 1]; //������ ���� ���� ����

		while (1) {
			addrlen = sizeof(clientaddr);
			retval = recvfrom(sock, buf, BUFSIZE, 0, (SOCKADDR *)&clientaddr, &addrlen);
								//��������,�����ּ�,ũ��,����(0),ip�ּ���Ʈ��ȣ,����
		
			if (sock == SOCKET_ERROR) {
				err_display("recvfrom()");
				continue;   //�������� �޾ƾ��ϴϱ� ��Ƽ��
			}

			//���������� ���
			buf[retval] = '\0';//���� ������ ���� \0�� �߰�
			printf("[udp%s:%d] : %s\n", inet_ntoa(clientaddr.sin_addr),
										ntohs(clientaddr.sin_family),buf);

		//������ ������
			retval == sendto(sock, buf, retval, 0, (SOCKADDR *)&clientaddr, sizeof(clientaddr));
			if (sock == SOCKET_ERROR) {
				err_display("sendto()"); 
				continue;
			}
			
	}

		//���ϸ�����
		closesocket(sock);
		WSACleanup(); 

}
