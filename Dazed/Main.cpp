#include "Platform.h"

int main()
{
	Platform::InitWindowApp(480,480,"hello world");
	while(1)
	{
		Platform::SetPixel(240,240,Color(255,0,0));
		Platform::Update();
		MSG msg;
		while (1) {
			if (!PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) break;
			if (!GetMessage(&msg, NULL, 0, 0)) break;
			DispatchMessage(&msg);
		}
	}
}