#pragma once
class Renderer
{
public:
	Renderer();
	~Renderer();
private:
	int mWidth;
	int mHeight;
	float* mZbuffer;
	unsigned int* mTexture;
};

