//207 임계영역 설정 하는 예
#include <Windows.h>
#include <stdio.h>
#define MAXCNT 1000000
int g_count = 0;
CRITICAL_SECTION cs; //임계영역을 체크할 변수

DWORD WINAPI MyThread1(LPVOID arg) {
	for (int i = 0; i < MAXCNT;i++) {
		EnterCriticalSection(&cs);
		g_count += 2; //공유자원 접근
		LeaveCriticalSection(&cs);

	}
	return 0;
}
	DWORD WINAPI MyThread2(LPVOID arg) {
		for (int i = 0; i < MAXCNT;i++) {
			EnterCriticalSection(&cs);
			g_count -= 2; //공유자원 접근
			LeaveCriticalSection(&cs);

		}
		return 0;
	}
	int main() {
		//임계영역 초기화
		InitializeCriticalSection(&cs);
		HANDLE hTread[2];
		hTread[0] = CreateThread(NULL, 0, MyThread1, NULL, 0, NULL);
		hTread[1] = CreateThread(NULL, 0, MyThread2, NULL, 0, NULL);
		WaitForMultipleObjects(2, hTread, TRUE, INFINITE);
		
		printf("g_count=%d\n", g_count);
		return 0;

		//결과값 0


	}