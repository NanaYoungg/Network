//192p ������ ����,����

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
	//����,����ũ��,�Լ�.�Ű�����,�ٷν���OR�Ͻ�����,������ID��

	//������ ���� ���� ����
		if (hThread == NULL) {
			return 1;
		}

		printf("������ ������ ��� = %d\n", sum);

		ResumeThread(hThread); //������ �ٽ� ����
		WaitForSingleObject(hThread, INFINITE);
		printf("id=%d, ������ ���� �� ��� =%d\n", GetCurrentThreadId(),sum);  //���� ���̵� �����ʾƵ� �˾Ƽ� ���̵����� GetCurrentThreadId
		return 0;
}