//#define FUNC_PASS_TEST //函数测试
//#define GRAPHICAS_TEST //图形测试


#include "Maths.h"
#include "Mesh.h"
#include <Windows.h>
#include <iostream>

// 颜色结构
struct Color{ int r,g,b; Color(int rr,int gg,int bb):r(rr),b(bb),g(gg){} };

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

// 顶点变换相关矩阵,以及它们的计算函数
mat	ModelMatrix;								// 世界转换
void ComputeModelMatrix(const mat&,const mat&,const mat&);		
mat ViewMatrix;									// 视见转换
void ComputeViewMatrix(const vec3f&,const vec3f&,const vec3f);		
mat ProjectMatrix;								// 投影转换	
void ComputeProjectMatrix(float,float,float,float);	
mat ViewportMatrix;								// 视口转换
void ComputeViewportMatrix(float,float,float,float);	



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

#ifdef FUNC_PASS_TEST
//测试函数
int main()
{
	std::cout<<"输入 T 进入测试模式，输入其它符号进入渲染模式。" << std::endl;
	char flag;
	std::cin>>flag;
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
		for (int i=gWidth*gHeight; i--; zbuffer[i] = -10000.0f);
		// 1.2 清除帧颜色缓存
		memset(fbo, 0, gWidth * gHeight * 4);

		// 2.在此处循环渲染模型。
#pragma region Render region
		SetPixel(240,240,Color(255,0,0));


#pragma endregion



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
	vec3f n = (eye - at).normalize();
	vec3f u = (up ^ n).normalize();
	vec3f v = (n ^ u).normalize();

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
	projMat[2][2] = farZ / (farZ - nearZ);
	projMat[2][3] = -nearZ*farZ / (farZ - nearZ);
	projMat[3][2] = 1;
	projMat[3][3] = 0;

	ProjectMatrix = projMat;
}