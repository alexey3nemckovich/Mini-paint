#ifndef DRAWING_PROCESS_CLASS_INTERFACE
#define DRAWING_PROCESS_CLASS_INTERFACE
#include <windows.h>
#include <vector>
#include "DrawingObject.h"

enum WORKING_MODE
{
	DRAWING,
	MOVING,
	SELECTING_AREA,
};

class DrawingProcess
{
	private:
		HWND hWnd;
		WORKING_MODE workingMode;
		bool isDrawingNow = false;
		HPEN hPen;
		HBRUSH hBrush;
		POINT prevOrigin;
		POINT origin;
		DrawingObject* createCurrentShape(DRAWING_OBJECTS, HPEN, HBRUSH);
		void recalcCoordinates();
		DrawingObject *currentShape;
		vector<DrawingObject*> shapes;
		void drawToHDC(HDC, POINT);
		void redrawAllDrawnShapes(HDC);
		float zoom;
		HENHMETAFILE loadedFile;
		void clearShapesList();
	public:
		void setLoadedFile(HENHMETAFILE file);
		void setOriginTo(POINT origin);
		void zoomIn(float zoom);
		POINT getDrawingOrigin();
		void setWorkingMode(WORKING_MODE workingMode);
		WORKING_MODE getWorkingMode();
		DrawingProcess(HWND);
		bool isDrawing();
		void startOrContinueDrawingShape(POINT, DRAWING_OBJECTS, HPEN, HBRUSH);
		void drawToFile(HDC);
		void drawToFile(HDC, RECT);
		void drawing(POINT);
		void drawingText(DWORD);
		void stopDrawingCurrentShape();
		~DrawingProcess();
		DRAWING_TYPE getDrawingType();
		DrawingObject* popLastDrawingObject();
};
#endif