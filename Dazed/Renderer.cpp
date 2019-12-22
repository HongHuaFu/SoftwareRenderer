#include "Renderer.hpp"

SenceParameters gMainSenceParameter;

bool Renderer::Init()
{
	const char* winTitle = "Dazed";
	const char* meshPath = "b.obj";
	mWidth = 700;
	mHeight = 700;

	mZbuffer = new float[mWidth * mHeight];
	ObjParser::ParserMesh(meshPath,mMesh);


	gMainSenceParameter = SenceParameters();
	gMainSenceParameter.gMeshMove	= {0.0f,0.8f,0.0f};
	gMainSenceParameter.gMeshRotate = {-30.0f,0,180.f};
	gMainSenceParameter.gMeshScale	= {1.5f,1.5f,1.5f};

	gMainSenceParameter.gLight		= {0.0f,0.0f,-1.0f};

	gMainSenceParameter.gAspect		= mWidth/mHeight;
	gMainSenceParameter.gEye		= {0.0f,0.0f,-3.0f};
	gMainSenceParameter.gAt			= {0.0f,0.0f,0.0f};	
	gMainSenceParameter.gUp			= {0.0f,1.0f,0.0f};	

	gMainSenceParameter.gFovy		= 60;
	gMainSenceParameter.gFarZ		= 0.1f;
	gMainSenceParameter.gNearZ		= 100.0f;


	Platform::MyRegisterClass();
	if(!Platform::InitInstance(mWidth,mHeight,winTitle))
	{
		return false;
	}



	return true;
}

void Renderer::Loop()
{
	SimpleShader Shader_simple; 

	MSG msg;
	while (true)
	{
		if (PeekMessage(&msg,0,0,0,PM_REMOVE))
		{
			if(msg.message == WM_QUIT) 
				break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		ClearBuffer();
		//DrawLine(vec2i(10,10),vec2i(230,403),Color(255,255,0));

		Shader_simple.MVP =  Geometry::ComputeMVP(gMainSenceParameter);
		for(int i = 0; i<mMesh.NumFaces; i++)
		{
			vec3f triangleVertex[3];
			vec3f triangleNormal[3];
			vec3f triangleUv[3];

			for(int j = 0; j<3; j++)
			{
				triangleVertex[j] = mMesh.vertexs[mMesh.faceVertexIndex[i].raw[j] -1];
				triangleNormal[j] = mMesh.vertexsNormal[mMesh.faceNormalIndex[i].raw[j] -1];
				triangleUv[j] = mMesh.vertexTextures[mMesh.faceTextureIndex[i].raw[j] -1];
			}


			vec4f SV_Vertex[3];
			for(int j = 0; j < 3; ++j){
				SV_Vertex[j] = Shader_simple.vertex(triangleVertex[j],triangleNormal[j],triangleUv[j],j,gMainSenceParameter.gLight);
			}


			//透视除法阶段
			for(int j = 0; j < 3; ++j){
				float re_w = 1.0f / SV_Vertex[j].w;
				SV_Vertex[j] = SV_Vertex[j] * re_w;
				SV_Vertex[j].w = re_w;
			}


			RasterizeTriangle(SV_Vertex, Shader_simple);
		}


		Platform::UpdateBuffer(mWidth,mHeight);
	}
}

void Renderer::Destroy()
{
	delete[] mZbuffer;
}

void Renderer::ClearBuffer()
{
	for (int i=mWidth*mHeight; i--; mZbuffer[i] = 10000.0f);
	memset(gFbo, 0, mWidth * mHeight * 4);
}

void Renderer::RasterizeTriangle(vec4f SV_vertexs[3],Shader& shader)

{
	// 1. 视口转换 (-1,1) => (0,width/height)
	// 我简单的映射到全屏，就不使用ViewPort矩阵了
	vec3f gl_coord[3];

	//用于透视插值校正
	float re_w[3]; 
	for(int i = 0; i<3; i++)
	{
		re_w[i] = SV_vertexs[i].w;
		gl_coord[i].x = (SV_vertexs[i].x + 1.0f) * mWidth / 2.f;
		gl_coord[i].y = (SV_vertexs[i].y + 1.0f) * mHeight / 2.f;
		gl_coord[i].z = (SV_vertexs[i].z + 1.0f) / 2.f; 
	}

	// 2.计算包围盒 
	float xMax = (std::max)({gl_coord[0].x, gl_coord[1].x, gl_coord[2].x});
	float xMin = (std::min)({gl_coord[0].x, gl_coord[1].x, gl_coord[2].x});

	float yMax = (std::max)({gl_coord[0].y, gl_coord[1].y, gl_coord[2].y});
	float yMin = (std::min)({gl_coord[0].y, gl_coord[1].y, gl_coord[2].y});

	xMax = (std::min)(xMax,(float)(mWidth -1.f));
	xMin = (std::max)(xMin, 0.f);
	yMax = (std::min)(yMax, (float)(mHeight -1.f));
	yMin = (std::max)(yMin, 0.f);

	int x = 0;
	int y =0;
	//遍历包围盒内的像素点
	for(x = xMin; x<= xMax; x++)
	{
		for(y = yMin; y<= yMax; y++)
		{
			//计算重心三角形
			vec2f current_pixel = vec2f(x,y);
			vec2f A = vec2f(gl_coord[0].x,gl_coord[0].y);
			vec2f B = vec2f(gl_coord[1].x,gl_coord[1].y);
			vec2f C = vec2f(gl_coord[2].x,gl_coord[2].y);
			vec3f weight = Barycentric(A,B,C,current_pixel);

			//不在三角形内部的像素点跳过
			if(weight.x <0 || weight.y<0||weight.z < 0) continue;

			//深度插值 因为没有透视校正所以为非线性
			float currentDepth = weight.x * gl_coord[0].z+
				weight.y * gl_coord[1].z+
				weight.z * gl_coord[2].z;

			//深度测试
			if(currentDepth > mZbuffer[y * mWidth+x]) continue;

			//深度排序
			mZbuffer[y * mWidth+x] = currentDepth;

			float u = x/mWidth;
			float v = y/mHeight;
			Color fragmenCol = shader.fragment(u,v);
			SetPixel(x,y,fragmenCol);
		}
	}

}

vec3f Renderer::Barycentric(vec2f A,vec2f B,vec2f C,vec2f P)

{
	vec2f ab = B - A;
	vec2f ac = C - A;
	vec2f ap = P - A;
	float factor = 1 / (ab.x * ac.y - ab.y * ac.x);
	float s = (ac.y * ap.x - ac.x * ap.y) * factor;
	float t = (ab.x * ap.y - ab.y * ap.x) * factor;
	vec3f weights = vec3f(1 - s - t, s, t);
	return weights;
}

void Renderer::SetPixel(const int & x,const int & y,const Color & col)
{
	unsigned char *dst = &(gFbo[y * mWidth * 4 + x*4]);
	dst[0] = (unsigned char)(col.b); 
	dst[1] = (unsigned char)(col.g);  
	dst[2] = (unsigned char)(col.r);
	dst = nullptr;
}

void Renderer::DrawLine(const vec2i & p0,const vec2i & p1,const Color & col)
{
	int x0 = p0.x; 
	int x1 = p1.x;
	int y0 = p0.y; 
	int y1 = p1.y;
	bool steep = false;
	if (std::abs( x0 - x1 ) < std::abs( y0 - y1 ))
	{
		std::swap( x0 , y0 );
		std::swap( x1 , y1 );
		steep = true;
	}

	if (x0 > x1)
	{
		std::swap( x0 , x1 );
		std::swap( y0 , y1 );
	}


	int dx = x1 - x0;
	int dy = y1 - y0;


	int dy2 = std::abs( dy ) * 2;
	int d = 0;
	int y = y0;


	for (int x = x0; x <= x1; x++)
	{
		if (steep)
		{
			SetPixel( y , x , col );
		}
		else
		{
			SetPixel( x , y , col );
		}

		d += dy2;
		if (d > dx)
		{
			y += ( y1 > y0 ? 1 : -1 );
			d -= dx * 2;
		}
	}
}


