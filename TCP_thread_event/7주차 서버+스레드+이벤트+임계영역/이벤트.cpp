//210p �̺�Ʈ ��ȣ�� �̿��ؼ� ����ȭ

#include <Windows.h>
#include <stdio.h>
#define BUFSIZE 10
HANDLE hReadEvent;
HANDLE hWriteEvent;

int buf[BUFSIZE];

DWORD WINAPI WirteThread(LPVOID arg) {
	DWORD retval;
	for (int k = 1; k <= 500; k++) {
		retval = WaitForSingleObject(hReadEvent, INFINITE);  //�������ȭ��	

		for (int i = 0; i < BUFSIZE;i++) {
			buf[i] = k;   
		}
		SetEvent(hWriteEvent); //��ȣ����
	}
	return 0;
}

DWORD WINAPI ReadThread(LPVOID arg) {

	DWORD retval;
	while (1) {
		retval = WaitForSingleObject(hWriteEvent, INFINITE); //�������ȭ��
		//���� ������ ���
		printf("������ %4d:", GetCurrentThreadId());
		for (int i = 0; i < BUFSIZE;i++) {
			printf("%3d", buf[i]);
		}
		printf("\n");

		ZeroMemory(buf, sizeof(buf));
		//��ȣ����
		SetEvent(hReadEvent);

	}
	return 0;
}

int main()
{	//�̺�Ʈ ����
	hWriteEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	hReadEvent = CreateEvent(NULL, FALSE, TRUE, NULL);

	//������ ����
	HANDLE hThread[3];
	hThread[0] = CreateThread(NULL, 0, WirteThread, NULL, 0, NULL);
	hThread[1] = CreateThread(NULL, 0, ReadThread, NULL, 0, NULL);
	hThread[2] = CreateThread(NULL, 0, ReadThread, NULL, 0, NULL);
	//������ 3������ 2�� �ݺ���

	WaitForMultipleObjects(3, hThread, TRUE, INFINITE);
	CloseHandle(hWriteEvent);
	CloseHandle(hReadEvent);
	return 0;

}
