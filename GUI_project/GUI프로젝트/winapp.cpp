#define _CRT_SECURE_NO_WARNINGS


#include <Windows.h>  //win32API 사용
#include <stdio.h> 
//함수 원형 정해놓기
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void DisplayText(char *fmt, ...);

HINSTANCE hlnst; //인스턴스핸들
HWND hEdit; //편집 컨트롤 


//main  -> WinMain

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hPrevinstance,
	LPSTR ipCmdLine, int nCmdShow)
{

	hlnst = hinstance;

	//윈도우 클래스 등록
	WNDCLASS wndclass;
	wndclass.style = CS_HREDRAW | CS_VREDRAW; //창의 스타일(수평,수직)
	wndclass.lpfnWndProc = WndProc; //메세지 처리 함수
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0; //0이면 사용안함
	wndclass.hInstance = hinstance; //윈도우 메인 설정된 핸들값
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);//아이콘
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);//커서
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); //배경
	wndclass.lpszMenuName = NULL; //메뉴사용->리소스연결
	wndclass.lpszClassName = "MyWdClass"; //창생성시 사용하는 이름
	//오류메세지각자

	//윈도우 생성
	HWND hWnd = CreateWindow("MyWnClass", "제목 APP", WS_OVERLAPPEDWINDOW,
		0, 0, 600, 200, NULL, NULL, hinstance, NULL); //가로600세로200

//오류메세지


//윈도우 보여주기
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);


	//메세지 루프
	MSG msg;
	while (GetMessage(&msg, 0, 0, 0) > 0)
	{
		//메세지읽기
		TranslateMessage(&msg);				//메세지 가공(정리)
		DispatchMessage(&msg);		//윈도우 프로시저에 전달
	}
	return msg.wParam;
}
	//윈도우 프로시저 -> 메세지 처리함수
	LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lparam)
	{
		switch (uMsg)
		{
		case WM_CREATE:
			hEdit = CreateWindow("edeit", NULL, WS_CHILD, 0, 0, 0, 0, hWnd, (HMENU)100, hlnst, NULL);
			DisplayText("간단한 GUI \r\n");
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
	