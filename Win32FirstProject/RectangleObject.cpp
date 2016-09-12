#include "RectangleObject.h"

RectangleObject::RectangleObject(HPEN hPen, HBRUSH hBrush) : DrawingObject(hPen, hBrush)
{
	DrawingObject::setDrawingConditions(minCoordinatesCount, maxCoordinatesCount, limitedCoordinatesCount, drawingType, staticCoordinates);
}

void RectangleObject::draw(HDC hdc, bool redraw)
{
	HGDIOBJ prevPen = SelectObject(hdc, hPen);
	HGDIOBJ prevBrush = SelectObject(hdc, hBrush);
	Rectangle(hdc, coordinates[0].x, coordinates[0].y, coordinates[1].x, coordinates[1].y);
	SelectObject(hdc, prevPen);
	SelectObject(hdc, prevBrush);
}