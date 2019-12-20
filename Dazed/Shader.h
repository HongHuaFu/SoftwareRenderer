#pragma once
#include "Maths.h"
#include "Platform.h"
#include "Mesh.h"
#include "Graphics.h"


struct IShader
{
	virtual ~IShader();
	virtual vec4f vertex(int iface, int nthvert,const Mesh& mesh,const vec3f& light_dir) = 0;
	virtual bool fragment(vec3f bar, Color &color) = 0;
};

static vec3f m2v(mat m)
{
	return vec3f(m[0][0] / m[3][0],m[1][0]/m[3][0],m[2][0]/m[3][0]);
}

static mat v2m(vec3f v)
{
	mat m(4, 1);
	m[0][0] = v.x;
	m[1][0] = v.y;
	m[2][0] = v.z;
	m[3][0] = 1.f;
	return m;
}


struct SimpleShader : public IShader {
	
	mat MVP;
	float varying_intensity;

	//vertex shader
	virtual vec4f vertex(const vec3f& v,const vec3f& normal,const vec3f &light) {

		varying_intensity = max(0.0f,normal*light);
		mat r = MVP * v2m(v);
		return vec4f(r[0][0],r[1][0],r[2][0],r[3][0]);
	}
		


	virtual bool fragment(vec3f bar, Color &color) {
		float intensity = varying_intensity*bar * 255.f;  
		int r = intensity;
		color = Color(r,r,r);      
		return false;  //discard or not.
	}
};