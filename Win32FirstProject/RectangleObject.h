#ifndef RECTANGLE_CLASS_INTERFACE 
#define RECTANGLE_CLASS_INTERFACE
#include "DrawingObject.h"
#include <windows.h>

class RectangleObject : public DrawingObject
{
protected:
	int minCoordinatesCount = 2;
	int maxCoordinatesCount = 2;
	bool limitedCoordinatesCount = true;
	DRAWING_TYPE drawingType = SEPARATED;
public:
	RectangleObject(HPEN hPen, HBRUSH hBrush);
	void draw(HDC, bool);
};
#endif