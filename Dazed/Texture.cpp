#pragma once
#include "Texture.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture::Texture(const char * path)
{
	stbi_set_flip_vertically_on_load(true); 
	unsigned char* data = stbi_load(path, &width, &height, &channels, 0);
	

	for(int i = 0; i < width*height*channels; i++)
	{
		pixels.push_back(data[i]);
	}

	stbi_image_free(data);
}

Texture::~Texture()
{
}

Color Texture::GetPixel(float u,float v)
{
	if(u>1||v>1||u<0||v<0)
	{
		return Color(255,255,255);
	}
	int x = u * (width-1); 
	int y = v * (height-1);

	int index = (y * width + x)*channels;
	int r = pixels[index];
	int g = pixels[index+1];
	int b =	pixels[index+2];
	return Color(r,g,b);
}
