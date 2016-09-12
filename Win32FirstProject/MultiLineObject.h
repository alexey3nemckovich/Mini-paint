#ifndef MULTILINE_CLASS_INTERFACE 
#define MULTILINE_CLASS_INTERFACE
#include "DrawingObject.h"
#include <windows.h>

class MultiLineObject : public DrawingObject
{
protected:
	int minCoordinatesCount = 1;
	int maxCoordinatesCount = -1;
	bool limitedCoordinatesCount = false;
	DRAWING_TYPE drawingType = UNSEPARATED;
public:
	MultiLineObject(HPEN hPen, HBRUSH hBrush);
	void draw(HDC, bool);
};
#endif