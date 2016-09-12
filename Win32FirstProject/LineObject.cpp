#include "LineObject.h"

LineObject::LineObject(HPEN hPen, HBRUSH hBrush) : DrawingObject(hPen, hBrush)
{
	DrawingObject::setDrawingConditions(minCoordinatesCount, maxCoordinatesCount, limitedCoordinatesCount, drawingType, staticCoordinates);
}

void LineObject::draw(HDC hdc, bool redraw)
{
	HGDIOBJ prevPen = SelectObject(hdc, hPen);
	POINT oldPoint;
	MoveToEx(hdc, coordinates[0].x, coordinates[0].y, &oldPoint);
	LineTo(hdc, coordinates[1].x, coordinates[1].y);
	MoveToEx(hdc, oldPoint.x, oldPoint.y, NULL);
	SelectObject(hdc, prevPen);
}