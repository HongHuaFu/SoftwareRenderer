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

vec3f Rasterize::Barycentric(vec2f A,vec2f B,vec2f C,vec2f P)
{
	vec3f s[2];
	for (int i=2; i--; ) {
		s[i].raw[0] = C.raw[i]-A.raw[i];
		s[i].raw[1] = B.raw[i]-A.raw[i];
		s[i].raw[2] = A.raw[i]-P.raw[i];
	}
	vec3f u = (s[0]^ s[1]);
	if (std::abs(u.raw[2])>1e-2)
		return vec3f(1.f-(u.x+u.y)/u.z, u.y/u.z, u.x/u.z);
	return vec3f(-1,1,1);
}


#include <cmath>
void Rasterize::Triangle(vec4f pts[3],IShader & shader,float * zbuffer)
{
	vec2f pts_div[3];
	for(int i = 0; i<3; i++)
	{	
		pts_div[i] = vec2f(pts[i].x/pts[i].w,pts[i].y/pts[i].w);
	}

	vec2f bboxmin( 100000.0f, 100000.0f);
	vec2f bboxmax(-100000.0f, -100000.0f);

	
	for (int i=0; i<3; i++) {
		for (int j=0; j<2; j++) {
			bboxmin.raw[j] = min(bboxmin.raw[j], pts[i].raw[j]/pts[i].raw[3]);
			bboxmax.raw[j] = max(bboxmax.raw[j], pts[i].raw[j]/pts[i].raw[3]);
		}
	}

	vec2i P;
	Color color;
	for (P.x=bboxmin.x; P.x<=bboxmax.x; P.x++) {
		for (P.y=bboxmin.y; P.y<=bboxmax.y; P.y++) {
			vec2f tmp = vec2f(P.x,P.y);
			vec3f c = Barycentric(pts_div[0],pts_div[1],pts_div[2],tmp);

			float z = pts[0].raw[2]*c.x + pts[1].raw[2]*c.y + pts[2].raw[2]*c.z;

			float w = pts[0].raw[3]*c.x + pts[1].raw[3]*c.y + pts[2].raw[3]*c.z;

			int frag_depth = max(0, min(255, int(z/w+.5)));

			if (c.x<0 || c.y<0 || c.z<0 ) continue;

			//z test
			if(zbuffer[P.y*Platform::screen_width + P.x]>frag_depth) continue;

			bool discard = shader.fragment(c, color);

			if (!discard) {
				zbuffer[P.y*Platform::screen_width + P.x] = frag_depth;
				SetPixel(P.x, P.y, color);
			}
		}
	}
}

