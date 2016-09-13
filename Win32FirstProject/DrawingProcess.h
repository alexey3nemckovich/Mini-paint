#ifndef DRAWING_PROCESS_CLASS_INTERFACE
#define DRAWING_PROCESS_CLASS_INTERFACE
#include <windows.h>
#include <vector>
#include "DrawingObject.h"
#include "DrawingObjectFactory.h"
#include "DrawingObjectFactoriesIncludeList.h"

enum WORKING_MODE
{
	DRAWING,
	MOVING,
	SELECTING_AREA,
};

class DrawingProcess
{
	private:
		HPEN drawingPen;
		HBRUSH drawingBrush;
		DrawingObjectFactory *drawingObjectFactory;
		HWND hWnd;
		WORKING_MODE workingMode;
		vector<WORKING_MODE> workingModesHistory;
		bool isDrawingNow = false;
		HPEN hPen;
		HBRUSH hBrush;
		POINT prevOrigin;
		POINT origin;
		void setCurrentDrawingObjectFactory(DRAWING_OBJECTS);
		void recalcCoordinates();
		DrawingObject *currentShape;
		vector<DrawingObject*> shapes;
		void drawToHDC(HDC, POINT);
		void redrawAllDrawnShapes(HDC);
		float zoom;
		HENHMETAFILE loadedFile;
		void clearShapesList();
		DRAWING_OBJECTS currentDrawingObjectType;
	public:
		void setDrawingPen(HPEN hPen);
		void setDrawingBrush(HBRUSH hBrush);
		void setLoadedFile(HENHMETAFILE file);
		void setOriginTo(POINT origin);
		void setCurrentDrawingObjectsType(DRAWING_OBJECTS currentDrawingObjectsType);
		void switchToPrevWorkingMode();
		DRAWING_OBJECTS getCurrentDrawingObjectsType();
		void zoomIn(float zoom);
		float getZoom();
		POINT getDrawingOrigin();
		void setWorkingMode(WORKING_MODE workingMode);
		WORKING_MODE getWorkingMode();
		DrawingProcess(HWND);
		bool isDrawing();
		void startOrContinueDrawingShape(POINT, HPEN, HBRUSH);
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