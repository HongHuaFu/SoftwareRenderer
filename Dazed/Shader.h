#pragma once
#include "Maths.h"
#include "Mesh.h"

struct Shader
{
	virtual ~Shader() {};
	virtual Color fragment(float u, float v) = 0;
};


struct SimpleShader: public Shader
{
	// 传递值，由顶点着色器传递到片元着色器
	float vary_Intensity;

	// 返回一个 4x1的列向量
	mat vertex(const vec3f& normal,const vec3f& vert,const vec3f& light,mat& MVP)
	{
		vary_Intensity =  light * normal ;
		vary_Intensity = (vary_Intensity > 0 ? vary_Intensity : 0.0f);
		return MVP * mat(vert);
	}

	// 返回片元颜色 255
	Color fragment(float u,float v) override
	{
		int colorInten = 255 * vary_Intensity;
		return Color(colorInten,colorInten,colorInten);
	}
};
