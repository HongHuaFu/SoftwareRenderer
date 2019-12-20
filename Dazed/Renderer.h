#pragma once
#include "Mesh.h"
#include "Platform.h"



class Renderer
{
public:
	bool Init(int,int,const char*,const char*,const char*);
	void Loop();
	void Quit();

private:
	void clearBuffer();
private:
	int mWidth;
	int mHeight;
	float* mZbuffer;
	unsigned char* mTexture;
	Mesh mMesh;
};

