#ifndef ELLIPSE_CLASS_INTERFACE 
#define ELLIPSE_CLASS_INTERFACE
#include "DrawingObject.h"
#include <windows.h>

class EllipseObject : public DrawingObject
{
protected:
	int minCoordinatesCount = 2;
	int maxCoordinatesCount = 2;
	bool limitedCoordinatesCount = true;
	DRAWING_TYPE drawingType = SEPARATED;
public:
	EllipseObject(HPEN hPen, HBRUSH hBrush);
	void draw(HDC, bool);
};
#endif