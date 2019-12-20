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
	vec3f light_dir(1,1,1);
	vec3f eye(0,-1,3);
	vec3f center(0,0,0);
	vec3f up(0,1,0);

	LookAt(eye, center, up);
	ViewPort(mWidth/8, mHeight/8, mWidth*3/4, mHeight*3/4);
	ProjectMatrix(-1.f/(eye-center).len());
	light_dir.normalize();

	GroundShader shader;

	while(1)
	{
		MSG msg;
		while (true) {
			if (!PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) break;
			if (!GetMessage(&msg, NULL, 0, 0)) break;
			DispatchMessage(&msg);
		}

		clearBuffer();
		for (int i=0; i<mMesh.NumFaces; i++) {
			vec4f screen_coords[3];
			for (int j=0; j<3; j++) {
				//vertex shader.
				screen_coords[j] = shader.vertex(i, j,mMesh,light_dir);
			}

			//if(Rasterize::ClipTriangles(screen_coords)) continue;
			Rasterize::Triangle(screen_coords,shader,mZbuffer);
		}


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
