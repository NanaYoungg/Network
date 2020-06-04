#define _CRT_SECURE_NO_WARNINGS


#include <Windows.h>  //win32API ���
#include <stdio.h> 
//�Լ� ���� ���س���
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void DisplayText(char *fmt, ...);

HINSTANCE hlnst; //�ν��Ͻ��ڵ�
HWND hEdit; //���� ��Ʈ�� 


//main  -> WinMain

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hPrevinstance,
	LPSTR ipCmdLine, int nCmdShow)
{

	hlnst = hinstance;

	//������ Ŭ���� ���
	WNDCLASS wndclass;
	wndclass.style = CS_HREDRAW | CS_VREDRAW; //â�� ��Ÿ��(����,����)
	wndclass.lpfnWndProc = WndProc; //�޼��� ó�� �Լ�
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0; //0�̸� ������
	wndclass.hInstance = hinstance; //������ ���� ������ �ڵ鰪
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);//������
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);//Ŀ��
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); //���
	wndclass.lpszMenuName = NULL; //�޴����->���ҽ�����
	wndclass.lpszClassName = "MyWdClass"; //â������ ����ϴ� �̸�
	//�����޼�������

	//������ ����
	HWND hWnd = CreateWindow("MyWnClass", "���� APP", WS_OVERLAPPEDWINDOW,
		0, 0, 600, 200, NULL, NULL, hinstance, NULL); //����600����200

//�����޼���


//������ �����ֱ�
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);


	//�޼��� ����
	MSG msg;
	while (GetMessage(&msg, 0, 0, 0) > 0)
	{
		//�޼����б�
		TranslateMessage(&msg);				//�޼��� ����(����)
		DispatchMessage(&msg);		//������ ���ν����� ����
	}
	return msg.wParam;
}
	//������ ���ν��� -> �޼��� ó���Լ�
	LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lparam)
	{
		switch (uMsg)
		{
		case WM_CREATE:
			hEdit = CreateWindow("edeit", NULL, WS_CHILD, 0, 0, 0, 0, hWnd, (HMENU)100, hlnst, NULL);
			DisplayText("������ GUI \r\n");
			return 0;

		case WM_SIZE:
		case WM_SETFOCUS:
		case WM_DESTROY:

		}
	return DefWindowProc(hWnd, uMsg, wParam, lparam);


}
	

	void DisplayText(char *fmt, ...) 
	{
		va_list arg;
		va_start(arg, fmt);
		char cbuf[256];
		vsprintf(cbuf, fmt, arg);
		int nLength = GetWindowTextLength(hEdit);
		SendMessage(hEdit, EM_SETSEL, nLength, nLength);
		SendMessage(hEdit, EM_REPLACESEL, FALSE, (LPARAM)cbuf);
		va_end(arg);
	}
	