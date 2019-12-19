#include "Rasterize.h"





void Rasterize::SetPixel(int x,int y,const Color & col)
{
	Platform::SetPixel(x,y,col);
}

void Rasterize::DrawLineDDA(const vec2i & p0,const vec2i & p1,const Color & col)
{
	int dx = p1.x - p0.x;
	int dy = p1.y-p0.y;
	int steps;
	float ddx,ddy;
	float x= p0.x;
	float y = p0.y;


	fabs(dx)>fabs(dy) ? steps = fabs(dx):steps = fabs(dy);

	ddx =(float) dx /(float)steps;
	ddy =(float) dy / (float)steps;

	SetPixel(round(x),round(y),col);
	for(int k = 0; k<steps; k++)
	{
		x+=ddx;
		y+=ddy;
		SetPixel(round(x),round(y),col);
	}
}
