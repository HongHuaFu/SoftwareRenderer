#pragma once
#include "Maths.hpp"
#include "Mesh.hpp"
#include "Geometry.hpp"
#include "Platform.hpp"
#include "Shader.hpp"

extern SenceParameters gMainSenceParameter;

class Renderer
{
public:
	bool Init();
	void Loop();
	void Destroy();

private:
	float* mZbuffer;
	int mWidth,mHeight;
	Mesh mMesh;

private:
	void ClearBuffer();
	void RasterizeTriangle(vec4f SV_Vertexs[3],Shader& shader);
	vec3f Barycentric(vec2f A,vec2f B,vec2f C,vec2f P);
	void SetPixel(const int& x,const int& y,const Color& col);
	void DrawLine(const vec2i& p0,const vec2i& p1,const Color& col);
};

