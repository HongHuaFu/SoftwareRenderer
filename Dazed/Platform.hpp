#pragma once
#include <Windows.h>


extern HWND ghMainWnd;	
extern HDC ghdcMainWnd;						
extern HBITMAP gdibMap;						
extern HBITMAP goldMap;		
extern int gInputKeys[512];
extern unsigned char* gFbo;

namespace Platform
{
	void MyRegisterClass();
	BOOL InitInstance(int,int,const char*);
	void UpdateBuffer(int&,int&);
	static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
};
