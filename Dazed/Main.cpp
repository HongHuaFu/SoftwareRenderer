#include "Renderer.hpp"

int main()
{
	Renderer RTX;
	RTX.Init();
	RTX.Loop();
	RTX.Destroy();
}