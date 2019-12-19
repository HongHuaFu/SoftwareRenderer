#pragma once
class Renderer
{
public:
	Renderer();
	~Renderer();
private:
	int mWidth;
	int mHeight;
	unsigned int** fbo;
	float** zbuffer;
	unsigned int**
};

