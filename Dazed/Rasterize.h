#pragma once
#include "Maths.h"
#include "Platform.h"
#include "Shader.h"

class Rasterize
{
public:
	static void SetPixel(int x,int y,const Color& col);

	//DDA draw line func.
	static void DrawLineDDA(const vec2i& p0,const vec2i& p1,const Color& col);

	//Bresenham draw line func.
	static void DrawLine(const vec2i& p0,const vec2i& p1,const Color& col);

	static void DrawLineTriangle(const vec2i& p0,const vec2i& p1,const vec2i& p2,const Color& col)
	{
		DrawLine(p0,p1,col);
		DrawLine(p0,p2,col);
		DrawLine(p1,p2,col);
	}

	static vec3f Barycentric(vec2f A, vec2f B, vec2f C, vec2f P);


	//clip triangle fully outside window.
	static bool ClipTriangles(vec4f* clipSpaceVertices){
		int count = 0;
		for(int i = 0; i < 3; ++i){
			vec4f vertex = clipSpaceVertices[i];
			bool inside = (-vertex.w <= vertex.x <= vertex.w ) 
				&& (-vertex.w <= vertex.y <= vertex.w)
				&& (0 <= vertex.z <= vertex.w);
			if (!inside) ++count;
		}
		
		return count == 3 ;
	}


	static void Triangle(vec4f pts[3],IShader &shader,float* zbuffer);
};

