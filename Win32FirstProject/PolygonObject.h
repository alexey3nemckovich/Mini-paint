#ifndef POLYGON_CLASS_INTERFACE 
#define POLYGON_CLASS_INTERFACE
#include "DrawingObject.h"
#include <windows.h>

class PolygonObject : public DrawingObject
{
protected:
	int minCoordinatesCount = 3;
	int maxCoordinatesCount = -1;
	bool limitedCoordinatesCount = false;
	DRAWING_TYPE drawingType = SEPARATED;
public:
	PolygonObject(HPEN hPen, HBRUSH hBrush);
	void draw(HDC, bool);
};
#endif