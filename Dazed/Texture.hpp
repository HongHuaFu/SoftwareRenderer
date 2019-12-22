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

	//��ΪʲôҪ�� vector<>������ֱ��ʹ�� unsigner char* pixels
	//��Ϊ��ʹ�ú���ʱ��Ī������ַ���Խ����������������ṹ���������ݹ����µ��ڴ�Խ��
	std::vector<unsigned char> pixels;

};

