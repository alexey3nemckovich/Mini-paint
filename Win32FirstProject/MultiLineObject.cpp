#include "MultiLineObject.h"

MultiLineObject::MultiLineObject(HPEN hPen, HBRUSH hBrush) : DrawingObject(hPen, hBrush)
{
	DrawingObject::setDrawingConditions(minCoordinatesCount, maxCoordinatesCount, limitedCoordinatesCount, drawingType, staticCoordinates);
}

void MultiLineObject::draw(HDC hdc, bool redraw)
{
	HGDIOBJ prevPen = SelectObject(hdc, hPen);
	int coordinatesCount = coordinates.size();
	for (int i = 0; i < coordinatesCount; i++)
	{
		Ellipse(hdc, coordinates[i].x, coordinates[i].y, coordinates[i].x + 2, coordinates[i].y + 2);
	}
	SelectObject(hdc, prevPen);
}