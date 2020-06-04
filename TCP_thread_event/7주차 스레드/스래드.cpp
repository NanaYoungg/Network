//192p 스레드 생성,종료

#include <Windows.h>
#include <stdio.h>

int sum = 0;

DWORD WINAPI MyThread(LPVOID arg) {

	int num = (int)arg;
	for (int i = 1; i < num; i++) {
		sum += i;


	}
	return 0;

}
int main() {
	int num = 100;
	HANDLE hThread = CreateThread(NULL, 0, MyThread, (LPVOID)num, CREATE_SUSPENDED, NULL); //
	//보안,스택크기,함수.매개변수,바로실행OR일시정지,스레드ID값

	//스레드 생성 실패 오류
		if (hThread == NULL) {
			return 1;
		}

		printf("스레드 실행전 결과 = %d\n", sum);

		ResumeThread(hThread); //스레드 다시 시작
		WaitForSingleObject(hThread, INFINITE);
		printf("id=%d, 스레드 실행 후 결과 =%d\n", GetCurrentThreadId(),sum);  //내가 아이디 쓰지않아도 알아서 아이디해줌 GetCurrentThreadId
		return 0;
}