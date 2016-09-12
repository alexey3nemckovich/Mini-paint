#include "DrawingProcess.h"
#include "DrawingObjectsIncludeList.h"
#include <Windows.h>

DrawingProcess::DrawingProcess(HWND hWnd)
{
	this->hWnd = hWnd;
	origin.x = 0;
	origin.y = 0;
	prevOrigin.x = 0;
	prevOrigin.y = 0;
	workingMode = DRAWING;
	zoom = 1;
}

DrawingProcess::~DrawingProcess()
{
	int shapesCount = shapes.size();
	for (int i = 0; i < shapesCount; i++)
	{
		delete(shapes[shapesCount-1]);
		shapes.pop_back();
		shapesCount--;
	}
}

void DrawingProcess::redrawAllDrawnShapes(HDC hdc)
{
	int shapesCount = shapes.size();
	for (int i = 0; i < shapesCount; i++)
	{
		shapes[i]->draw(hdc, false);
	}
}

void DrawingProcess::stopDrawingCurrentShape()
{
	if (currentShape->isReadyToDraw())
	{
		shapes.push_back(currentShape);
	}
	InvalidateRect(hWnd, NULL, true);
	isDrawingNow = false;
}

DRAWING_TYPE DrawingProcess::getDrawingType()
{
	return currentShape->getDrawingType();
}

DrawingObject* DrawingProcess::createCurrentShape(DRAWING_OBJECTS shapeType, HPEN hPen, HBRUSH hBrush)
{
	switch (shapeType)
	{
	case LINE:
	{
		return new LineObject(hPen, hBrush);
	}
	case BROKEN_LINE:
	{
		return new BrokenLineObject(hPen, hBrush);
	}
	case MULTI_LINE:
	{
		return new MultiLineObject(hPen, hBrush);
	}
	case RECTANGLE:
	{
		return new RectangleObject(hPen, hBrush);
	}
	case POLYGON:
	{
		return new PolygonObject(hPen, hBrush);
	}
	case ELLIPSE:
	{
		return new EllipseObject(hPen, hBrush);
	}
	case TEXT:
	{
		return new TextObject(hPen, hBrush);
	}
	default:
		break;
	}
}

void DrawingProcess::setOriginTo(POINT origin)
{
	prevOrigin = this->origin;
	this->origin = origin;
}

POINT DrawingProcess::getDrawingOrigin()
{
	return origin;
}

void DrawingProcess::startOrContinueDrawingShape(POINT currentLocation, DRAWING_OBJECTS currentShapeType, HPEN hPen, HBRUSH hBrush)
{
	currentLocation.x = currentLocation.x / zoom;
	currentLocation.y = currentLocation.y / zoom;
	if (isDrawingNow == false)
	{
		this->currentShape = createCurrentShape(currentShapeType, hPen, hBrush);
		isDrawingNow = true;
	}
	currentShape->addNewOrChangeLastCoordinate(currentLocation);
	if (currentShape->isReadyToDraw() && currentShape->hasLimitedCoordinatesCount())
	{
		shapes.push_back(currentShape);
		isDrawingNow = false;
	}
}

void DrawingProcess::recalcCoordinates()
{
	int shapesCount = shapes.size();
	int deltaX = origin.x - prevOrigin.x;
	int deltaY = origin.y - prevOrigin.y;
	for (int i = 0; i < shapesCount; i++)
	{
		shapes[i]->recalculateCoordinates(deltaX, deltaY);
	}
	if (shapesCount != 0)
	{
		if (shapes[shapesCount - 1] != currentShape)
		{
			currentShape->recalculateCoordinates(deltaX, deltaY);
		}
	}
}

void DrawingProcess::drawToFile(HDC fileContext)
{
	RECT windowRect;
	GetClientRect(hWnd, &windowRect);
	FillRect(fileContext, &windowRect, CreateSolidBrush(RGB(255, 255, 255)));
	redrawAllDrawnShapes(fileContext);
}

void DrawingProcess::drawToHDC(HDC hdc, POINT currentLocation)
{
	redrawAllDrawnShapes(hdc);
	if (isDrawingNow == true)
	{
		if (currentShape->hasStaticCoordinates() == false)
		{
			if (currentShape->getDrawingType() == SEPARATED)
			{
				currentShape->changeLastOrAddNewCoordinate(currentLocation);
			}
			else
			{
				currentShape->addNewOrChangeLastCoordinate(currentLocation);
			}
		}
		currentShape->draw(hdc, true);
	}
}

void DrawingProcess::zoomIn(float zoom)
{
	if (((this->zoom * zoom) <= 64) && ((this->zoom * zoom) >= 0.25))
	{
		this->zoom *= zoom;
		InvalidateRect(hWnd, NULL, true);
	}
}

void DrawingProcess::drawing(POINT currentLocation)
{
	currentLocation.x = currentLocation.x / zoom;
	currentLocation.y = currentLocation.y / zoom;
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hWnd, &ps);
	//init buffer
	RECT windowRect;
	GetClientRect(hWnd, &windowRect);
	//
	windowRect.right = windowRect.right / zoom;
	windowRect.bottom = windowRect.bottom / zoom;
	//
	int windowWidth = windowRect.right - windowRect.left;//!
	int windowHeight = windowRect.bottom - windowRect.top;//!
	HDC mdc = CreateCompatibleDC(hdc);
	HBITMAP mbmp = CreateCompatibleBitmap(hdc, windowWidth, windowHeight);
	SelectObject(mdc, mbmp);
	FillRect(mdc, &windowRect, CreateSolidBrush(RGB(255, 255, 255)));
	//drawing logic
	if (workingMode == MOVING)
	{
		recalcCoordinates();
	}
	drawToHDC(mdc, currentLocation);
	
	//zoom logic
	if (zoom != 1)
	{
		XFORM xForm;
		xForm.eM11 = xForm.eM22 = zoom;
		xForm.eM12 = xForm.eM21 = xForm.eDx = xForm.eDy = 0;
		SetGraphicsMode(hdc, GM_ADVANCED);
		SetWorldTransform(hdc, &xForm);
	}

	//copying image from buffer to screen
	BitBlt(hdc, 0, 0, windowWidth, windowHeight, mdc, 0, 0, SRCCOPY);

	//free resources
	DeleteObject(mbmp);
	DeleteDC(mdc);
	DeleteDC(hdc);
	EndPaint(hWnd, &ps);
}

void DrawingProcess::drawingText(DWORD character) 
{
	if (character != 8)
	{
		currentShape->addDataCell(character);
	}
	else
	{
		currentShape->removeDataCell();
	}
}

bool DrawingProcess::isDrawing()
{
	return isDrawingNow;
}

void DrawingProcess::setWorkingMode(WORKING_MODE workingMode)
{
	this->workingMode = workingMode;
}

WORKING_MODE DrawingProcess::getWorkingMode()
{
	return workingMode;
}