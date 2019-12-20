#include "Renderer.h"
#include "Graphics.h"
#include "Rasterize.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Shader.h"





bool Renderer::Init(int width,int height,
					const char* title,
					const char* meshPath,
					const char* texturePath)
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
	
}

void Renderer::Quit()
{
	delete[] mZbuffer;
	stbi_image_free(mTexture);
}

void Renderer::clearBuffer()
{
	//zbuffer clean.
	Color clearColor = Color(0,0,0);
	for (int i=mWidth*mHeight; i--; mZbuffer[i] = -10000.0f);
	for(int i = 0; i<mWidth; i++)
	{
		for(int j = 0; j<mHeight; j++)
		{
			Platform::SetPixel(i,j,clearColor);
		}
	}
}
