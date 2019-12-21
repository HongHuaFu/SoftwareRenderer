//#define FUNC_PASS_TEST //函数测试
//#define GRAPHICAS_TEST //图形测试
//#define Frame_Draw_DEBUG	//线框渲染测试 PASS
#define Fix_PipeLine_DEBUG	//固定渲染管线测试

#include <algorithm>
#include "Maths.h"
#include "Mesh.h"
#include "Shader.h"
#include <Windows.h>
#include <iostream>



// 全局变量:
int gWidth			= 480;					    // 窗口宽度
int gHeight			= 480;						// 窗口高度
LPCWSTR szTitle		= L"Hello World";			// 标题栏文本
unsigned char* fbo	= nullptr;                  // 帧缓存
int gInputKeys[512] = {0};						// 按键状态
float* zbuffer		= nullptr;					// z缓存
Mesh gMesh;										// 渲染网格
const char* gMeshPath
					= "a.obj";					// 网格路径

// 模型位置属性
vec3f gMeshMove			= {0.0f,0.0f,0.0f};	    // 模型在世界空间下偏移
vec3f gMeshRotate		= {0.0f,0.0f,0.0f};	    // 模型旋转值
vec3f gMeshScale		= {1.0f,1.0f,1.0f};		// 模型缩放值
vec3f gLight			= {0.0f,0.0f,1.0f};

// 相机属性
vec3f gEye				= {0.0f,0.0f,8.0f};		//相机在世界空间下的位置
vec3f gAt				= {0.0f,0.0f,0.0f};		//相机视点
vec3f gUp				= {0.0f,1.0f,0.0f};		//相机向上方向向量
float gAspect			= gWidth/gHeight;		//宽高比
float gFovy				= 45;					//视野
float gFarZ				= 0.1f;					//近平面
float gNearZ			= 100.0f;				//远平面




// 顶点变换相关矩阵,以及它们的计算函数
mat	ModelMatrix;								// 世界转换
void ComputeModelMatrix(const mat&,const mat&,const mat&);		
mat ViewMatrix;									// 视见转换
void ComputeViewMatrix(const vec3f&,const vec3f&,const vec3f);		
mat ProjectMatrix;								// 投影转换	
void ComputeProjectMatrix(float,float,float,float);	
mat ViewportMatrix;								// 视口转换
void ComputeViewportMatrix(float,float,float,float);	
mat MVP;										// MVP矩阵计算
void ComputeMVP();



HINSTANCE hInst		= NULL;                     // 当前实例
HWND ghMainWnd		= NULL;						// HWND
HDC ghdcMainWnd     = NULL;						// HDC
HBITMAP gdibMap		= NULL;						// DIB
HBITMAP goldMap		= NULL;						// BitMap


WCHAR szWindowClass[] = L"xsIkL_46O9";          // 主窗口类名


// 此代码模块中包含的函数的前向声明:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
void				SetPixel(const int&,const int&,const Color&);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

//光栅化函数
bool Cliptriangle(mat*);
void RasterizeTriangle(vec4f*,Shader&);
vec3f Barycentric(vec2f,vec2f,vec2f,vec2f);
void DrawLine_DDA(const vec2i& p0,const vec2i& p1,const Color& col);
void DrawLine(const vec2i& p0,const vec2i& p1,const Color& col);
void RasterizeTriangle_Fixpipleline(vec4f* NDC_vertex);


#ifdef FUNC_PASS_TEST
//测试函数
int main()
{
	std::cout<<"输入 T 进入测试模式，输入其它符号进入渲染模式。" << std::endl;
	char flag;
	std::cin>>flag;
	if(flag=='T')
	{
		//矩阵乘法测试  Pass
		mat a = mat::identity(4);
		mat b = mat::identity(4);
		a[0][0] = 4;
		a[0][1] = 4;
		a[1][1] = 2;
		a[1][3] = -1;
		a[2][2] = 2;
		a[3][3] = 0;
		a[3][2] = 2;

		b[0][0] = 5;
		b[0][2] = 5;
		b[1][1] = 5;
		b[1][3] = 75;
		b[2][0] = 5;
		b[2][2] = 0;
		b[2][3] = 5;
		b[3][2] = 5;
		b[3][3] = 0;
		mat c = a*b;
	
	}

	return 0;
}




	
#else
//主函数
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: 在此处初始化渲染器资源。
	// 1.创建zbuffer
	zbuffer = new float[gWidth*gHeight];			// TODO: 记得释放
	// 2.加载网格
	gMesh = Mesh();	
	ObjParser::ParserMesh(gMeshPath,gMesh);
	// 3.计算变换矩阵
	ComputeMVP();
	// 因为视口变换比较简单，我直接在渲染循环里计算，不使用矩阵。
	//ComputeViewportMatrix(0,0,1,0);

	// 注册窗口类
	MyRegisterClass(hInstance);

	// 执行应用程序初始化:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	MSG msg;

	// 主消息循环:
	while (true)
	{
		// 检测队列中是否有消息，如果有，读取它
		if (PeekMessage(&msg,0,0,0,PM_REMOVE))
		{
			// 检测是否是退出消息
			if(msg.message == WM_QUIT) 
				break;

			// 转换加速
			TranslateMessage(&msg);

			//将消息发送给WndProc处理
			DispatchMessage(&msg);
		}

		// 渲染逻辑
		// 1.清除上一帧缓存
		// 1.1 清除zbuffer
		for (int i=gWidth*gHeight; i--; zbuffer[i] = 1.0f);
		// 1.2 清除帧颜色缓存
		memset(fbo, 0, gWidth * gHeight * 4);

		// 2.在此处循环渲染模型。

#ifdef Frame_Draw_DEBUG
		for(int i = 0; i<gMesh.NumFaces; i++)
		{
			vec3f triangleVertex[3];
			vec2i screenVertex[3];
			//读取每个三角形的顶点，法线和UV
			for(int j = 0; j<3; j++)
			{
				// -1是因为obj网格面是从1开始而非0
				triangleVertex[j] = gMesh.vertexs[gMesh.faceVertexIndex[i].raw[j] -1];
				// 转化为屏幕中央
				// -0.5是为了防止溢出
				screenVertex[j].x =(int)( (triangleVertex[j].x + 1.0f)*gWidth / 2.0f - .5);
				screenVertex[j].y =(int)( (triangleVertex[j].y + 1.0f)*gHeight / 2.0f - .5);
			}

			DrawLine(screenVertex[0],screenVertex[1],Color(255,0,0));
			DrawLine(screenVertex[0],screenVertex[2],Color(255,0,0));
			DrawLine(screenVertex[2],screenVertex[1],Color(255,0,0));
		}
#endif // 线框渲染测试 Pass

#ifdef Fix_PipeLine_DEBUG
		ComputeMVP();

		for(int i = 0; i<gMesh.NumFaces; i++)
		{
			vec3f triangleVertex[3];
			vec4f SV_vertex[3];
			//读取每个三角形的顶点，法线和UV
			for(int j = 0; j<3; j++)
			{
				// -1是因为obj网格面是从1开始而非0
				triangleVertex[j] = gMesh.vertexs[gMesh.faceVertexIndex[i].raw[j] -1];
				
				mat tmp = MVP * mat(triangleVertex[j]);

				//转化为裁剪空间坐标
				SV_vertex[j] = tmp.ToVec4f();
			}
			//跳过裁剪阶段

			//透视除法阶段 转化为NDC空间下
			vec4f NDC_Vertex[3];
			for(int j = 0; j < 3; ++j){
				float w = SV_vertex[j].w;
				NDC_Vertex[j] = SV_vertex[j] * (1.f / w);
				NDC_Vertex[j].w = w; //保留w分量
			}
			RasterizeTriangle_Fixpipleline(NDC_Vertex);
		}

#endif // 固定渲染管道渲染


#ifdef Shader_DEBUG__

		SimpleShader shader_simple;
		for(int i = 0; i<gMesh.NumFaces; i++)
		{
			vec3f triangleVertex[3];
			vec3f triangleNormal[3];
			vec3f triangleUv[3];

			//读取每个三角形的顶点，法线和UV
			for(int j = 0; j<3; j++)
			{
				// -1是因为obj网格面是从1开始而非0
				triangleVertex[j] = gMesh.vertexs[gMesh.faceVertexIndex[i].raw[j] -1];
				triangleNormal[j] = gMesh.vertexsNormal[gMesh.faceNormalIndex[i].raw[j] -1];
				triangleUv[j] = gMesh.vertexTextures[gMesh.faceTextureIndex[i].raw[j] -1];
			}


			//应用着色器
			mat SV_Vertex[3];
			for(int j = 0; j < 3; ++j){
				SV_Vertex[j] = shader_simple.vertex(triangleNormal[j],triangleVertex[j], 
					gLight, MVP);
			}

			// 视锥剔除阶段
			// 没有裁剪阶段因为计算起来好慢

			//透视除法阶段 转化为NDC空间下
			vec4f NDC_Vertex[3];
			for(int j = 0; j < 3; ++j){
				NDC_Vertex[j] = SV_Vertex[j].ToVec4f();
				float w = NDC_Vertex[j].w;
				NDC_Vertex[j] = NDC_Vertex[j] * (1.f / w);
				NDC_Vertex[j].w = w; //保留w分量
			}

			//背面剔除阶段 可选

			//光栅化阶段
			RasterizeTriangle(NDC_Vertex,shader_simple);
		}
#endif // Shader渲染测试



			
		




		// 3.将BitMap更新到屏幕中
		HDC hDC = GetDC(ghMainWnd);
		BitBlt(hDC, 0, 0, gWidth, gHeight, ghdcMainWnd, 0, 0, SRCCOPY);
		ReleaseDC(ghMainWnd, hDC);
	}

	// TODO: 释放资源
	// 1. 释放zbuffer
	delete[] zbuffer;	zbuffer = nullptr;

	return (int) msg.wParam;
}


#endif //  FUNC_PASS_TEST

//
//  函数: MyRegisterClass()
//
//  目标: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style          = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc    = WndProc;
	wcex.cbClsExtra     = 0;
	wcex.cbWndExtra     = 0;
	wcex.hInstance      = hInstance;
	wcex.hIcon          = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground  = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wcex.lpszMenuName   = NULL;
	wcex.lpszClassName  = szWindowClass;
	wcex.hIconSm        = LoadIcon(NULL,IDI_APPLICATION);
	return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目标: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // 将实例句柄存储在全局变量中

	ghMainWnd = CreateWindowW(szWindowClass, szTitle, 
					WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
					0, 0, 0, 0, nullptr, nullptr, hInstance, nullptr);


	if (!ghMainWnd)
	{
		return FALSE;
	}

	// 获取窗口HDC
	HDC tmpHdc = GetDC(ghMainWnd);
	ghdcMainWnd = CreateCompatibleDC(tmpHdc);
	ReleaseDC(ghMainWnd, tmpHdc);

	// 创建DIB
	LPVOID ptr;
	BITMAPINFO bi = { { sizeof(BITMAPINFOHEADER), 
		gWidth, -gHeight, 1, 32, BI_RGB, 
		gWidth * gHeight * 4, 0, 0, 0, 0 }  };
	gdibMap = CreateDIBSection(ghdcMainWnd, &bi, DIB_RGB_COLORS, &ptr, 0, 0);
	if(gdibMap == NULL)
		return false;
	goldMap = (HBITMAP)SelectObject(ghdcMainWnd, gdibMap);

	// 创建并绑定帧缓存
	fbo = (unsigned char*)ptr;
	memset(fbo, 0, gWidth * gHeight * 4);
	memset(gInputKeys, 0, sizeof(int) * 512);

	// 调整窗口大小和位置
	RECT rect = { 0, 0, gWidth, gHeight };
	AdjustWindowRect(&rect, GetWindowLong(ghMainWnd, GWL_STYLE), 0);
	int wx = rect.right - rect.left;
	int wy = rect.bottom - rect.top;
	int sx = (GetSystemMetrics(SM_CXSCREEN) - wx) / 2;
	int sy = (GetSystemMetrics(SM_CYSCREEN) - wy) / 2;
	if (sy < 0) sy = 0;

	SetWindowPos(ghMainWnd, NULL, sx, sy, wx, wy, 
		(SWP_NOCOPYBITS | SWP_NOZORDER | SWP_SHOWWINDOW));
	SetForegroundWindow(ghMainWnd);

	ShowWindow(ghMainWnd, nCmdShow);
	UpdateWindow(ghMainWnd);

	return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目标: 处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	// 按键按下
	case WM_KEYDOWN:
	{
		// 退出程序
		if(wParam==VK_ESCAPE)
		{
			DestroyWindow(ghMainWnd);
		}
	}
	break;

	// 处理退出
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	// 默认
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

//
//  函数: SetPixel(const int&,const int&,const Color&)
//
//  目标: 设置屏幕坐标像素。
//
//    x	   -  x坐标		0 - gWidth
//    y    -  y坐标		0 - gHeight
//  Color  - 像素点颜色	0 - 255
//  保证 x,y 位于屏幕空间内。
//
void SetPixel(const int& x,const int& y,const Color& col)
{
	unsigned char *dst = &(fbo[y * gWidth * 4 + x*4]);
	dst[0] = (unsigned char)(col.b); 
	dst[1] = (unsigned char)(col.g);  
	dst[2] = (unsigned char)(col.r);
	dst = nullptr;
}


//
//  函数: ComputeModelMatrix(const mat& scale,const mat& rotate,const mat& move)
//
//  目标: 计算模型空间到世界空间的转换矩阵。
//
//    scale	   -  (Sx,Sy,Sz)  x,y,z 轴的缩放倍数		
//    rotate   -  (Rx,Ry,Rz)  x,y,z 轴的旋转度数			
//    move     -  (Dx,Dy,Dz)  x,y,z 轴的原点偏移	
//
void ComputeModelMatrix(const vec3f& scale,const vec3f& rotate,const vec3f& move)
{

	/*
	  顶点缩放，表示为
			  ——							 ——				——
			  |	x_World = k1 * x_Model;		 |  k1  0	0	 |
		S =   | y_World = k2 * y_Model;  =》 |  0	k2	0	 |
			  | z_World = k3 * z_Model;		 |  0   0	k3	 |
			  ——							 ——				——

	  位移，表示为


			  ——							 ——				  ——
			  |	x_World = d1 + x_Model;		 |  1   0	0	d1 |
	    M =   | y_World = d2 + y_Model;  =》 |  0	1	0	d2 |
			  | z_World = d3 + z_Model;		 |  0   0	1   d3 |
			  |								 |	0   0   0   1  |
			  ——							 ——				 ——

	 旋转，假设在x-y坐标系下，线段长度为r，它起点在原点，终点为p1,与x轴夹角为 a1,
	 旋转角为a2,终点为p2。设旋转点为原点。

	 那么: 设 p1坐标为 (x1,y1)
			 p2 坐标为(x2,y2)

			 x1 = r* cosa1;
			 y1 = r* sina1;

			 x2 = r*cos(a1+a2);
			 y2 = r*sin(a1+a2);

			 展开有：
				x2 = x1* cosa2 - y1 * sina2;
				y2 = x1* sina2 + y1 * cosa2;

			实际上，z轴可视为垂直于x-y片面指向外，因此，这就是绕z轴的旋转方程。
			同理，见 x-y 替换成 x-z 和 y-z即可得到 绕y轴和绕 x轴的旋转方程。

			提取矩阵 Rx,Ry,Rz.
			那么旋转矩阵就为

			R = RzRyRx;		
			不考虑欧拉角万向节锁
		
		此时，模型矩阵	Model =  Move * Rotate * Scale 补齐为齐次矩阵

		注意表面法线的转换
			若包含了非均匀缩放操作，那么它的转换矩阵
			应该为Model逆的转置。
		证明在 https://zhuanlan.zhihu.com/p/72734738 别人写的
	*/
	// 1.计算缩放矩阵
	mat ScaleMatrix = mat::identity(4);
	ScaleMatrix[0][0] = scale.x;
	ScaleMatrix[1][1] = scale.y;
	ScaleMatrix[2][2] = scale.z;

	// 2.计算位移矩阵
	mat MoveMatrix = mat::identity(4);
	MoveMatrix[0][3] = move.x;
	MoveMatrix[1][3] = move.y;
	MoveMatrix[2][3] = move.z;

	// 3.计算旋转矩阵
	mat RotateMatrix = mat::identity(4);
	float cosX = std::cos(rotate.x);
	float sinX = std::sin(rotate.x);

	float cosY = std::cos(rotate.y);
	float sinY = std::sin(rotate.y);

	float cosZ = std::cos(rotate.z);
	float sinZ = std::sin(rotate.z);

	RotateMatrix[0][0] = cosY*cosZ;
	RotateMatrix[0][1] = -cosX*sinZ + sinX*sinY*cosZ;
	RotateMatrix[0][2] = sinX*sinZ + cosX*sinY*cosZ;

	RotateMatrix[1][0] = cosY*sinZ;
	RotateMatrix[1][1] = cosX*cosZ + sinX*sinY*sinZ;
	RotateMatrix[1][2] = -sinX*cosZ + cosX*sinY*sinZ;

	RotateMatrix[2][0] = -sinY;
	RotateMatrix[2][1] = sinX*cosY;
	RotateMatrix[2][2] = cosX*cosY;

	ModelMatrix = MoveMatrix * RotateMatrix * ScaleMatrix;

}

//
//  函数: ComputeViewMatrix(const vec3f&,const vec3f&,const vec3f)
//
//  目标: 计算视见转换矩阵。
//
//    eye	 -  (Ex,Ey,Ez)  摄像机在世界空间下的坐标	
//    at	 -  (Ax,Ay,Az)  摄像机指向的参考点		
//    up     -  (Dx,Dy,Dz)  相机上方向量，通常定义为y轴
//
//	对应OpenGL的LookAt函数
//  分析在我的文章里有
//
void ComputeViewMatrix(const vec3f& eye,const vec3f& at,const vec3f up)
{
	// 1.施密特正交化
	vec3f n = (eye - at).normalize(); //z
	vec3f u = (up ^ n).normalize();	  // x
	vec3f v = (n ^ u).normalize();		//y

	mat viewMat = mat::identity(4);
	viewMat[0][0] = u.x;
	viewMat[0][1] = u.y;
	viewMat[0][2] = u.z;
	viewMat[0][3] =  -(eye * u);

	viewMat[1][0] = v.x;
	viewMat[1][1] = v.y;
	viewMat[1][2] = v.z;
	viewMat[1][3] =  -(eye * v);

	viewMat[2][0] = n.x;
	viewMat[2][1] = n.y;
	viewMat[2][2] = n.z;
	viewMat[2][3] =  -(eye * n);

	ViewMatrix = viewMat;
}


//
//  函数: ComputeProjectMatrix(float,float,float,float)
//
//  目标: 计算投影矩阵矩阵。
//
//    fovy		-  截平面 y-z 两视锥体连线夹角（角度）	
//    aspect	-  投影平面宽高比	
//    farZ      -  视锥体最远Z平面于原点距离
//	  nearZ		-  视锥体最近Z平面与原点距离
//
void ComputeProjectMatrix(float fovy,float aspect,float farZ,float nearZ)
{
	mat projMat = mat::identity(4);
	// cot(fovy* pi / 360)
	float cotFovyDiv2 = 1 / tan(fovy*0.0087266462599716478846184538424);

	//使用左手坐标系
	projMat[0][0] = cotFovyDiv2 / aspect;
	projMat[1][1] = cotFovyDiv2;
	projMat[2][2] = -(nearZ + farZ) / (farZ - nearZ);
	projMat[2][3] = -2*nearZ*farZ / (farZ - nearZ);
	projMat[3][2] = -1;
	projMat[3][3] = 0;

	ProjectMatrix = projMat;
}


//
//  函数: ComputeViewportMatrix(float,float,float,float)
//
//  目标: 计算视口变换矩阵。
//
//    minX		-  屏幕左上角与渲染画布左上角顶点的x距离
//    minY		-  屏幕左上角与渲染画布左上角顶点的y距离
//    minZ      -  NDC矩形盒最小Z深度 （0）
//	  maxZ		-  NDC矩形盒最大Z深度 （1）
//
void ComputeViewportMatrix(float minX,float minY,float maxZ,float minZ)
{
	mat viewport = mat::identity(4);
	viewport[0][0] = gWidth/2.f;
	viewport[1][1] = gHeight/2.f;
	viewport[2][2] = maxZ - minZ;
	viewport[0][3] = minX + gWidth/2;
	viewport[1][3] = minY + gHeight/2;
	viewport[2][3] = minZ;

	ViewMatrix = viewport;
}

// 计算MVP矩阵
void ComputeMVP()
{
	ComputeModelMatrix(gMeshScale,gMeshRotate,gMeshMove);
	ComputeViewMatrix(gEye,gAt,gUp);
	ComputeProjectMatrix(gFovy,gAspect,gFarZ,gNearZ);

	MVP = ProjectMatrix * ViewMatrix * ModelMatrix;
}


//剔除视锥体外的三角网格
bool Cliptriangle(mat* SV_vertexs)
{
	int count = 0;
	for(int i = 0; i < 3; ++i){
		vec4f vertex = SV_vertexs[i].ToVec4f();
		bool inside = (-vertex.w <= vertex.x <= vertex.w ) 
			&& (-vertex.w <= vertex.y <= vertex.w)
			&& (0 <= vertex.z <= vertex.w);
		if (!inside) ++count;
	}
	//当计数为3时三个顶点都在视锥体外，将其剔除
	return count == 3 ;
}




// 参考http://blackpawn.com/texts/pointinpoly/
vec3f Barycentric(vec2f A,vec2f B,vec2f C,vec2f P)
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


void RasterizeTriangle_Fixpipleline(vec4f* NDC_vertex)
{
	// 1. 视口转换 (-1,1) => (0,width/height)
	// 我简单的映射到全屏，就不使用全局的ViewPort矩阵了
	vec3f gl_coord[3];
	for(int i = 0; i<3; i++)
	{
		gl_coord[i].x = (NDC_vertex[i].x + 1.0f) * gWidth / 2;
		gl_coord[i].y = (NDC_vertex[i].y + 1.0f) * gHeight / 2;
		gl_coord[i].z = (NDC_vertex[i].z + 1.0f) / 2; //深度值
	}

	// 2.计算包围盒 
	int xMax = (std::max)({gl_coord[0].x, gl_coord[1].x, gl_coord[2].x});
	int xMin = (std::min)({gl_coord[0].x, gl_coord[1].x, gl_coord[2].x});

	int yMax = (std::max)({gl_coord[0].y, gl_coord[1].y, gl_coord[2].y});
	int yMin = (std::min)({gl_coord[0].y, gl_coord[1].y, gl_coord[2].y});

	xMax = (std::min)(xMax,gWidth -1);
	xMin = (std::max)(xMin, 0);
	yMax = (std::min)(yMax, gHeight -1);
	yMin = (std::max)(yMin, 0);

	//遍历包围盒内的像素点
	for(int x = xMin; x<= xMax; x++)
	{
		for(int y = yMin; y<= yMax; y++)
		{
			//计算重心三角形
			vec2f current_pixel = vec2f(x,y);
			vec2f A = vec2f(gl_coord[0].x,gl_coord[0].y);
			vec2f B = vec2f(gl_coord[1].x,gl_coord[1].y);
			vec2f C = vec2f(gl_coord[2].x,gl_coord[2].y);
			vec3f weight = Barycentric(A,B,C,current_pixel);

			//不在三角形内部的像素点跳过
			if(weight.x <0 || weight.y<0||weight.z < 0) continue;

			//深度插值
			float currentDepth = weight.x * gl_coord[0].z +
				weight.y * gl_coord[1].z +
				weight.z * gl_coord[2].z;

			//深度测试
			if(currentDepth > zbuffer[y * gWidth+x]) continue;

			
			SetPixel(x,y,Color(255,0,0));
		}
	}

}


void RasterizeTriangle(vec4f* NDC_vertex,Shader& shader)
{
	// 1. 视口转换 (-1,1) => (0,width/height)
	// 我简单的映射到全屏，就不使用全局的ViewPort矩阵了
	vec3f gl_coord[3];
	for(int i = 0; i<3; i++)
	{
		gl_coord[i].x = (NDC_vertex[i].x + 1.0f) * gWidth / 2;
		gl_coord[i].y = (NDC_vertex[i].y + 1.0f) * gHeight / 2;
		gl_coord[i].z = (NDC_vertex[i].z + 1.0f) / 2; //深度值
	}

	// 2.计算包围盒 
	int xMax = (std::max)({gl_coord[0].x, gl_coord[1].x, gl_coord[2].x});
	int xMin = (std::min)({gl_coord[0].x, gl_coord[1].x, gl_coord[2].x});

	int yMax = (std::max)({gl_coord[0].y, gl_coord[1].y, gl_coord[2].y});
	int yMin = (std::min)({gl_coord[0].y, gl_coord[1].y, gl_coord[2].y});

	xMax = (std::min)(xMax,gWidth -1);
	xMin = (std::max)(xMin, 0);
	yMax = (std::min)(yMax, gHeight -1);
	yMin = (std::max)(yMin, 0);

	//遍历包围盒内的像素点
	for(int x = xMin; x<= xMax; x++)
	{
		for(int y = yMin; y<= yMax; y++)
		{
			//计算重心三角形
			vec2f current_pixel = vec2f(x,y);
			vec2f A = vec2f(gl_coord[0].x,gl_coord[0].y);
			vec2f B = vec2f(gl_coord[1].x,gl_coord[1].y);
			vec2f C = vec2f(gl_coord[2].x,gl_coord[2].y);
			vec3f weight = Barycentric(A,B,C,current_pixel);

			//不在三角形内部的像素点跳过
			if(weight.x <0 || weight.y<0||weight.z < 0) continue;

			//深度插值
			float currentDepth = weight.x * gl_coord[0].z +
								 weight.y * gl_coord[1].z +
								 weight.z * gl_coord[2].z;
			//深度测试
			if(currentDepth > zbuffer[y * gWidth+x]) continue;

			Color fragmenCol = shader.fragment(x,y);
			SetPixel(x,y,fragmenCol);
		}
	}

}

//DDA画线算法
void DrawLine_DDA(const vec2i& p0,const vec2i& p1,const Color& col)
{
	int dx = p1.x - p0.x;
	int dy = p1.y-p0.y;
	int steps;
	float ddx,ddy;
	float x= p0.x;
	float y = p0.y;

	fabs(dx)>fabs(dy) ? steps = fabs(dx):steps = fabs(dy);

	ddx =(float) dx /(float)steps;
	ddy =(float) dy / (float)steps;

	SetPixel(round(x),round(y),col);
	for(int k = 0; k<steps; k++)
	{
		x+=ddx;
		y+=ddy;
		SetPixel(round(x),round(y),col);
	}
}

// Bresenham 画线算法
void DrawLine(const vec2i& p0,const vec2i& p1,const Color& col)
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
