#include <WinSock2.h>
#include <stdio.h>
#include <ws2bth.h> //������� ����

int main() 
{
	int retval;
	//���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		return 1;
	}
	//��ġ  �˻�  -->ī������


	// ������� ���� �˻� �غ�
	DWORD qslen = sizeof(WSAQUERYSET); //������� ������ġ �� ���񽺿����� ����
	WSAQUERYSET *qs = (WSAQUERYSET *)malloc(qslen); 
	ZeroMemory(qs, qslen);
	qs->dwSize = qslen;		//����ü ��ü ũŰ
	qs->dwNameSpace = NS_BTH; //�ʼ�!! �������� -->�������
	//qs->lpServiceClassId = (GUID *)&BthServer_Service;
	//qs->lpszContext = SERVERADDR;
	DWORD flags = LUP_CONTAINERS | LUP_FLUSHCACHE | LUP_RETURN_ADDR;

	// ������� ���� �˻� ����
	HANDLE hLookup;
	retval = WSALookupServiceBegin(qs, flags, &hLookup); //����ü,��������,�ڵ鰪
	if (retval == SOCKET_ERROR) {
		printf("[����] �߰ߵ� ������� ��ġ ����!\n");
		exit(1);
	}

	// �˻��� ������� ���� ���� Ȯ��
	SOCKADDR_BTH *sa = NULL;
	int serverport = 0;
	bool done = false;
	while (!done) {
		retval = WSALookupServiceNext(hLookup, flags, &qslen, qs);
		if (retval == NO_ERROR) {
			// ������� ��ġ ������ ��� �ִ� ���� �ּ� ����ü�� ����
			sa = (SOCKADDR_BTH *)qs->lpcsaBuffer->RemoteAddr.lpSockaddr;
			// ���� ��Ʈ ��ȣ ���� �� Ż��
			//serverport = sa->port;
			char addr[40] = { 0 };
			DWORD addrlen = sizeof(addr);
			WSAAddressToString((SOCKADDR *)sa, sizeof(SOCKADDR_BTH), NULL, addr, &addrlen);
			printf("������� ��ġ �߰� %s -%s\n",
						addr, qs->lpszServiceInstanceName);
		}
		else {
			if (WSAGetLastError() == WSAEFAULT) {
				free(qs);
				qs = (WSAQUERYSET *)malloc(qslen);
			}
			else {
				done = true;
			}
		}
	}

	// ������� ���� �˻� ����
	WSALookupServiceEnd(hLookup);
	free(qs);
	WSACleanup();






}