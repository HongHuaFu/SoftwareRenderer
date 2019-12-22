#pragma once
#include "Maths.hpp"

struct Shader
{
	virtual ~Shader(){ }
	virtual vec4f vertex(const vec3f& vertex,const vec3f& normal,
						 const vec3f& textureUV,
						int index,const vec3f& light) = 0;
	virtual Color fragment(float u,float v) = 0;
};


struct SimpleShader : public Shader {
	mat MVP;
	float vary_Intensity;


	vec4f vertex(const vec3f& vertex,const vec3f& normal,
		const vec3f& textureUV,
		int index,const vec3f& light) override
	{
		vary_Intensity = (std::max)(0.0f,normal*light);
		return (MVP * mat(vertex)).ToVec4f();
	}

	Color fragment(float u, float v) override{
		int inten = 255*vary_Intensity;
		return Color(inten,inten,inten);
	}

};