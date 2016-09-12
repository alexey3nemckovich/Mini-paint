#include "MultiLineObject.h"

MultiLineObject::MultiLineObject(HPEN hPen, HBRUSH hBrush) : DrawingObject(hPen, hBrush)
{
	DrawingObject::setDrawingConditions(minCoordinatesCount, maxCoordinatesCount, limitedCoordinatesCount, drawingType, staticCoordinates);
}

void MultiLineObject::draw(HDC hdc, bool redraw)
{
	HGDIOBJ prevPen = SelectObject(hdc, hPen);
	POINT prevPos; 
	MoveToEx(hdc, 0, 0, &prevPos);
	int coordinatesCount = coordinates.size();
	if (coordinatesCount == 1)
	{
		Ellipse(hdc, coordinates[0].x - 1, coordinates[0].y - 1, coordinates[0].x + 1, coordinates[0].y + 1);
	}
	else
	{
		for (int i = 1; i < coordinatesCount; i++)
		{
			MoveToEx(hdc, coordinates[i-1].x, coordinates[i-1].y, NULL);
			LineTo(hdc, coordinates[i].x, coordinates[i].y);
		}
	}
	MoveToEx(hdc, prevPos.x, prevPos.y, NULL);
	SelectObject(hdc, prevPen);
}