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