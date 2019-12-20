#pragma once
#include "Mesh.h"
#include "Platform.h"



class Renderer
{
public:
	
	bool Init(int width = 480,int height = 480,
		const char* title = "Dazed",
		const char* meshPath = "a.obj",
		const char* texturePath = "a.png");
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

