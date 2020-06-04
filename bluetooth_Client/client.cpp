#pragma comment(lib, "ws2_32")
#include <WinSock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <ws2bth.h> //�������
#include <initGuid.h> //GUID -->�ĺ���


#define SERVERADDR "50:77:05:77:83:00 " //������� ��ġ �˻��Ҷ��� ������ �ּ�
#define BUFSIZE 512

//������� ��Ʈ��ȣ : BT_PORT_ANY : �ڵ��Ҵ�(1~30)

//GUID�Ҵ� -> ����->GUID����� ->���� 

DEFINE_GUID(BthServer_Service,
	0x99b580c6, 0x4acc, 0x4888, 0xb4, 0x61, 0x22, 0xe4, 0x6d, 0x53, 0xba, 0x5f);

void err_quit(char *msg) {
	//�������� ����
}
void err_display(char *msg) {
	//�������� �޽��� ���
}
int main()
{
	int retval;

	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		return 1;
	}

	//��ġ�˻� -> ī������ (p555 49-101���� ����) ����.


	//���ϻ���
	SOCKET sock = socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);
	if (sock == INVALID_SOCKET) {
		err_quit("socket()");
	}

	//CONNECT
	int serverport = 0;

	SOCKADDR_BTH serveraddr; //p65����
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	int addrlen = sizeof(serveraddr);
	WSAStringToAddress(SERVERADDR, AF_BTH, NULL, (SOCKADDR *)&serveraddr, &addrlen);
	serveraddr.port = BT_PORT_ANY; //�ڵ��Ҵ�(1-30)

	retval = connect(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));

	if (retval == SOCKET_ERROR) {
		err_quit("connect()");
	}

	char buf[BUFSIZE + 1];
	int len;


	while (1) {
		//����ڷκ��� �Է¹ޱ� 
		printf("\n[���� ������] :");
		if (fgets(buf, BUFSIZE + 1, stdin) == NULL) {
			break;
		}
		
		len = strlen(buf); 
		if (buf[len - 1] == '\n') {
			buf[len - 1] = '\0';
		}
		if (strlen(buf) == 0) {
			break;
		}
		
		retval = send(sock, buf, strlen(buf), 0);
		if (retval == SOCKET_ERROR) {
			err_display("send()");
			break;
		}

		printf("������� %d ����Ʈ�� ���½��ϴ�\n", retval);
	}
	closesocket(sock);
	WSACleanup();
}

