#pragma once
#include <Windows.h>

struct Color
{
	int r,g,b;
	Color(int rr,int gg,int bb):r(rr),b(bb),g(gg){ }
	Color(){ }
};

class Platform
{
public:
	static bool InitWindowApp(int width,int height,const char* title);
	static void Update();
	static void SetPixel(int,int,const Color&);
	static bool screen_exit;
	static int screen_keys[512];
	static int screen_width;
	static int screen_height;

private:
	static HWND ghMainWnd;
	static HDC ghdcMainWnd;
	static HBITMAP gdibMap;
	static HBITMAP goldMap;
	static unsigned char* fbo;
	
	static int screen_pitch;
	
	
private:
	
	static LRESULT WndProc(HWND, UINT, WPARAM, LPARAM);	
};

