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


struct GroundShader : public IShader {
	//vvec2f varying_uv; 
	vec3f varying_intensity;

	//vertex shader
	virtual vec4f vertex(int iface, int nthvert,const Mesh& mesh,const vec3f& light_dir) {


		float _in = mesh.vertexsNormal[(mesh.faceNormalIndex[iface]).raw[nthvert]]*light_dir;
		varying_intensity.raw[nthvert] = _in > 0.f? _in:0.f;

		vec3f orign_Vertex = mesh.vertexs[mesh.faceVertexIndex[iface].raw[nthvert]];

		mat gl_Vertex = v2m(orign_Vertex); 
		mat r = Viewport*Projection*ModelView*gl_Vertex; 

		return vec4f(r[0][0],r[1][0],r[2][0],r[3][0]);
	}
		


	virtual bool fragment(vec3f bar, Color &color) {
		float intensity = varying_intensity*bar * 255.f;  
		int r = intensity;
		color = Color(r,r,r);      
		return false;  //discard or not.
	}
};