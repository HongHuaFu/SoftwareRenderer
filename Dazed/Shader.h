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
	// ����ֵ���ɶ�����ɫ�����ݵ�ƬԪ��ɫ��
	float vary_Intensity;

	// ����һ�� 4x1��������
	mat vertex(const vec3f& normal,const vec3f& vert,const vec3f& light,mat& MVP)
	{
		vary_Intensity =  light * normal ;
		vary_Intensity = (vary_Intensity > 0 ? vary_Intensity : 0.0f);
		return MVP * mat(vert);
	}

	// ����ƬԪ��ɫ 255
	Color fragment(float u,float v) override
	{
		int colorInten = 255 * vary_Intensity;
		return Color(colorInten,colorInten,colorInten);
	}
};
