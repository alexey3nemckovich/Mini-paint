#include "TextObject.h"

TextObject::TextObject(HPEN hPen, HBRUSH hBrush) : DrawingObject(hPen, hBrush)
{
	DrawingObject::setDrawingConditions(minCoordinatesCount, maxCoordinatesCount, limitedCoordinatesCount, drawingType, staticCoordinates);
}

void TextObject::draw(HDC hdc, bool redraw)
{
	HGDIOBJ prevPen = SelectObject(hdc, hPen);
	TextOut(hdc, coordinates[0].x, coordinates[0].y, objectData.data(), objectData.size());
	SelectObject(hdc, prevPen);
}