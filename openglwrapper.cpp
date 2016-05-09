
#include "includes.hpp"

///>====================================----------------------------
///
///	i m p l e m e n t a c j a
///
///>====================================----------------------------

WNDCLASSEX			g_WC;
PIXELFORMATDESCRIPTOR		g_pfd;
HDC				g_hDC;
HWND				g_hWnd;
HGLRC				g_hGLRC;

void ResizeWindow(GLsizei width, GLsizei height)
{
	if (height == 0)										// Prevent A Divide By Zero By
		height = 1;										// Making Height Equal One
	glViewport(0, 0, width, height);						// Reset The Current Viewport
	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix

	/// skalowanie (wspólczynnik ekranu)...
	gluPerspective(45.0f, (GLfloat)width/(GLfloat)height, 0.1f, 100.0f);
	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glLoadIdentity();									// Reset The Modelview Matrix

	::SwapBuffers(g_hDC);
};

////////////////////////////////////////////////////////////////////

void SwapBuf()
{
	::SwapBuffers(g_hDC);
}

////////////////////////////////////////////////////////////////////

void InitOpenGL(SWrapper &wrapper, HWND hParWnd)
{
	GLuint		PixelFormat;
	DWORD		dwExStyle;
	DWORD		dwStyle;
	RECT		WindowRect;

        g_WC.cbSize         = sizeof(WNDCLASSEX);
        g_WC.style          = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
        g_WC.lpfnWndProc    = wrapper.lpfnProc;
        g_WC.cbClsExtra     = 0;
        g_WC.cbWndExtra     = 0;
        g_WC.hInstance      = wrapper.hInst;
        g_WC.hIcon          = LoadIcon(wrapper.hInst, MAKEINTRESOURCE(IDI_IKONA));
        g_WC.hCursor        = LoadCursor(NULL, IDC_ARROW);
        g_WC.hbrBackground  = NULL;
        g_WC.lpszMenuName   = NULL;
        g_WC.lpszClassName  = wrapper.szTitle;
        g_WC.hIconSm        = NULL;

        if (!RegisterClassEx(&g_WC))
        	throw CException("InitOpenGL: RegisterClassEx()");

	WindowRect.left		= (long)0;
	WindowRect.right	= (long)wrapper.nWidth;
	WindowRect.top		= (long)0;
	WindowRect.bottom	= (long)wrapper.nHeight;

	dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;

	if (wrapper.bFullS) {

		DEVMODE		dmSettings;

		dwStyle = WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
		ZeroMemory(&dmSettings, sizeof(dmSettings));

		if (!EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dmSettings))
			throw CException("InitOpenGL: EnumDisplaySettings");

		dmSettings.dmPelsWidth	= wrapper.nWidth;
		dmSettings.dmPelsHeight	= wrapper.nHeight;

		int result = ChangeDisplaySettings(&dmSettings, CDS_FULLSCREEN);

		if (result != DISP_CHANGE_SUCCESSFUL)
			throw CException("InitOpenGL: ChangeDisplaySettings");

		::ShowCursor(FALSE);

	} else {
		dwStyle	= WS_SYSMENU|WS_CAPTION|WS_BORDER|WS_OVERLAPPED|WS_VISIBLE|WS_MINIMIZEBOX|WS_POPUP;
	}

	/// ustaw parametry okna:
	if (!AdjustWindowRectEx(&WindowRect, dwStyle, false, dwExStyle))
		throw CException("InitOpenGL: AdjustWindowRectEx");

	/// stwórz okno:
	if (!(g_hWnd = ::CreateWindowEx(
		dwExStyle,
		wrapper.szTitle,
		wrapper.szTitle,
		dwStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		0, 0,
		WindowRect.right - WindowRect.left,
		WindowRect.bottom - WindowRect.top,
		hParWnd,
		NULL, /// bez menu
		wrapper.hInst, NULL)))
		throw CException("InitOpenGL: CreateWindowEx");

	/// ustaw odpowiednio 'PIXELFORMATDESCRIPTOR':
	ZeroMemory(&g_pfd, sizeof(PIXELFORMATDESCRIPTOR));
	g_pfd.nSize	= sizeof(PIXELFORMATDESCRIPTOR);
	g_pfd.nVersion	= 1;
	g_pfd.dwFlags	= PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	g_pfd.iPixelType = PFD_TYPE_RGBA;
	g_pfd.cColorBits = wrapper.nBPP;
	g_pfd.cDepthBits = 16; /// 16-bitowy z-bufor
	g_pfd.iLayerType = PFD_MAIN_PLANE;

	if (!(g_hDC = GetDC(g_hWnd)))
		throw CException("Nie mogê utworzyæ Device Context.");

	if (!(PixelFormat = ChoosePixelFormat(g_hDC, &g_pfd)))
		throw CException("Ten format pixeli nie pasuje do aktualnego DC.");

	if (!SetPixelFormat(g_hDC, PixelFormat, &g_pfd))
		throw CException("Nie mo¿na ustawiæ tego formatu pixeli.");

	if (!(g_hGLRC = wglCreateContext(g_hDC)))
		throw CException("Nie mogê utworzyæ GL Rendering Context.");

	if (!wglMakeCurrent(g_hDC, g_hGLRC))
		throw CException("Nie mogê uaktywniæ GL Rendering Context.");

	::ShowWindow(g_hWnd, SW_SHOW);
	::SetForegroundWindow(g_hWnd);
	::SetFocus(g_hWnd);

	ResizeWindow(wrapper.nWidth, wrapper.nHeight);
}

////////////////////////////////////////////////////////////////////

void DeInitOpenGL(SWrapper &wrapper)
{
	wglMakeCurrent(NULL, NULL);

	::ReleaseDC(g_hWnd, g_hDC);

	if (!wglDeleteContext(g_hGLRC))
		throw CException("Nie mogê skasowaæ GL Renderding Contextu.");

	::UnregisterClass(wrapper.szTitle, wrapper.hInst);

	if (wrapper.bFullS)
		::ShowCursor(TRUE);
}



