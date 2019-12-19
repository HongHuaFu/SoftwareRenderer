#pragma once
#include <Windows.h>

struct Color
{
	int r,g,b;
};

class Platform
{
public:
	static bool Init();
	static void Update();
	static void SetPixel(int,int,const Color&);
private:
	static HWND ghMainWnd;
	static HDC ghdcMainWnd;
	static HBITMAP gdibMap;
	static HBITMAP goldMap;
	static unsigned char* fbo;
	static int screen_width;
	static int screen_height;
	static int screen_pitch;
	static int screen_keys[512];
private:
	static bool InitWindowApp(int width,int height,const char* title);
	static LRESULT WndProc(HWND, UINT, WPARAM, LPARAM);	
};

