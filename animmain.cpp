
#include "resource.h"
#include "includes.hpp"

///>====================================----------------------------
///
///	p r o t o t y p y
///
///>====================================----------------------------
int CALLBACK SetupDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
int CALLBACK AboutDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK MainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK winproc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

char PlayAnimDlgInit(void);


///>====================================----------------------------
///
///	z m i e n n e   g l o b a l n e
///
///>====================================----------------------------
HWND		g_HWndSetup;
SWrapper	g_WindowParams; /// modyfikowana przez SetupDlgProc
DWORD		g_StartTime;
bool		g_Keys[256]; /// stan klawiszy

RECT		g_NoPicture = {0, 0, 350, 250};
BOOL		g_bPicture;

CScene		g_Scene;

char		*g_lpszProgName = NULL;
char		*g_lpszNoFileProgName = "Animacja 3D";

HWND		MainhWnd;

HINSTANCE	g_hInstance;


///>====================================----------------------------
/// function:	   SetupDlgProc
/// description:   obs³uguje okno dialogowe z wyborem rozdzielczoœci
///>====================================----------------------------
int CALLBACK SetupDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
	case WM_INITDIALOG:

		/// IDC_RADIO2 - 640 x 480
		/// IDC_RADIO5 - full screen
		/// IDC_RADIO6 - windowed
		/// IDC_RADIO7 - 16 bpp
		/// IDC_RADIO8 - 32 bpp
		SendDlgItemMessage(hwnd, IDC_RADIO2, BM_SETCHECK, BST_CHECKED, 0);
		SendDlgItemMessage(hwnd, IDC_RADIO6, BM_SETCHECK, BST_CHECKED, 0);
		SendDlgItemMessage(hwnd, IDC_RADIO8, BM_SETCHECK, BST_CHECKED, 0);
		return 1;

	case WM_COMMAND: {
		switch (LOWORD(wParam)) {

		case IDOK:

			if (SendDlgItemMessage(hwnd, IDC_RADIO1, BM_GETCHECK, 0, 0) == BST_CHECKED) {
				g_WindowParams.nWidth = 320;
				g_WindowParams.nHeight = 240;
			}
			else if (SendDlgItemMessage(hwnd, IDC_RADIO2, BM_GETCHECK, 0, 0) == BST_CHECKED) {
				g_WindowParams.nWidth = 640;
				g_WindowParams.nHeight = 480;
			}
			else if (SendDlgItemMessage(hwnd, IDC_RADIO3, BM_GETCHECK, 0, 0) == BST_CHECKED) {
				g_WindowParams.nWidth = 800;
				g_WindowParams.nHeight = 600;
			}
			else if (SendDlgItemMessage(hwnd, IDC_RADIO4, BM_GETCHECK, 0, 0) == BST_CHECKED) {
				g_WindowParams.nWidth = 1024;
				g_WindowParams.nHeight = 768;
			}

			if (SendDlgItemMessage(hwnd, IDC_RADIO5, BM_GETCHECK, 0, 0) == BST_CHECKED)
				g_WindowParams.bFullS = true;
			else
				g_WindowParams.bFullS = false;

			if (SendDlgItemMessage(hwnd, IDC_RADIO7, BM_GETCHECK, 0, 0) == BST_CHECKED)
				g_WindowParams.nBPP = 16;
			else
				g_WindowParams.nBPP = 32;

			EndDialog(hwnd, 1);
			break;

		case IDCANCEL: EndDialog(hwnd, 0); break;
		default: break;

		}
		} break;

	default: break;
    	}
    	return 0;
}

///>====================================----------------------------
/// function:	   MainWndProc
/// description:   obs³uga ca³ego programu
///>====================================----------------------------
LRESULT CALLBACK MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
        switch (uMsg) {

/*	case WM_CREATE:
///		PtrDA_OGL->OnCreate();
		return 0;
		break;

	case WM_PAINT:
///		PtrDA_OGL->OnPaint();
		return 0;
		break;

	case WM_SIZE:
///		PtrDA_OGL->Resize(LOWORD(lParam), HIWORD(lParam));
		return 0;
		break;*/

	case WM_KEYDOWN:
		//g_Keys[wParam] = true;
		if (wParam == VK_ESCAPE)
			DestroyWindow(hWnd);
			///::PostQuitMessage(0);
		break;

	case WM_KEYUP:
		//g_Keys[wParam] = false;
		break;

	case WM_DESTROY:
		::PostQuitMessage(0);
		break;

	default:
		break;
	}

	return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
}


///>====================================----------------------------
///
///	W i n M a i n
///
///>====================================----------------------------

////////////////////////////////////////////////////////////////////

void PlayAnimation()
{
	MSG	main_msg;
	g_StartTime = 0;

	ZeroMemory(&main_msg, sizeof(main_msg));

	while (main_msg.message != WM_QUIT)
	{
		if (::PeekMessage(&main_msg, NULL, 0, 0, PM_NOREMOVE))
		{
			if (::GetMessage(&main_msg, NULL, 0, 0))
			{
				::TranslateMessage(&main_msg);
				::DispatchMessage(&main_msg);
			}
		}

		if (!g_StartTime)
			g_StartTime = GetTickCount();
		float time = (float)(GetTickCount() - g_StartTime)/1000.0f;

		if (time>=(g_Scene.dwFrameStop-g_Scene.dwFrameStart+1)/33.3f)
			g_StartTime = GetTickCount();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		g_Scene.RenderFrame( 33.3f*time );
		SwapBuf();
	}

}

////////////////////////////////////////////////////////////////////

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow)
{
	
	WNDCLASSEX	glowne_okienko;
	MSG		msg;

	g_hInstance = hInst;
	g_lpszProgName = new char[strlen(g_lpszNoFileProgName) + 1];
	strcpy(g_lpszProgName, g_lpszNoFileProgName);
	
	// trzeba zainicjowac nasze okienko poczatkowymi wartosciami:
	glowne_okienko.cbSize		= sizeof(WNDCLASSEX);
	glowne_okienko.style		= CS_DBLCLKS;
	glowne_okienko.lpfnWndProc	= &winproc;
	glowne_okienko.cbClsExtra	= 0;
	glowne_okienko.cbWndExtra	= 0;
	glowne_okienko.hInstance	= hInst;
	glowne_okienko.hIcon		= LoadIcon(hInst, MAKEINTRESOURCE(IDI_IKONA));
	glowne_okienko.hCursor		= LoadCursor(NULL, IDC_ARROW);
	glowne_okienko.hbrBackground	= GetSysColorBrush(COLOR_APPWORKSPACE);
	glowne_okienko.lpszMenuName	= MAKEINTRESOURCE(IDR_MENU1);
	glowne_okienko.lpszClassName	= g_lpszProgName;
	glowne_okienko.hIconSm		= NULL;
	if (!::RegisterClassEx(&glowne_okienko))
		return 0;

	RECT	rokno(g_NoPicture);
	DWORD	style = WS_SYSMENU|WS_CAPTION|WS_BORDER|WS_OVERLAPPED|
			   WS_VISIBLE|WS_MINIMIZEBOX;

	g_bPicture = FALSE;
	g_WindowParams.nWidth	= 640;
	g_WindowParams.nHeight	= 480;
	g_WindowParams.nBPP	= 32;
	g_WindowParams.bFullS	= true;

	MainhWnd = ::CreateWindow(g_lpszProgName, g_lpszProgName,
	    style,
	    rokno.left, rokno.top, rokno.right, rokno.bottom,
	    NULL,
	    NULL,
	    hInst,
	    NULL);

	::ShowWindow(MainhWnd,nCmdShow);
	::UpdateWindow(MainhWnd);

	while (::GetMessage(&msg, NULL, 0, 0)) {
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}
	return(msg.wParam);
} /// WinMain

////////////////////////////////////////////////////////////////////

LRESULT CALLBACK winproc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	MENUITEMINFO	mii;
	HMENU		hMenu = GetMenu(hWnd);
	OPENFILENAME	ofn;
	char		path[MAX_PATH];
	HCURSOR		hcur;

	switch (uMsg) {

	case WM_COMMAND:
		switch (LOWORD(wParam)) {

		case ID_MENU_FILE_OPEN:		// wczytywanie pliku
			memset(&ofn, 0, sizeof(ofn));
			memset(path, 0, MAX_PATH);
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = hWnd;
			ofn.lpstrFile = path;
			ofn.nMaxFile = MAX_PATH;
			ofn.lpstrFilter = "3D Studio R4 (*.3ds)\0*.3ds\0\0";
			ofn.lpstrTitle = NULL;
			ofn.Flags = OFN_EXPLORER | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
			ofn.lpstrInitialDir = ".\\";

			if (::GetOpenFileName(&ofn)) {
				CLoader3DS	load_3ds(g_Scene);

				hcur = ::SetCursor(LoadCursor(NULL, IDC_WAIT)); 

				load_3ds.Load(path);
				if (!g_Scene.SetActiveCamera("Camera01"))
					g_Scene.SetFirstCamera();

				::SetCursor(hcur);

				/// ustaw odpowiednio menu systemowe:
				::ZeroMemory(&mii, sizeof(mii));
				mii.cbSize = sizeof(mii);
				mii.fMask = MIIM_STATE;
				mii.fState = MFS_ENABLED;
				::SetMenuItemInfo(hMenu, ID_MENU_VIEW_ANIMATION, FALSE, &mii);
				::SetMenuItemInfo(hMenu, ID_MENU_FILE_CLOSE, FALSE, &mii);

				/// dopisz do titlebar'u nazwê otwartego pliku:
				char *foo = new char [strlen(g_lpszNoFileProgName) + strlen(path + ofn.nFileOffset) + 4];
				strcpy(foo, g_lpszNoFileProgName);
				strcat(foo, " - ");
				strcat(foo, path + ofn.nFileOffset);
				DELETE_ARRAY(g_lpszProgName);
				g_lpszProgName = foo;
				::SetWindowText(hWnd, g_lpszProgName);
				
				/// tutaj poœrednio ustawienie odpowiednie nazwu dla okienka z animacj¹:
				DELETE_ARRAY(g_WindowParams.szTitle);
				g_WindowParams.szTitle = strdup(path + ofn.nFileOffset);
			}

			break;

		case ID_MENU_FILE_CLOSE: {
			g_Scene.Release();

			/// wygaœ odpowiednie opcje w menu:
			ZeroMemory(&mii, sizeof(mii));
			mii.cbSize = sizeof(mii);
			mii.fMask = MIIM_STATE;
			mii.fState = MFS_GRAYED;
			::SetMenuItemInfo(hMenu, ID_MENU_VIEW_ANIMATION, FALSE, &mii);
			::SetMenuItemInfo(hMenu, ID_MENU_FILE_CLOSE, FALSE, &mii);

			/// umieœæ w titlebarze sam¹ nazwê aplikacji:
			DELETE_ARRAY(g_lpszProgName);
			g_lpszProgName = strdup(g_lpszNoFileProgName);
			::SetWindowText(hWnd, g_lpszProgName);
			} 
			break;

		case ID_MENU_FILE_EXIT:
			g_Scene.Release();
			::SendMessage(hWnd, WM_CLOSE, 0, 0);
			break;

		case ID_MENU_VIEW_ANIMATION: {

			if (!DialogBox(g_hInstance, MAKEINTRESOURCE(IDD_DIALOG1), MainhWnd, (DLGPROC)SetupDlgProc))
				return 0;

			if (g_WindowParams.szTitle == NULL)
				g_WindowParams.szTitle = strdup("test");
			g_WindowParams.lpfnProc = MainWndProc;
			g_WindowParams.hInst = g_hInstance;

			try {
				if (!g_Scene.SetActiveCamera("Camera01"))
					g_Scene.SetFirstCamera();

				::EnableWindow(hWnd, FALSE);

				InitOpenGL(g_WindowParams, hWnd);

				g_Scene.SetScreenAspect((float)g_WindowParams.nWidth/g_WindowParams.nHeight);

				//g_Scene.PrintInfo(LOG);

				g_Scene.PrepareAnimation();
				g_Scene.InitTextures();
				g_Scene.InitDisplayLists();
				PlayAnimation();
				g_Scene.DeinitDisplayLists();
				g_Scene.DeinitTextures();

				DeInitOpenGL(g_WindowParams);

				::EnableWindow(hWnd, TRUE);
				::SetActiveWindow(hWnd);


			} catch (CException ex) {
				if (ex.type == CException::EXCEPTION_STRING) {
					static char *error_msg = strdup(ex.error_string);
					fatal_error(error_msg);
				} else {
					char	buffer[10];
					_itoa(ex.error, buffer, 10);
					fatal_error(buffer);
				}
			} catch (...) {
				fatal_error("Unknown error!");
			}
			
		} break;

		case ID_MENU_HELP_ABOUT:
			::DialogBox(g_hInstance, MAKEINTRESOURCE(IDD_ABOUTBOX), MainhWnd, (DLGPROC)AboutDlgProc);
		break;

		default:
			break;
		}

		break;
		
	case WM_DESTROY:
		::PostQuitMessage(0);
		break;

	case WM_CREATE:
		ZeroMemory(&mii, sizeof(mii));
		mii.cbSize = sizeof(mii);
		mii.fMask = MIIM_STATE;
		mii.fState = MFS_GRAYED;
		::SetMenuItemInfo(hMenu, ID_MENU_VIEW_ANIMATION, FALSE, &mii);
		::SetMenuItemInfo(hMenu, ID_MENU_FILE_CLOSE, FALSE, &mii);
		break;
		
	default:
		return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
		break;
	}
	return 0;
}

///>====================================----------------------------
/// function:	   AboutDlgProc
/// description:   obs³uguje okno dialogowe z informacjami o programie
///>====================================----------------------------
int CALLBACK AboutDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {

	case WM_COMMAND: {
		switch (LOWORD(wParam)) {

		case ID_ABOUT_OK:
			::EndDialog(hwnd, 1);
			break;

		default: break;

		}
		} break;

	case WM_CLOSE:
		::EndDialog(hwnd, 1);
		break;

	default: break;
    	}
    	return 0;
}
