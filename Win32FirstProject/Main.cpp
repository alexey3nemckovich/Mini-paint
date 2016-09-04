#define _USE_MATH_DEFINES
#include <windows.h>
#include <tchar.h>
#include <xstring>
#include "resource.h"
#include <fstream>
#include <math.h>
#include <CommCtrl.h>

typedef std::basic_string<TCHAR, std::char_traits<TCHAR>, std::allocator<TCHAR>> String;

//global variables
TCHAR  WinName[] = _T("WinFrame");
BOOL   sessionSaved = false;
OPENFILENAMEW openFileName;
CHOOSECOLOR chooseColor;
HBRUSH hBrush;
HWND aboutDialog = NULL;
HWND lineThicknessDialog = NULL;

//functions and procedures signatures
ATOM                  RegisterWindowClass(HINSTANCE hInstance);
BOOL                  InitInstance(HINSTANCE, int);
LRESULT CALLBACK      WndProcMessages(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK      AboutDialogProc(_In_ HWND   hwndDlg, _In_ UINT   uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);
INT_PTR CALLBACK      LineThicknessDialogProc(_In_ HWND   hwndDlg, _In_ UINT   uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);
HWND                  CreateTrackbar(HWND, UINT, UINT, UINT, UINT, UINT, UINT);
void				  MenuClick(HWND hWnd, WORD menuItemID);
int					  ShowWarning(LPCWSTR lpText, LPCWSTR lpCaption);
void				  SaveFileDialog(HWND hWnd);
BOOL				  SaveFile(HWND hWnd);
void			      LoadFileDialog(HWND hWnd);
BOOL	              LoadFile(HWND hWnd);
void				  ExitApplication(HWND hWnd);
void				  InitOpenFileNameStructure(HWND hWnd);
void		          InitChooseColorStructure(HWND hWnd);
void				  ChooseColorProc();
void				  CreateDialogs();
void				  InitResources(HWND hWnd);
void				  FreeResources(HWND hWnd);

//application start point
int APIENTRY _tWinMain(HINSTANCE This, HINSTANCE prev, LPTSTR cmd, int mode)
{
	HACCEL hAccelTable;
	MSG msg;
	RegisterWindowClass(This);
	InitInstance(This, mode);
	hAccelTable = LoadAccelerators(This, MAKEINTRESOURCE(IDR_ACCELERATOR));
	//main window messages processing cicle
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return 0;
}

//main window messages processer
LRESULT CALLBACK WndProcMessages(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc = GetDC(hWnd);
	WORD wParamLowWord = LOWORD(wParam);
	WORD wParamHighWord = HIWORD(wParam);
	PAINTSTRUCT ps;
	switch (message)
	{
		//Window creating
	case WM_CREATE:
		InitResources(hWnd);
		break;
		//Timer message
	case WM_TIMER:
		break;
	case WM_SIZE:
		break;
		//window repaint
	case WM_PAINT:
		LineTo(hdc, 100, 100);
		break;
		//menu events
	case WM_COMMAND:
		MenuClick(hWnd, wParamLowWord);
		break;
		//mouse events
	case WM_RBUTTONDOWN://right mouse button down
		break;
	case WM_LBUTTONDOWN://left mouse button down
		break;
	case WM_RBUTTONUP://right mouse button up
		break;
	case WM_LBUTTONUP://left mouse button up
		break;
		//keyboard click
	case WM_CHAR:
		break;
		//destroying window
	case WM_DESTROY:
		FreeResources(hWnd);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	ReleaseDC(hWnd, hdc);
	return 0;
}

//init and register main window class 
ATOM RegisterWindowClass(HINSTANCE hInstance)
{
	WNDCLASS wc;
	wc.hInstance = hInstance;
	wc.lpszClassName = WinName;
	wc.lpfnWndProc = WndProcMessages;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hCursor = LoadCursor(NULL, IDC_CROSS);
	wc.lpszMenuName = NULL;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = CreateSolidBrush(RGB(152, 184, 222));
	return RegisterClass(&wc);
}

//creating and initialization main window 
BOOL InitInstance(HINSTANCE hInstance, int mode)
{
	HWND hWnd;
	hWnd = CreateWindow
	(
		WinName,
		_T("Mini-Paint"),
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		800,
		600,
		HWND_DESKTOP,
		NULL,
		hInstance,
		NULL
	);
	if (!hWnd)
	{
		return false;
	}
	HMENU hMenu = LoadMenu(NULL, MAKEINTRESOURCE(IDR_MENU));
	SetMenu(hWnd, hMenu);
	ShowWindow(hWnd, mode);
	UpdateWindow(hWnd);
	return true;
}

void InitOpenFileNameStructure(HWND hWnd)
{
	openFileName.lStructSize = sizeof(OPENFILENAME);
	openFileName.hwndOwner = hWnd;
	openFileName.lpstrFilter = _T("Enhanced Metafile\0*.emf\0Text File\0*.txt");
	openFileName.lpstrFile = new TCHAR[256];
	openFileName.lpstrFile[0] = '\0';
	openFileName.nMaxFile = 256;
	openFileName.lpstrInitialDir = _T(".\\");
	openFileName.lpstrDefExt = _T("emf");
}

void InitChooseColorStructure(HWND hWnd)
{
	static COLORREF arCustClr[16];
	static DWORD rgbCurrent;
	ZeroMemory(&chooseColor, sizeof(CHOOSECOLOR));
	chooseColor.lStructSize = sizeof(CHOOSECOLOR);
	chooseColor.hwndOwner = hWnd;
	chooseColor.lpCustColors = (LPDWORD)arCustClr;
	chooseColor.rgbResult = rgbCurrent;
	chooseColor.Flags = CC_FULLOPEN | CC_RGBINIT;
}

//standard choose color dialog
void ChooseColorProc()
{
	if (ChooseColor(&chooseColor) == true)
	{
		hBrush = CreateSolidBrush(chooseColor.rgbResult);
	}
}

//create dialog windows
void CreateDialogs(HWND hWnd)
{
	aboutDialog = CreateDialog(NULL, MAKEINTRESOURCE(IDD_ABOUT_DIALOG), hWnd, AboutDialogProc);
	lineThicknessDialog = CreateDialog(NULL, MAKEINTRESOURCE(IDD_LINE_THICKNESS_DIALOG), hWnd, LineThicknessDialogProc);
}

//initalization of all application resources
void InitResources(HWND hWnd)
{
	InitOpenFileNameStructure(hWnd);
	InitChooseColorStructure(hWnd);
	CreateDialogs(hWnd);
	SetTimer(hWnd, 1, 1000, NULL);
}

//release all application resources
void FreeResources(HWND hWnd)
{
	free(openFileName.lpstrFile);
	KillTimer(hWnd, 1);
	DestroyWindow(aboutDialog);
	DestroyWindow(lineThicknessDialog);
}

HWND CreateTrackbar(
	HWND hWnd,
	UINT minValue,
	UINT maxValue,
	UINT width,
	UINT height,
	UINT x,
	UINT y
	)
{
	HWND trackBar;
	//here is creating trackbar component
	trackBar = CreateWindowEx(
		0,
		TRACKBAR_CLASS, 
		_T("TrackBar Control"), 
		WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS | TBS_BOTTOM | TBS_HORZ | TBS_TOOLTIPS,
		x,
		y,
		width,
		height,
		hWnd,
		NULL,
		NULL,
		NULL
		);
	//setting trackbar values range
	SendMessage(trackBar, TBM_SETRANGE, (WPARAM)true, (LPARAM)MAKELONG(minValue, maxValue));
	//setting the default number of logical positions the trackbar's slider moves
	SendMessage(trackBar, TBM_SETPAGESIZE, 0, (LPARAM)5);
	SetFocus(trackBar);
	return trackBar;
}

//model dialog about messages processer
INT_PTR CALLBACK AboutDialogProc(
	_In_ HWND   hwndDlg,
	_In_ UINT   uMsg,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam
	)
{
	switch (uMsg)
	{
	case WM_COMMAND:
		if (LOWORD(wParam) != IDOK)
		{
			break;
		}
	case WM_CLOSE:
		ShowWindow(hwndDlg, SW_HIDE);
		break;
	default:
		return false;
	}
	return true;
}

//model dialog line thickness messages processer
INT_PTR CALLBACK LineThicknessDialogProc(
	_In_ HWND   hwndDlg, 
	_In_ UINT   uMsg, 
	_In_ WPARAM wParam, 
	_In_ LPARAM lParam
	)
{
	static HWND trackbar;
	switch (uMsg)
	{
	case WM_HSCROLL:
		break;
	case WM_INITDIALOG:
		RECT rect;
		GetWindowRect(hwndDlg, &rect);
		trackbar = CreateTrackbar(hwndDlg, 1, 30, rect.right - rect.left - 40, rect.bottom - rect.top - 100, 10, 10);
		break;
	case WM_COMMAND:
		if (LOWORD(wParam) != IDOK)
		{
			break;
		}
		//here get thickness
		SendMessage(trackbar, TBM_GETPOS, 0, 0);//result is thickness
	case WM_CLOSE:
		ShowWindow(hwndDlg, SW_HIDE);
		break;
	default:
		return false;
	}
	return true;
}

//menu click processer
void MenuClick(HWND hWnd, WORD menuItemID)
{
	switch (menuItemID)
	{
		case ID_LOAD:
			LoadFileDialog(hWnd);
			break;
		case ID_SAVE:
			SaveFileDialog(hWnd);
			break;
		case ID_EXIT:
			ExitApplication(hWnd);
			break;
		case ID_LINE_THICKNESS:
			ShowWindow(lineThicknessDialog, SW_SHOW);
			break;
		case ID_ABOUT:
			ShowWindow(aboutDialog, SW_SHOW);
			break;
		case ID_COLOR:
			ChooseColorProc();
			break;
		default:
			break;
	}
}

void ExitApplication(HWND hWnd)
{
	//if file was saved application is finishing
	//else user should see a warning dialog window
	if (sessionSaved)
	{
		ExitProcess(0);
	}
	else
	{
		int result = ShowWarning(_T("Сохранить?"), _T("Файл не сохранён!"));
		if (result == IDYES)
		{
			SaveFileDialog(hWnd);
		}
		ExitProcess(0);
	}
}

void SaveFileDialog(HWND hWnd)
{
	while (!SaveFile(hWnd))
	{
		int dialogResult = ShowWarning(_T("Повторить попытку?"), _T("Не удалось сохранить файл!"));
		if (dialogResult == IDNO)
		{
			return;
		}
	}
}

void LoadFileDialog(HWND hWnd)
{
	while (!LoadFile(hWnd))
	{
		int dialogResult = ShowWarning(_T("Повторить попытку?"), _T("Не удалось загрузить файл!"));
		if (dialogResult == IDNO)
		{
			return;
		}
	}
}

BOOL LoadFile(HWND hWnd)
{
	//if all ok function returns true
	//else false
	if (GetOpenFileNameW(&openFileName) == 0)
	{
		if (CommDlgExtendedError() == 0)
		{
			return true;
		}
		return false;
	}
	//think about after coding drawing
	return true;
}

BOOL SaveFile(HWND hWnd)
{
	if (GetSaveFileNameW(&openFileName) == 0)
	{
		if (CommDlgExtendedError() == 0)
		{
			return true;
		}
		return false;
	}
	//here is creating extended windows metafile
	//and program is writing all shapes to this file
	HENHMETAFILE hmf;
	HDC hdc = GetDC(hWnd);
	HDC hdcEMF;
	hdcEMF = CreateEnhMetaFile(hdc, openFileName.lpstrFile, NULL, NULL);
	//here writting all shapes to file
	hmf = CloseEnhMetaFile(hdcEMF);
	DeleteEnhMetaFile(hmf);
	ReleaseDC(hWnd, hdc);
	sessionSaved = true;
	return true;
}

//warning dialog
int ShowWarning(LPCWSTR lpText, LPCWSTR lpCaption)
{
	int dialogResult = MessageBox(NULL, lpText, lpCaption,
		MB_YESNO |
		MB_DEFBUTTON1 |
		MB_ICONWARNING |
		MB_DEFAULT_DESKTOP_ONLY
	);
	return dialogResult;
}