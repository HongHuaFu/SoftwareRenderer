#include "Rasterize.h"

int main()
{
	Platform::InitWindowApp(480,480,"hello world");
	while(1)
	{
		Platform::SetPixel(240,240,Color(255,20,45));
		Rasterize::DrawLineDDA(vec2i(50,50),vec2i(400,450),Color(255,20,45));
		Rasterize::DrawLine(vec2i(50,50),vec2i(10,450),Color(255,20,45));
		Platform::Update();
		MSG msg;
		while (1) {
			if (!PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) break;
			if (!GetMessage(&msg, NULL, 0, 0)) break;
			DispatchMessage(&msg);
		}
	}
}