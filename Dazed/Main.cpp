#include "Maths.h"
#include "Mesh.h"
#include <Windows.h>

// ��ɫ�ṹ
struct Color{ int r,g,b; Color(int rr,int gg,int bb):r(rr),b(bb),g(gg){} };

// ȫ�ֱ���:
int gWidth			= 480;					    // ���ڿ��
int gHeight			= 480;						// ���ڸ߶�
LPCWSTR szTitle		= L"Hello World";			// �������ı�
unsigned char* fbo	= nullptr;                  // ֡����
int gInputKeys[512] = {0};						// ����״̬
float* zbuffer		= nullptr;					// z����
Mesh gMesh;										// ��Ⱦ����
const char* gMeshPath
					= "a.obj";					// ����·��

// ����任��ؾ���,�Լ����ǵļ��㺯��
mat	ModelMatrix;								// ����ת��
void ComputeModelMatrix(const mat&,const mat&,const mat&);		
mat ViewMatrix;									// �Ӽ�ת��
void ComputeViewMatrix(const vec3f&,const vec3f&,const vec3f);		
mat ProjectMatrix;								// ͶӰת��	
void ComputeProjectMatrix(float,float,float,float);	
mat ViewportMatrix;								// �ӿ�ת��
void ComputeViewportMatrix(float,float,float,float);	



HINSTANCE hInst		= NULL;                     // ��ǰʵ��
HWND ghMainWnd		= NULL;						// HWND
HDC ghdcMainWnd     = NULL;						// HDC
HBITMAP gdibMap		= NULL;						// DIB
HBITMAP goldMap		= NULL;						// BitMap


WCHAR szWindowClass[] = L"xsIkL_46O9";          // ����������


// �˴���ģ���а����ĺ�����ǰ������:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
void				SetPixel(const int&,const int&,const Color&);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);


//������
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: �ڴ˴���ʼ����Ⱦ����Դ��
	// 1.����zbuffer
	zbuffer = new float[gWidth*gHeight];			// TODO: �ǵ��ͷ�
	// 2.��������
	gMesh = Mesh();	
	ObjParser::ParserMesh(gMeshPath,gMesh);

	// ע�ᴰ����
	MyRegisterClass(hInstance);

	// ִ��Ӧ�ó����ʼ��:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	MSG msg;

	// ����Ϣѭ��:
	while (true)
	{
		// ���������Ƿ�����Ϣ������У���ȡ��
		if (PeekMessage(&msg,0,0,0,PM_REMOVE))
		{
			// ����Ƿ����˳���Ϣ
			if(msg.message == WM_QUIT) 
				break;

			// ת������
			TranslateMessage(&msg);

			//����Ϣ���͸�WndProc����
			DispatchMessage(&msg);
		}

		// ��Ⱦ�߼�
		// 1.�����һ֡����
		// 1.1 ���zbuffer
		for (int i=gWidth*gHeight; i--; zbuffer[i] = -10000.0f);
		// 1.2 ���֡��ɫ����
		memset(fbo, 0, gWidth * gHeight * 4);

		// 2.�ڴ˴�ѭ����Ⱦģ�͡�
#pragma region Render region
		SetPixel(240,240,Color(255,0,0));


#pragma endregion



		// 3.��BitMap���µ���Ļ��
		HDC hDC = GetDC(ghMainWnd);
		BitBlt(hDC, 0, 0, gWidth, gHeight, ghdcMainWnd, 0, 0, SRCCOPY);
		ReleaseDC(ghMainWnd, hDC);
	}

	// TODO: �ͷ���Դ
	// 1. �ͷ�zbuffer
	delete[] zbuffer;	zbuffer = nullptr;

	return (int) msg.wParam;
}

//
//  ����: MyRegisterClass()
//
//  Ŀ��: ע�ᴰ���ࡣ
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
//   ����: InitInstance(HINSTANCE, int)
//
//   Ŀ��: ����ʵ�����������������
//
//   ע��:
//
//        �ڴ˺����У�������ȫ�ֱ����б���ʵ�������
//        ��������ʾ�����򴰿ڡ�
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // ��ʵ������洢��ȫ�ֱ�����

	ghMainWnd = CreateWindowW(szWindowClass, szTitle, 
					WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
					0, 0, 0, 0, nullptr, nullptr, hInstance, nullptr);


	if (!ghMainWnd)
	{
		return FALSE;
	}

	// ��ȡ����HDC
	HDC tmpHdc = GetDC(ghMainWnd);
	ghdcMainWnd = CreateCompatibleDC(tmpHdc);
	ReleaseDC(ghMainWnd, tmpHdc);

	// ����DIB
	LPVOID ptr;
	BITMAPINFO bi = { { sizeof(BITMAPINFOHEADER), 
		gWidth, -gHeight, 1, 32, BI_RGB, 
		gWidth * gHeight * 4, 0, 0, 0, 0 }  };
	gdibMap = CreateDIBSection(ghdcMainWnd, &bi, DIB_RGB_COLORS, &ptr, 0, 0);
	if(gdibMap == NULL)
		return false;
	goldMap = (HBITMAP)SelectObject(ghdcMainWnd, gdibMap);

	// ��������֡����
	fbo = (unsigned char*)ptr;
	memset(fbo, 0, gWidth * gHeight * 4);
	memset(gInputKeys, 0, sizeof(int) * 512);

	// �������ڴ�С��λ��
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
//  ����: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  Ŀ��: ���������ڵ���Ϣ��
//
//  WM_COMMAND  - ����Ӧ�ó���˵�
//  WM_PAINT    - ����������
//  WM_DESTROY  - �����˳���Ϣ������
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	// ��������
	case WM_KEYDOWN:
	{
		// �˳�����
		if(wParam==VK_ESCAPE)
		{
			DestroyWindow(ghMainWnd);
		}
	}
	break;

	// �����˳�
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	// Ĭ��
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

//
//  ����: SetPixel(const int&,const int&,const Color&)
//
//  Ŀ��: ������Ļ�������ء�
//
//    x	   -  x����		0 - gWidth
//    y    -  y����		0 - gHeight
//  Color  - ���ص���ɫ	0 - 255
//  ��֤ x,y λ����Ļ�ռ��ڡ�
//
void SetPixel(const int& x,const int& y,const Color& col)
{
	unsigned char *dst = &(fbo[y * gWidth * 4 + x*4]);
	dst[0] = (unsigned char)(col.b); 
	dst[1] = (unsigned char)(col.g);  
	dst[2] = (unsigned char)(col.r);
}


//
//  ����: ComputeModelMatrix(const mat& scale,const mat& rotate,const mat& move)
//
//  Ŀ��: ����ģ�Ϳռ䵽����ռ��ת������
//
//    scale	   -  (Sx,Sy,Sz)  x,y,z ������ű���		
//    rotate   -  (Rx,Ry,Rz)  x,y,z �����ת����			
//    move     -  (Dx,Dy,Dz)  x,y,z ���ԭ��ƫ��	
//
void ComputeModelMatrix(const vec3f& scale,const vec3f& rotate,const vec3f& move)
{

	/*
	  �������ţ���ʾΪ
			  ����							 ����				����
			  |	x_World = k1 * x_Model;		 |  k1  0	0	 |
		S =   | y_World = k2 * y_Model;  =�� |  0	k2	0	 |
			  | z_World = k3 * z_Model;		 |  0   0	k3	 |
			  ����							 ����				����

	  λ�ƣ���ʾΪ


			  ����							 ����				  ����
			  |	x_World = d1 + x_Model;		 |  1   0	0	d1 |
	    M =   | y_World = d2 + y_Model;  =�� |  0	1	0	d2 |
			  | z_World = d3 + z_Model;		 |  0   0	1   d3 |
			  |								 |	0   0   0   1  |
			  ����							 ����				 ����

	 ��ת��������x-y����ϵ�£��߶γ���Ϊr���������ԭ�㣬�յ�Ϊp1,��x��н�Ϊ a1,
	 ��ת��Ϊa2,�յ�Ϊp2������ת��Ϊԭ�㡣

	 ��ô: �� p1����Ϊ (x1,y1)
			 p2 ����Ϊ(x2,y2)

			 x1 = r* cosa1;
			 y1 = r* sina1;

			 x2 = r*cos(a1+a2);
			 y2 = r*sin(a1+a2);

			 չ���У�
				x2 = x1* cosa2 - y1 * sina2;
				y2 = x1* sina2 + y1 * cosa2;

			ʵ���ϣ�z�����Ϊ��ֱ��x-yƬ��ָ���⣬��ˣ��������z�����ת���̡�
			ͬ���� x-y �滻�� x-z �� y-z���ɵõ� ��y����� x�����ת���̡�

			��ȡ���� Rx,Ry,Rz.
			��ô��ת�����Ϊ

			R = RzRyRx;		
			������ŷ�����������
		
		��ʱ��ģ�;���	Model =  Move * Rotate * Scale ����Ϊ��ξ���

		ע����淨�ߵ�ת��
			�������˷Ǿ������Ų�������ô����ת������
			Ӧ��ΪModel���ת�á�
		֤���� https://zhuanlan.zhihu.com/p/72734738 ����д��
	*/
	// 1.�������ž���
	mat ScaleMatrix = mat::identity(4);
	ScaleMatrix[0][0] = scale.x;
	ScaleMatrix[1][1] = scale.y;
	ScaleMatrix[2][2] = scale.z;

	// 2.����λ�ƾ���
	mat MoveMatrix = mat::identity(4);
	MoveMatrix[0][3] = move.x;
	MoveMatrix[1][3] = move.y;
	MoveMatrix[2][3] = move.z;

	// 3.������ת����
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