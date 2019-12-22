#pragma once
#include "Maths.hpp"

class Texture
{
public:
	Texture(){ }
	Texture(const char* path);
	~Texture();

	Color GetPixel(float u,float v);

private:
	int width,height,channels;

	//我为什么要用 vector<>而不是直接使用 unsigner char* pixels
	//因为我使用后者时，莫名会出现访问越界现象，用线性链表结构则解决了数据过大导致的内存越界
	std::vector<unsigned char> pixels;

};

