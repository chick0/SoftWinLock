// 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
#define WIN32_LEAN_AND_MEAN

// Windows 헤더 파일
#include <SDKDDKVer.h>
#include <windows.h>

HINSTANCE g_hInst;
LPCTSTR lpszClass = TEXT("SoftWinLock");

HWND g_hWnd;
HHOOK hHook;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT WINAPI KeyboardHookProc(INT, WPARAM, LPARAM);

int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpszCmdParam, _In_ int nCmdShow)
{
	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;
	g_hInst = hInstance;
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hInstance = hInstance;
	WndClass.lpfnWndProc = WndProc;
	WndClass.lpszClassName = lpszClass;
	WndClass.lpszMenuName = NULL;
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&WndClass);

	hWnd = CreateWindow(
		lpszClass, 
		lpszClass, 
		WS_OVERLAPPED | WS_SYSMENU,
		// 윈도우 생성 위치
		CW_USEDEFAULT, CW_USEDEFAULT,
		// 윈도우 크기
		150, 150,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	ShowWindow(hWnd, nCmdShow);
	g_hWnd = hWnd;

	while (GetMessage(&Message, NULL, 0, 0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}

	return (int)Message.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	switch (iMessage) {
	case WM_CREATE:
		hHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardHookProc, g_hInst, NULL);
		break;

	case WM_DESTROY:
		UnhookWindowsHookEx(hHook);
		PostQuitMessage(0);
		return 0;
	}

	return (DefWindowProc(hWnd, iMessage, wParam, lParam));
}

struct KeyboardEvent {
	INT vkCode;
	INT scanCode;
	INT flags;
	INT time;
	INT dwExtraInfo;
};

LRESULT WINAPI KeyboardHookProc(INT nCode, WPARAM wParam, LPARAM lParam) {
	if (nCode >= 0) {
		struct KeyboardEvent *e = *(KeyboardEvent**)&lParam;

		switch (wParam)
		{
		case WM_KEYDOWN:
		case WM_KEYUP:
			if (e->vkCode == VK_LWIN || e->vkCode == VK_RWIN) {
				return 1;
			}
			break;
		}
	}

	return CallNextHookEx(hHook, nCode, wParam, lParam);
}