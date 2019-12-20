#include "Renderer.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"






bool Renderer::Init(int width = 480,int height = 480,
					const char* title = "Dazed",
					const char* meshPath = "a.obj",
					const char* texturePath = "a.png")
{
	mWidth = width;
	mHeight = height;

	//1.load mesh.
	ObjParser::ParserMesh(meshPath,mMesh);
	mZbuffer = new float[height*width];

	//2.load texture.
	int texWid,texHeight,texChannels;
	mTexture = stbi_load(texturePath,&texWid,&texHeight,&texChannels,STBI_rgb);

	return Platform::InitWindowApp(width,height,title);
}

void Renderer::Loop()
{
	float angle = 1.0f;
	float pos = 4.0f;
	while(Platform::screen_exit = false && Platform::screen_keys[VK_ESCAPE] ==0)
	{
		MSG msg;
		while (1) {
			if (!PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) break;
			if (!GetMessage(&msg, NULL, 0, 0)) break;
			DispatchMessage(&msg);
		}
	}
}

void Renderer::Quit()
{
	delete[] mZbuffer;
	stbi_image_free(mTexture);
}

void Renderer::clearBuffer()
{
	//zbuffer clean.
	for (int i=mWidth*mHeight; i--; mZbuffer[i] = -10000.0f);
}
