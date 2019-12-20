#include "Renderer.h"

int main()
{
	Renderer RT;
	RT.Init(480, 480,
		"Dazed",
		"a.obj",
		"a.tga");
	RT.Loop();
	RT.Quit();
}