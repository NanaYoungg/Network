//210p 이벤트 신호를 이용해서 동기화

#include <Windows.h>
#include <stdio.h>
#define BUFSIZE 10
HANDLE hReadEvent;
HANDLE hWriteEvent;

int buf[BUFSIZE];

DWORD WINAPI WirteThread(LPVOID arg) {
	DWORD retval;
	for (int k = 1; k <= 500; k++) {
		retval = WaitForSingleObject(hReadEvent, INFINITE);  //다음대기화면	

		for (int i = 0; i < BUFSIZE;i++) {
			buf[i] = k;   
		}
		SetEvent(hWriteEvent); //신호번경
	}
	return 0;
}

DWORD WINAPI ReadThread(LPVOID arg) {

	DWORD retval;
	while (1) {
		retval = WaitForSingleObject(hWriteEvent, INFINITE); //다음대기화면
		//읽을 데이터 출력
		printf("스레드 %4d:", GetCurrentThreadId());
		for (int i = 0; i < BUFSIZE;i++) {
			printf("%3d", buf[i]);
		}
		printf("\n");

		ZeroMemory(buf, sizeof(buf));
		//신호번경
		SetEvent(hReadEvent);

	}
	return 0;
}

int main()
{	//이벤트 생성
	hWriteEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	hReadEvent = CreateEvent(NULL, FALSE, TRUE, NULL);

	//스레드 생성
	HANDLE hThread[3];
	hThread[0] = CreateThread(NULL, 0, WirteThread, NULL, 0, NULL);
	hThread[1] = CreateThread(NULL, 0, ReadThread, NULL, 0, NULL);
	hThread[2] = CreateThread(NULL, 0, ReadThread, NULL, 0, NULL);
	//생성은 3개지만 2개 반복중

	WaitForMultipleObjects(3, hThread, TRUE, INFINITE);
	CloseHandle(hWriteEvent);
	CloseHandle(hReadEvent);
	return 0;

}
