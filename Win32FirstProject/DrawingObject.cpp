#include "DrawingObject.h"

DrawingObject::DrawingObject(HPEN hPen, HBRUSH hBrush)
{
	this->hPen = hPen;
	this->hBrush = hBrush;
}

DrawingObject::~DrawingObject()
{
	DeleteObject(hPen);
	DeleteObject(hBrush);
}

DRAWING_TYPE DrawingObject::getDrawingType()
{
	return drawingType;
}

bool DrawingObject::isReadyToDraw()
{
	return readyToDraw;
}

bool DrawingObject::hasLimitedCoordinatesCount()
{
	return limitedCoordinatesCount;
}

bool DrawingObject::hasStaticCoordinates()
{
	return staticCoordinates;
}

void DrawingObject::setDrawingConditions(int minCoordinatesCount, int maxCoordinatesCount, bool limitedCoordinatesCount, DRAWING_TYPE drawingType, bool staticCoordinates)
{
	this->minCoordinatesCount = minCoordinatesCount;
	this->maxCoordinatesCount = maxCoordinatesCount;
	this->limitedCoordinatesCount = limitedCoordinatesCount;
	this->drawingType = drawingType;
	this->staticCoordinates = staticCoordinates;
}

void DrawingObject::recalculateCoordinates(int deltaX, int deltaY)
{
	int coordinatesCount = coordinates.size();
	for (int i = 0; i < coordinatesCount; i++)
	{
		coordinates[i].x += deltaX;
		coordinates[i].y += deltaY;
	}
}

void DrawingObject::addDataCell(DWORD data)
{
	objectData += (TCHAR)data;
}

void DrawingObject::removeDataCell()
{
	if (objectData.size() != 0)
	{
		objectData.pop_back();
	}
}

void DrawingObject::addNewOrChangeLastCoordinate(POINT newPoint)
{
	if (readyToDraw && limitedCoordinatesCount)
	{
		coordinates.pop_back();
		coordinates.push_back(newPoint);
	}
	else
	{
		coordinates.push_back(newPoint);
		if (readyToDraw == false)
		{
			if (limitedCoordinatesCount)
			{
				if (coordinates.size() == this->maxCoordinatesCount)
				{
					readyToDraw = true;
				}
			}
			else
			{
				if (coordinates.size() >= this->minCoordinatesCount)
				{
					readyToDraw = true;
				}
			}
		}
	}
}

vector<POINT> DrawingObject::getCoordinates()
{
	return coordinates;
}

void DrawingObject::changeLastOrAddNewCoordinate(POINT newPoint)
{
	if (coordinates.size() > 1)
	{
		coordinates[coordinates.size() - 1] = newPoint;
	}
	else
	{
		addNewOrChangeLastCoordinate(newPoint);
	}
}