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
	mat ModelView  = lookat(eye, center, Vec3f(0,1,0));
	mat Projection = Matrix::identity(4);
	Matrix ViewPort   = viewport(width/8, height/8, width*3/4, height*3/4);
	Projection[3][2] = -1.f/(eye-center).norm();

	while(1)
	{
		MSG msg;
		while (true) {
			if (!PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) break;
			if (!GetMessage(&msg, NULL, 0, 0)) break;
			DispatchMessage(&msg);
		}

		clearBuffer();
#pragma region Draw Region

		for (int i=0; i<mMesh.NumFaces; i++) {
			vec3f triangleVertex[3];
			vec3f triangleNormal[3];
			vec3f triangleUV[3];
	
			for (int j=0; j<3; j++) {
				triangleVertex[i] = mMesh.vertexs[mMesh.faceVertexIndex[i].raw[j]];
				triangleNormal[i] = mMesh.vertexsNormal[mMesh.faceNormalIndex[i].raw[j]];
				triangleUV[i] = mMesh.vertexTextures[mMesh.faceTextureIndex[i].raw[j]];
			}

			for(int i = 0; i<3; i++)
			{
				triangleVertex[i] = 
			}

			Rasterize::DrawLineTriangle(screen_coords[0],screen_coords[1],screen_coords[2],Color(255,0,0));
		}

#pragma endregion


		


		Platform::Update();
		Sleep(1);
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
