

#ifndef __openglwrapper_hpp__
#define __openglwrapper_hpp__

struct SWrapper
{
	bool		bFullS;
	int		nWidth, nHeight, nBPP;
	char		*szTitle;

	WNDPROC		lpfnProc; /// window procedure
	HINSTANCE	hInst; /// uchwyt instancji

	~SWrapper()
	{
		DELETE_STRING(szTitle);
	}
};


void DeInitOpenGL(SWrapper &);
void ResizeWindow(GLsizei width, GLsizei height);
void InitOpenGL(SWrapper &, HWND);
void SwapBuf();

#endif

