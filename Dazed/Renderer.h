#pragma once
class Renderer
{
public:
	Renderer();
	~Renderer();
private:
	int mWidth;
	int mHeight;
	unsigned int* mFBO;
	float* mZbuffer;
	unsigned int* mTexture;
};

