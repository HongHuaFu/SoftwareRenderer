#pragma once
#include "Maths.h"
#include "Platform.h"


class Rasterize
{
public:
	static void SetPixel(int x,int y,const Color& col);

	static void DrawLineDDA(const vec2i& p0,const vec2i& p1,const Color& col);
};

