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

void Rasterize::DrawLine(const vec2i & p0,const vec2i & p1,const Color & col)
{
	int x0 = p0.x; 
	int x1 = p1.x;
	int y0 = p0.y; 
	int y1 = p1.y;
	bool steep = false;
	if (std::abs( x0 - x1 ) < std::abs( y0 - y1 ))
	{
		std::swap( x0 , y0 );
		std::swap( x1 , y1 );
		steep = true;
	}

	if (x0 > x1)
	{
		std::swap( x0 , x1 );
		std::swap( y0 , y1 );
	}


	int dx = x1 - x0;
	int dy = y1 - y0;


	int dy2 = std::abs( dy ) * 2;
	int d = 0;
	int y = y0;


	for (int x = x0; x <= x1; x++)
	{
		if (steep)
		{
			SetPixel( y , x , col );
		}
		else
		{
			SetPixel( x , y , col );
		}

		d += dy2;
		if (d > dx)
		{
			y += ( y1 > y0 ? 1 : -1 );
			d -= dx * 2;
		}
	}
}

