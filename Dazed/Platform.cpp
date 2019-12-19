#include "Platform.h"

#pragma region global static values init.

HWND Platform::ghMainWnd = NULL;
HDC Platform::ghdcMainWnd = NULL;
HBITMAP Platform::gdibMap = NULL;
HBITMAP Platform::goldMap = NULL;
unsigned char* Platform::fbo = NULL;


#pragma endregion




bool Platform::InitWindowApp(int width,int height,const char * title)
{
	//1.register window class and create window.
	WNDCLASS wc = { CS_BYTEALIGNCLIENT, 
				   (WNDPROC)WndProc, 0, 0, 0, 
					NULL, NULL, NULL, 
					NULL, "XXXCLX.SDS.SD" };

	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.hInstance = GetModuleHandle(NULL);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);

	if (!RegisterClass(&wc)) 
		return false;

	ghMainWnd = CreateWindow("XXXCLX.SDS.SD",title,
					WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
					0, 0, 0, 0, NULL, NULL, GetModuleHandle(NULL), NULL);

	if(ghMainWnd == NULL)
		return false;

	//2.get window hdc.
	HDC tmpHdc = GetDC(ghMainWnd);
	ghdcMainWnd = CreateCompatibleDC(tmpHdc);
	ReleaseDC(ghMainWnd, tmpHdc);

	//3.dibMap create.
	LPVOID ptr;
	BITMAPINFO bi = { { sizeof(BITMAPINFOHEADER), 
						width, -height, 1, 32, BI_RGB, 
						width * height * 4, 0, 0, 0, 0 }  };
	gdibMap = CreateDIBSection(ghdcMainWnd, &bi, DIB_RGB_COLORS, &ptr, 0, 0);
	if(gdibMap == NULL)
		return false;

	goldMap = (HBITMAP)SelectObject(ghdcMainWnd, gdibMap);

	//4.fbo bind and set screen parameter.
	fbo = (unsigned char*)ptr;
	memset(fbo, 0, width * height * 4);//TODO: remember to release.
	memset(screen_keys, 0, sizeof(int) * 512);
	screen_width = width;
	screen_height = height;
	screen_pitch = 4;

	//5.adjust window.
	RECT rect = { 0, 0, width, height };
	AdjustWindowRect(&rect, GetWindowLong(ghMainWnd, GWL_STYLE), 0);
	int wx = rect.right - rect.left;
	int wy = rect.bottom - rect.top;
	int sx = (GetSystemMetrics(SM_CXSCREEN) - wx) / 2;
	int sy = (GetSystemMetrics(SM_CYSCREEN) - wy) / 2;
	if (sy < 0) sy = 0;

	SetWindowPos(ghMainWnd, NULL, sx, sy, wx, wy, 
			    (SWP_NOCOPYBITS | SWP_NOZORDER | SWP_SHOWWINDOW));
	SetForegroundWindow(ghMainWnd);
	ShowWindow(ghMainWnd, SW_NORMAL);

	return true;
}
