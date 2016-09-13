#include "DrawingProcess.h"
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
	loadedFile = NULL;
	currentDrawingObjectType = MULTI_LINE;
	drawingObjectFactory = new MultiLineObjectFactory();
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
	DeleteEnhMetaFile(loadedFile);
}

void DrawingProcess::setDrawingPen(HPEN hPen)
{
	this->drawingPen = hPen;
}

void DrawingProcess::setDrawingBrush(HBRUSH hBrush)
{
	this->drawingBrush = hBrush;
}

void DrawingProcess::setCurrentDrawingObjectsType(DRAWING_OBJECTS currentDrawingObjectType)
{
	if (currentDrawingObjectType != this->currentDrawingObjectType)
	{
		this->currentDrawingObjectType = currentDrawingObjectType;
		setCurrentDrawingObjectFactory(currentDrawingObjectType);
	}
}

DRAWING_OBJECTS DrawingProcess::getCurrentDrawingObjectsType()
{
	return currentDrawingObjectType;
}

float DrawingProcess::getZoom()
{
	return zoom;
}

void DrawingProcess::setCurrentDrawingObjectFactory(DRAWING_OBJECTS drawingObjectType)
{
	delete(drawingObjectFactory);
	switch (drawingObjectType)
	{
		case LINE:
		{
			drawingObjectFactory = new LineObjectFactory();
		}
		break;
		case BROKEN_LINE:
		{
			drawingObjectFactory = new BrokenLineObjectFactory();
		}
		break;
		case MULTI_LINE:
		{
			drawingObjectFactory = new MultiLineObjectFactory();
		}
		break;
		case RECTANGLE:
		{
			drawingObjectFactory = new RectangleObjectFactory();
		}
		break;
		case POLYGON:
		{
			drawingObjectFactory = new PolygonObjectFactory();
		}
		break;
		case ELLIPSE:
		{
			drawingObjectFactory = new EllipseObjectFactory();
		}
		break;
		case TEXT:
		{
			drawingObjectFactory = new TextObjectFactory();
		}
		break;
		default:
			break;
	}
}

DrawingObject* DrawingProcess::popLastDrawingObject()
{
	currentShape = NULL;
	int shapesCount = shapes.size();
	if (shapesCount != 0)
	{
		DrawingObject* lastObject = shapes[shapesCount - 1];
		shapes.pop_back();
		return lastObject;
	}
	return NULL;
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

void DrawingProcess::clearShapesList()
{
	int shapesCount = shapes.size();
	DrawingObject *drawingObj;
	for (int i = 0; i < shapesCount; i++)
	{
		drawingObj = popLastDrawingObject();
		delete(drawingObj);
	}
}

void DrawingProcess::setLoadedFile(HENHMETAFILE file)
{
	DeleteEnhMetaFile(loadedFile);
	this->loadedFile = file;
	int shapesCount = shapes.size();
	if (shapesCount != 0)
	{
		clearShapesList();
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

void DrawingProcess::startOrContinueDrawingShape(POINT currentLocation, HPEN hPen, HBRUSH hBrush)
{
	currentLocation.x = currentLocation.x / zoom;
	currentLocation.y = currentLocation.y / zoom;
	if (isDrawingNow == false)
	{
		if (workingMode == SELECTING_AREA)
		{
			this->currentShape = drawingObjectFactory->newDrawingObject(hPen, (HBRUSH)GetStockObject(HOLLOW_BRUSH));
		}
		else
		{
			this->currentShape = drawingObjectFactory->newDrawingObject(hPen, hBrush);
		}
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
		if ((shapes[shapesCount - 1] != currentShape) && (currentShape != NULL))
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

void DrawingProcess::drawToFile(HDC fileContext, RECT rectToDraw)
{
	FillRect(fileContext, &rectToDraw, CreateSolidBrush(RGB(255, 255, 255)));
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
	//redrawing loaded file
	if (loadedFile != NULL)
	{
		windowRect.right *= zoom;
		windowRect.bottom *= zoom;
		windowRect.left += origin.x;
		windowRect.right += origin.x;
		windowRect.bottom += origin.y;
		windowRect.top += origin.y;
		PlayEnhMetaFile(mdc, loadedFile, &windowRect);
	}

	//recalc
	if (workingMode == MOVING)
	{
		recalcCoordinates();
	}

	//drawing
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

void DrawingProcess::switchToPrevWorkingMode()
{
	int workingModesHistorySize = workingModesHistory.size();
	if (workingModesHistorySize > 0)
	{
		WORKING_MODE prevWorkingMode = workingModesHistory[workingModesHistorySize - 1];
		workingMode = prevWorkingMode;
		workingModesHistory.pop_back();
	}
}

void DrawingProcess::setWorkingMode(WORKING_MODE workingMode)
{
	if (workingModesHistory.size() == 3)
	{
		workingModesHistory.pop_back();
	}
	workingModesHistory.push_back(this->workingMode);
	this->workingMode = workingMode;
}

WORKING_MODE DrawingProcess::getWorkingMode()
{
	return workingMode;
}