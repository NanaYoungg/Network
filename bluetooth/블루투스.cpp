#include <WinSock2.h>
#include <stdio.h>
#include <ws2bth.h> //블루투스 지원

int main() 
{
	int retval;
	//윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		return 1;
	}
	//장치  검색  -->카페참조


	// 블루투스 서비스 검색 준비
	DWORD qslen = sizeof(WSAQUERYSET); //블루투스 연결장치 및 서비스에대한 정보
	WSAQUERYSET *qs = (WSAQUERYSET *)malloc(qslen); 
	ZeroMemory(qs, qslen);
	qs->dwSize = qslen;		//구조체 자체 크키
	qs->dwNameSpace = NS_BTH; //필수!! 서비스정보 -->블루투스
	//qs->lpServiceClassId = (GUID *)&BthServer_Service;
	//qs->lpszContext = SERVERADDR;
	DWORD flags = LUP_CONTAINERS | LUP_FLUSHCACHE | LUP_RETURN_ADDR;

	// 블루투스 서비스 검색 시작
	HANDLE hLookup;
	retval = WSALookupServiceBegin(qs, flags, &hLookup); //구조체,상태정보,핸들값
	if (retval == SOCKET_ERROR) {
		printf("[오류] 발견된 블루투스 장치 없음!\n");
		exit(1);
	}

	// 검색된 블루투스 서비스 정보 확인
	SOCKADDR_BTH *sa = NULL;
	int serverport = 0;
	bool done = false;
	while (!done) {
		retval = WSALookupServiceNext(hLookup, flags, &qslen, qs);
		if (retval == NO_ERROR) {
			// 블루투스 장치 정보를 담고 있는 소켓 주소 구조체에 접근
			sa = (SOCKADDR_BTH *)qs->lpcsaBuffer->RemoteAddr.lpSockaddr;
			// 서버 포트 번호 저장 후 탈출
			//serverport = sa->port;
			char addr[40] = { 0 };
			DWORD addrlen = sizeof(addr);
			WSAAddressToString((SOCKADDR *)sa, sizeof(SOCKADDR_BTH), NULL, addr, &addrlen);
			printf("블루투스 장치 발견 %s -%s\n",
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

	// 블루투스 서비스 검색 종료
	WSALookupServiceEnd(hLookup);
	free(qs);
	WSACleanup();






}