#include "PolygonObject.h"

PolygonObject::PolygonObject(HPEN hPen, HBRUSH hBrush) : DrawingObject(hPen, hBrush)
{
	DrawingObject::setDrawingConditions(minCoordinatesCount, maxCoordinatesCount, limitedCoordinatesCount, drawingType, staticCoordinates);
}

void PolygonObject::draw(HDC hdc, bool redraw)
{
	HGDIOBJ prevPen = SelectObject(hdc, hPen);
	int coordinatesCount = coordinates.size();
	POINT oldPoint;
	MoveToEx(hdc, coordinates[0].x, coordinates[0].y, &oldPoint);
	for (int i = 1; i < coordinatesCount; i++)
	{
		LineTo(hdc, coordinates[i].x, coordinates[i].y);
		MoveToEx(hdc, coordinates[i].x, coordinates[i].y, NULL);
	}
	if (redraw == false)
	{
		LineTo(hdc, coordinates[0].x, coordinates[0].y);
	}
	MoveToEx(hdc, oldPoint.x, oldPoint.y, NULL);
	SelectObject(hdc, prevPen);
}