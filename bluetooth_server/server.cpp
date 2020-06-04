#pragma comment(lib, "ws2_32")
#include <WinSock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <ws2bth.h> //�������
#include <initGuid.h> //GUID -->�ĺ���

#define BUFSIZE 512

//������� ��Ʈ��ȣ : BT_PORT_ANY : �ڵ��Ҵ�(1~30)

//GUID�Ҵ� -> ����->GUID����� ->���� 

DEFINE_GUID(BthServer_Service ,
	0x99b580c6, 0x4acc, 0x4888, 0xb4, 0x61, 0x22, 0xe4, 0x6d, 0x53, 0xba, 0x5f);

//��������Լ�

void err_quit(char *msg) {
	//�������� ����
}
void err_display(char *msg) {
	//�������� �޽��� ���
}

int main()  //����(�������)->bind(�������)->listen
{
	int retval;

	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		return 1;
	}

	//bind ->�ּ����� ���� . ���������


	//���ϻ���
	SOCKET listen_sock = socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);
	if (listen_sock == INVALID_SOCKET) {
		err_quit("socket()");
	}


	//2�ܰ� : ip�ּ�, ��Ʈ���� ==> bind(����, �ּ�����, �ּұ���)
	SOCKADDR_BTH serveraddr; //p65����
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.addressFamily = AF_BTH;
	serveraddr.btAddr = 0; //������� ��ġ�ּ�
	serveraddr.port = BT_PORT_ANY; //�ڵ��Ҵ�(1-30)

	retval = bind(listen_sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	//bind(����, �ּ�,�ּұ���)
	if (retval == SOCKET_ERROR) {
		err_quit("bind()");
	}

	//���� ��Ʈ ��ȣ ���(�ڵ��Ҵ��ε� ���������)
	int addrlen = sizeof(serveraddr);
	retval = getsockname(listen_sock, (SOCKADDR *)&serveraddr,&addrlen);
	printf("������� ��Ʈ��ȣ %d\n", serveraddr.port);


	//�������� ���
	CSADDR_INFO addrinfo;
	addrinfo.LocalAddr.lpSockaddr = (SOCKADDR *)&serveraddr;
	addrinfo.LocalAddr.iSockaddrLength = sizeof(serveraddr);

	addrinfo.RemoteAddr.lpSockaddr = (SOCKADDR *)&serveraddr;
	addrinfo.RemoteAddr.iSockaddrLength = sizeof(serveraddr);
	addrinfo.iSocketType = SOCK_STREAM;
	addrinfo.iProtocol = BTHPROTO_RFCOMM;

	//������� ��ġ ����

	WSAQUERYSET qset;
	ZeroMemory(&qset, sizeof(qset));

	qset.dwSize = sizeof(qset);
	qset.dwNameSpace = NS_BTH;
	qset.lpServiceClassId = (GUID*)&BthServer_Service; //gui�̸�
	qset.dwNumberOfCsAddrs = 1;
	qset.lpcsaBuffer = &addrinfo;
	retval = WSASetService(&qset, RNRSERVICE_REGISTER,0); //�������

	//listen

	retval = listen(listen_sock, 1);
	if (retval == SOCKET_ERROR) {
		err_quit("listen()");
	}

	//������ ��� ����� ����

	SOCKET client_sock; 
	SOCKADDR_BTH clientaddr;

	char buf[BUFSIZE + 1];




	while (1) {
		addrlen = sizeof(clientaddr);
		//4�ܰ�
		client_sock = accept(listen_sock,
			(SOCKADDR *)&clientaddr, &addrlen);
		//accept(����,Ŭ���̾�Ʈ�ּ�,����)
		if (client_sock == INVALID_SOCKET) {
			err_display("accept()");
			break;//���ѹݺ� ����
		}

		//������ Ŭ���̾�Ʈ �������
	

		while (1) {
	
			retval = recv(client_sock, buf, BUFSIZE, 0);
			if (retval == SOCKET_ERROR) {
				err_display("recv()");
				break;
			}
			else if (retval == 0) { //���ᰡ �� ���
				break;
			}

			buf[retval] = '\0';
			printf(" ������� ���� %d \n", buf);
			}

		//7�ܰ� : ��������
		closesocket(client_sock);
		printf("Ŭ���̾�Ʈ ����");


		}


	closesocket(listen_sock);
	WSACleanup();


}