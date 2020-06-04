#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "ws2_32")
#include <WinSock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <ws2tcpip.h> //add_memebeship -->��Ƽĳ��Ʈ �Լ� 

#define MULTICASTIP "237.10.19.10"  //224~239 ���� �ȿ���
//��ε� : 255.255.255.255
//TCP/UDP : ������(127.0.0.1)

#define REMOTEPORT 9000 
#define LOCALPORT 9000
//��Ʈ��ȣ(����)
#define BUFSIZE 512 //������ ������� ũ��
void err_quit(char *msg) {
}
void err_display(char *msg) {
}
//sender�Լ�����


DWORD WINAPI SenderThread(LPVOID arg) {

	int retval; //���� ���¸� üũ�� ����

				//���� �ʱ�ȭ

	
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
			
		}

		printf("��Ƽ %d ����Ʈ�� ���½��ϴ�\n", retval);

	}
	closesocket(sock);

}



// Reciver�Լ� ����
DWORD WINAPI ReciverThread(LPVOID arg) {
	int retval; //���� ���¸� üũ�� ����


	
	//1�ܰ�: ���ϻ��� : socket(�ּ�ü��,���Ϲ��,��������)
	SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
	//tcp : SOCK_STRAM , udp : SCOK_DGRAM
	if (sock == INVALID_SOCKET) {
		err_quit("socket()");
	}

	//���뿩�� �߰� (���Ͽɼ�)

	BOOL optval = TRUE;
	retval = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *)&optval, sizeof(optval));
	if (retval == SOCKET_ERROR) {
		err_quit("socketopt()");
	}



	//bind

	//2�ܰ� : ip�ּ�, ��Ʈ���� ==> bind(����, �ּ�����, �ּұ���)
	SOCKADDR_IN serveraddr; //p65����
	ZeroMemory(&serveraddr, sizeof(serveraddr));

	//zeromemory(��ġ,ũ��) : 0���� �ʱ�ȭ 
	serveraddr.sin_family = AF_INET;//�ּ�ü��(Ipv4)
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);//����ip�ּ�
	serveraddr.sin_port = htons(LOCALPORT);
	// ��Ʈ��ȣ ==> ȣ��Ʈ -> ��Ʈ��ũ �������

	retval = bind(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	//bind(����, �ּ�,�ּұ���)
	if (retval == SOCKET_ERROR) {
		err_quit("bind()");
	}

	//��Ƽĳ��Ʈ �׷� ����
	struct ip_mreq mreq;
	mreq.imr_multiaddr.s_addr = inet_addr(MULTICASTIP); //�׷�IP
	mreq.imr_interface.s_addr = htonl(INADDR_ANY);//�׷�ȿ� ���� IP
	retval = setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&mreq, sizeof(mreq));
	if (retval == SOCKET_ERROR) {
		err_quit("setsockopt()");
	}


	//������ ��ſ� ����� ���� (���ϻ���x)

	SOCKADDR_IN peeraddr; // ip�� ��Ʈ��ȣ
	int addrlen;
	char buf[BUFSIZE + 1]; //������ ���� ���� ����

	while (1) {
		addrlen = sizeof(peeraddr);
		retval = recvfrom(sock, buf, BUFSIZE, 0, (SOCKADDR *)&peeraddr, &addrlen);
		//��������,�����ּ�,ũ��,����(0),ip�ּ���Ʈ��ȣ,����

		if (retval == SOCKET_ERROR) {
			err_display("recvfrom()");
			continue;   //�������� �޾ƾ��ϴϱ� ��Ƽ��
		}

		//���������� ���
		buf[retval] = '\0';//���� ������ ���� \0�� �߰�
		printf("[��Ƽ%s:%d] %s\n", inet_ntoa(peeraddr.sin_addr),
			ntohs(peeraddr.sin_family), buf);

		//Ż��
		retval == setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&mreq, sizeof(mreq));

		if (retval == SOCKET_ERROR) {
			err_quit("setsockopt()");

		}




	}
	closesocket(sock);

}

	int main() {




		//���� �ʱ�ȭ
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





