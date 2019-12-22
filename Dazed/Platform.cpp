#include "Platform.hpp"

HWND ghMainWnd		= NULL;				
HDC ghdcMainWnd     = NULL;			
HBITMAP gdibMap		= NULL;		
HBITMAP goldMap		= NULL;	
unsigned char* gFbo  = nullptr;
int gInputKeys[512] = {0};	

void Platform::MyRegisterClass()
{
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetModuleHandle(NULL);
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = "WinDOWCLASSNAME";
	RegisterClass(&wc);
}

BOOL Platform::InitInstance(int width,int height,const char* title)
{
	ghMainWnd = CreateWindow("WinDOWCLASSNAME", title, 
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		0, 0, 0, 0, NULL, NULL, GetModuleHandle(NULL), NULL);

	if (ghMainWnd == NULL)
	{
		return FALSE;
	}


	HDC tmpHdc = GetDC(ghMainWnd);
	ghdcMainWnd = CreateCompatibleDC(tmpHdc);
	ReleaseDC(ghMainWnd, tmpHdc);

	// 创建DIB
	LPVOID ptr;
	BITMAPINFO bi = { { sizeof(BITMAPINFOHEADER), 
		width, -height, 1, 32, BI_RGB, 
		width * height * 4, 0, 0, 0, 0 }  };
	gdibMap = CreateDIBSection(ghdcMainWnd, &bi, DIB_RGB_COLORS, &ptr, 0, 0);
	if(gdibMap == NULL)
		return false;
	goldMap = (HBITMAP)SelectObject(ghdcMainWnd, gdibMap);

	// 创建并绑定帧缓存
	gFbo = (unsigned char*)ptr;
	memset(gFbo, 0, width * height * 4);
	memset(gInputKeys, 0, sizeof(int) * 512);

	// 调整窗口大小和位置
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

	ShowWindow(ghMainWnd, SW_SHOW);
	UpdateWindow(ghMainWnd);


	return TRUE;
}

void Platform::UpdateBuffer(int& gWidth,int& gHeight)
{
	HDC hDC = GetDC(ghMainWnd);
	BitBlt(hDC, 0, 0, gWidth, gHeight, ghdcMainWnd, 0, 0, SRCCOPY);
	ReleaseDC(ghMainWnd, hDC);
}

LRESULT Platform::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		// 按键按下
	case WM_KEYDOWN:
	{
		//设置为按下状态
		gInputKeys[wParam & 511] = 1;
		// 退出程序
		if(wParam==VK_ESCAPE)
		{
			DestroyWindow(ghMainWnd);
		}
	}
	break;

	case WM_KEYUP:
	{
		//设置为松开状态
		gInputKeys[wParam & 511] = 0;
	}
	break;
	// 处理退出
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

		// 默认
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
