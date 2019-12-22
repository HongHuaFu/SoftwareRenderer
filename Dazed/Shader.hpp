#pragma once
#include "Maths.hpp"
#include "Texture.hpp"

struct Shader
{
	virtual ~Shader(){ }
	virtual vec4f vertex(const vec3f& vertex,const vec3f& normal,
						 const vec3f& textureUV,
						int index,const vec3f& light) = 0;
	virtual Color fragment(vec3f& lerpCorrect) = 0;
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

	Color fragment(vec3f& lerpCorrect) override{
		int inten = 255*vary_Intensity;
		return Color(inten,inten,inten);
	}

};

struct TextureShader : public Shader
{
	mat MVP;
	Texture AbedoTexture;
	//使用透视校正
	float vary_Intensity[3];
	vec3f vary_UVs[3];
	float correct_vary_Intensity;
	vec3f correct_vary_UV;

	vec4f vertex(const vec3f& vertex,const vec3f& normal,
		const vec3f& textureUV,
		int index,const vec3f& light) override
	{
		vary_UVs[index] = textureUV;
		vary_Intensity[index] = (std::max)(0.0f,normal*light);
		return (MVP * mat(vertex)).ToVec4f();
	}

	Color fragment(vec3f& lerpCorrect) override{
		float normalizer =1/( lerpCorrect.x + lerpCorrect.y + lerpCorrect.z);
		correct_vary_Intensity = (vary_Intensity[0] * lerpCorrect.x  +
							      vary_Intensity[1] * lerpCorrect.y +
								  vary_Intensity[2] * lerpCorrect.z) * normalizer;
		correct_vary_UV = ( vary_UVs[0] * lerpCorrect.x  +
							vary_UVs[1] * lerpCorrect.y +
							vary_UVs[2] * lerpCorrect.z) * normalizer;

		return AbedoTexture.GetPixel(correct_vary_UV.x,correct_vary_UV.y);
	}

};