//207 �Ӱ迵�� ���� �ϴ� ��
#include <Windows.h>
#include <stdio.h>
#define MAXCNT 1000000
int g_count = 0;
CRITICAL_SECTION cs; //�Ӱ迵���� üũ�� ����

DWORD WINAPI MyThread1(LPVOID arg) {
	for (int i = 0; i < MAXCNT;i++) {
		EnterCriticalSection(&cs);
		g_count += 2; //�����ڿ� ����
		LeaveCriticalSection(&cs);

	}
	return 0;
}
	DWORD WINAPI MyThread2(LPVOID arg) {
		for (int i = 0; i < MAXCNT;i++) {
			EnterCriticalSection(&cs);
			g_count -= 2; //�����ڿ� ����
			LeaveCriticalSection(&cs);

		}
		return 0;
	}
	int main() {
		//�Ӱ迵�� �ʱ�ȭ
		InitializeCriticalSection(&cs);
		HANDLE hTread[2];
		hTread[0] = CreateThread(NULL, 0, MyThread1, NULL, 0, NULL);
		hTread[1] = CreateThread(NULL, 0, MyThread2, NULL, 0, NULL);
		WaitForMultipleObjects(2, hTread, TRUE, INFINITE);
		
		printf("g_count=%d\n", g_count);
		return 0;

		//����� 0


	}