#define _USE_MATH_DEFINES
#define STATUS_BAR_PARTS_COUNT 4
#include <windows.h>
#include <tchar.h>
#include <xstring>
#include "resource.h"
#include <fstream>
#include <math.h>
#include <CommCtrl.h>
#include "DrawingProcess.h"

typedef std::basic_string<TCHAR, std::char_traits<TCHAR>, std::allocator<TCHAR>> String;

//global variables
TCHAR  WinName[] = _T("WinFrame");
BOOL   sessionSaved = false;
OPENFILENAMEW openFileName;
CHOOSECOLOR chooseColor;
int penColor;
int brushColor;
int textColor;
int penThickness;
HPEN hPen;
HPEN hTextPen;
HBRUSH hBrush;
HWND aboutDialog = NULL;
HWND lineThicknessDialog = NULL;
HWND statusBar = NULL;
DrawingProcess *drawingProcess;
DRAWING_OBJECTS currentShapeType;
HCURSOR crossCursor;
HCURSOR handCursor;
bool zoom = false;

//functions and procedures signatures
ATOM                  RegisterWindowClass(HINSTANCE hInstance);
BOOL                  InitInstance(HINSTANCE, int);
LRESULT CALLBACK      WndProcMessages(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK      AboutDialogProc(_In_ HWND   hwndDlg, _In_ UINT   uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);
INT_PTR CALLBACK      LineThicknessDialogProc(_In_ HWND   hwndDlg, _In_ UINT   uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);
HWND                  CreateTrackbar(HWND, UINT, UINT, UINT, UINT, UINT, UINT);
HWND				  CreateStatusBar(HWND, int, HINSTANCE, int);
int					  ShowWarning(LPCWSTR lpText, LPCWSTR lpCaption);
void				  SaveFileDialog(HWND hWnd);
BOOL				  SaveFile(HWND hWnd);
void			      LoadFileDialog(HWND hWnd);
BOOL	              LoadFile(HWND hWnd);
void				  ExitApplication(HWND hWnd);
void				  InitOpenFileNameStructure(HWND hWnd);
void		          InitChooseColorStructure(HWND hWnd);
int					  ChooseColorProc();
void				  CreateDialogs(HWND hWnd);
void				  InitResources(HWND hWnd);
void				  FreeResources(HWND hWnd);
//handlers
void				  MenuClick(HWND hWnd, WORD menuItemID);
void				  LeftButtonDown(HWND hWnd, POINT mousePos);
void				  LeftButtonUp(HWND hWnd, POINT mousePos);
void				  MouseMove(HWND hWnd, POINT prevMousePos, POINT currentMousePos);
void				  MouseWheel(HWND hWnd, WPARAM wParam);

//application start point
int APIENTRY _tWinMain(HINSTANCE This, HINSTANCE prev, LPTSTR cmd, int mode)
{
	HACCEL hAccelTable;
	MSG msg;
	crossCursor = LoadCursor(NULL, IDC_CROSS);
	handCursor = LoadCursor(NULL, IDC_HAND);
	RegisterWindowClass(This);
	InitInstance(This, mode);
	SetCursor(crossCursor);
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
	WORD wParamLowWord = LOWORD(wParam);
	WORD wParamHighWord = HIWORD(wParam);
	WORD lParamLowWord = LOWORD(lParam);
	WORD lParamHighWord = HIWORD(lParam);
	PAINTSTRUCT ps;
	static POINT mousePos;
	switch (message)
	{
		//Window creating
		case WM_CREATE:
		{
			InitResources(hWnd);
		}
		break;
		//mouse events
		case WM_RBUTTONDOWN://right mouse button down
		{
			if (drawingProcess->isDrawing())
			{
				drawingProcess->stopDrawingCurrentShape();
			}
		}
		break;
		case WM_LBUTTONDOWN://left mouse button down
		case WM_LBUTTONUP:
		{
			mousePos.x = lParamLowWord;
			mousePos.y = lParamHighWord;
			if ((GetKeyState(VK_LBUTTON) & 0x100) != 0)
			{
				LeftButtonDown(hWnd, mousePos);
			}
			else
			{
				LeftButtonUp(hWnd, mousePos);
			}
		}
		break;
		case WM_MOUSEMOVE://mouse move
		{
			POINT currentMousePos;
			currentMousePos.x = lParamLowWord;
			currentMousePos.y = lParamHighWord;
			MouseMove(hWnd, mousePos, currentMousePos);
			mousePos = currentMousePos;
		}
		break;
		case WM_MBUTTONDOWN:
		{
			mousePos.x = lParamLowWord;
			mousePos.y = lParamHighWord;
			SetCursor(handCursor);
			drawingProcess->setWorkingMode(MOVING);
		}
		break;
		case WM_MBUTTONUP:
		{
			SetCursor(crossCursor);
			drawingProcess->setWorkingMode(DRAWING);
		}
		break;
		case WM_MOUSEWHEEL:
		{
			MouseWheel(hWnd, wParam);
		}
		break;
		//window repaint
		case WM_PAINT:
		{
			GetCursorPos(&mousePos);
			ScreenToClient(hWnd, &mousePos);
			drawingProcess->drawing(mousePos);
		}
		break;
		//keyboard click
		case WM_CHAR:
		{
			if (drawingProcess->isDrawing() && (currentShapeType == TEXT))
			{
				if (wParam != 13)
				{
					drawingProcess->drawingText(wParam);
					InvalidateRect(hWnd, NULL, true);
				}
				else
				{
					drawingProcess->stopDrawingCurrentShape();
				}
			}
		}
		break;
		case WM_SIZE:
		{

			int* rightEdgeXCoordinates = (int*)malloc(sizeof(int) * STATUS_BAR_PARTS_COUNT);
			RECT windowRect;
			GetClientRect(hWnd, &windowRect);
			int statusBarPartWidth = (windowRect.right - windowRect.left) / STATUS_BAR_PARTS_COUNT;
			int prevRightEdgeXCoordinate = windowRect.left;
			for (int i = 0; i < STATUS_BAR_PARTS_COUNT; i++)
			{
				rightEdgeXCoordinates[i] = prevRightEdgeXCoordinate + statusBarPartWidth;
				prevRightEdgeXCoordinate = rightEdgeXCoordinates[i];
			}
			SendMessage(statusBar, SB_SETPARTS, (WPARAM)STATUS_BAR_PARTS_COUNT, (LPARAM)rightEdgeXCoordinates);
			SetWindowPos(statusBar, NULL, 0, 0, 0, 0, SWP_SHOWWINDOW);
		}
		break;
		//menu events
		case WM_COMMAND:
		{
			drawingProcess->setWorkingMode(DRAWING);
			MenuClick(hWnd, wParamLowWord);
		}
		break;
		case WM_CLOSE:
		{
			ExitApplication(hWnd);
		}
		break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void LeftButtonDown(HWND hWnd, POINT mousePos)
{
	switch (drawingProcess->getWorkingMode())
	{
		case DRAWING:
		{
			drawingProcess->startOrContinueDrawingShape(mousePos, currentShapeType, hPen, hBrush);
		}
		break;
		default:
			break;
	}
}

void LeftButtonUp(HWND hWnd, POINT mousePos)
{
	switch (drawingProcess->getWorkingMode())
	{
		case DRAWING:
		{
			if (drawingProcess->isDrawing() && (drawingProcess->getDrawingType() == UNSEPARATED))
			{
				drawingProcess->stopDrawingCurrentShape();
			}
		}
		break;
		default:
			break;
	}
}

void MouseMove(HWND hWnd, POINT prevMousePos, POINT currentMousePos)
{
	if (drawingProcess->isDrawing())
	{
		InvalidateRect(hWnd, NULL, true);
	}
	else
	{
		if (drawingProcess->getWorkingMode() == MOVING)
		{
			if ((GetKeyState(VK_MBUTTON) & 0x100) != 0)
			{
				bool altPushed = (GetKeyState(VK_MENU) & 0x100) != 0;
				bool ctrlPushed = (GetKeyState(VK_CONTROL) & 0x100) != 0;
				bool shiftPushed = (GetKeyState(VK_SHIFT) & 0x100) != 0;
				if ((!shiftPushed && !altPushed && !ctrlPushed) || (shiftPushed && !altPushed && !ctrlPushed))
				{
					POINT prevOrigin = drawingProcess->getDrawingOrigin();
					POINT newOrigin;
					newOrigin.x = prevOrigin.x + (currentMousePos.x - prevMousePos.x);
					newOrigin.y = prevOrigin.y + (currentMousePos.y - prevMousePos.y);
					drawingProcess->setOriginTo(newOrigin);
					InvalidateRect(hWnd, NULL, true);
				}
			}
		}
	}
}

void MouseWheel(HWND hWnd, WPARAM wParam)
{
	WORD wParamLowWord = LOWORD(wParam);
	if ((wParamLowWord == MK_CONTROL) || (wParamLowWord == (MK_CONTROL | MK_SHIFT)))
	{
		WORD wheelDestiny = HIWORD(wParam);
		if (wheelDestiny > 65000)
		{
			drawingProcess->zoomIn(0.5);
		}
		else
		{
			drawingProcess->zoomIn(2);
		}
	}
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
	wc.hCursor = NULL;
	wc.lpszMenuName = NULL;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = NULL;
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
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,
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
	statusBar = CreateStatusBar(hWnd, NULL, hInstance, 10);
	ShowWindow(hWnd, SW_MAXIMIZE);
	UpdateWindow(hWnd);
 	HDC hdc = GetDC(hWnd);
	SetGraphicsMode(hdc, GM_ADVANCED);
	SetMapMode(hdc, MM_LOENGLISH);
	ReleaseDC(hWnd, hdc);
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
	chooseColor.Flags = CC_RGBINIT | CC_PREVENTFULLOPEN;
}

//standard choose color dialog
int ChooseColorProc()
{
	if (ChooseColor(&chooseColor) == true)
	{
		return chooseColor.rgbResult;
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
	drawingProcess = new DrawingProcess(hWnd);
	penThickness = 0;
	penColor = RGB(0, 0, 0);
	brushColor = RGB(255, 255, 255);
	hPen = CreatePen(PS_SOLID, penThickness, penColor);
	hBrush = CreateSolidBrush(brushColor);
	currentShapeType = MULTI_LINE;
}

//release all application resources
void FreeResources(HWND hWnd)
{
	delete(drawingProcess);
	free(openFileName.lpstrFile);
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

HWND CreateStatusBar(
	HWND hWnd,
	int idStatus,
	HINSTANCE hinst,
	int cParts
)
{
	HWND statusBar;
	statusBar = CreateWindowEx(
		0,
		STATUSCLASSNAME,
		NULL,
		WS_CHILD | WS_VISIBLE,
		0, 0, 0, 0,
		hWnd,
		(HMENU)idStatus,
		hinst,
		NULL
	);
	int* rightEdgeXCoordinates = (int*)malloc(sizeof(int) * cParts);
	RECT windowRect;
	GetClientRect(hWnd, &windowRect);
	int statusBarPartWidth = (windowRect.right - windowRect.left) / cParts;
	int prevRightEdgeXCoordinate = windowRect.left;
	for (int i = 0; i < cParts; i++)
	{
		rightEdgeXCoordinates[i] = prevRightEdgeXCoordinate + statusBarPartWidth;
		prevRightEdgeXCoordinate = rightEdgeXCoordinates[i];
	}
	SendMessage(statusBar, SB_SETPARTS, (WPARAM)cParts, (LPARAM)rightEdgeXCoordinates);
	return statusBar;
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
		penThickness = SendMessage(trackbar, TBM_GETPOS, 0, 0);
		hPen = CreatePen(PS_SOLID, penThickness, penColor);
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
		case ID_LINE:
		{
			currentShapeType = LINE;
		}
		break;
		case ID_BROKEN_LINE:
		{
			currentShapeType = BROKEN_LINE;
		}
		break;
		case ID_MULTILINE:
		{
			currentShapeType = MULTI_LINE;
		}
		break;
		case ID_RECTANGLE:
		{
			currentShapeType = RECTANGLE;
		}
		break;
		case ID_POLYGON:
		{
			currentShapeType = POLYGON;
		}
		break;
		case ID_ELLIPSE:
		{
			currentShapeType = ELLIPSE;
		}
		break;
		case ID_TEXT:
		{
			currentShapeType = TEXT;
		}
		break;
		case ID_LOAD:
		{
			LoadFileDialog(hWnd);
		}
		break;
		case ID_SAVE:
		{
			SaveFileDialog(hWnd);
		}
		break;
		case ID_EXIT:
		{
			ExitApplication(hWnd);
		}
		break;
		case ID_LINE_THICKNESS:
		{
			ShowWindow(lineThicknessDialog, SW_SHOW);
		}
		break;
		case ID_ABOUT:
		{
			ShowWindow(aboutDialog, SW_SHOW);
		}
		break;
		case ID_PEN_COLOR:
		{
			penColor = ChooseColorProc();
			hPen = CreatePen(PS_SOLID, penThickness, penColor);
		}
		break;
		case ID_BRUSH_COLOR:
		{
			brushColor = ChooseColorProc(); 
			hBrush = CreateSolidBrush(brushColor);
		}
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
		FreeResources(hWnd);
		PostQuitMessage(0);
	}
	else
	{
		int result = ShowWarning(_T("Сохранить?"), _T("Файл не сохранён!"));
		if (result == IDYES)
		{
			SaveFileDialog(hWnd);
		}
		FreeResources(hWnd);
		PostQuitMessage(0);
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
	HDC hdc = GetDC(hWnd);
	HENHMETAFILE hemf = GetEnhMetaFileW(openFileName.lpstrFile);
	//
	drawingProcess->setLoadedFile(hemf);
	InvalidateRect(hWnd, NULL, true);
	//RECT rect;
	//GetClientRect(hWnd, &rect);
	//PlayEnhMetaFile(hdc, hemf, &rect);
	//
	//DeleteEnhMetaFile(hemf);
	ReleaseDC(hWnd, hdc);
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
	HENHMETAFILE hmf;
	HDC hdc = GetDC(hWnd);
	HDC hdcEMF;
	hdcEMF = CreateEnhMetaFile(hdc, openFileName.lpstrFile, NULL, NULL);
	drawingProcess->drawToFile(hdcEMF);
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