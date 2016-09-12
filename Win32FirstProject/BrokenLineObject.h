#ifndef BROKEN_LINE_CLASS_INTERFACE 
#define BROKEN_LINE_CLASS_INTERFACE
#include "DrawingObject.h"

class BrokenLineObject : public DrawingObject
{
protected:
	int minCoordinatesCount = 2;
	int maxCoordinatesCount = -1;
	bool limitedCoordinatesCount = false;
	DRAWING_TYPE drawingType = SEPARATED;
public:
	BrokenLineObject(HPEN hPen, HBRUSH hBrush);
	void draw(HDC, bool);
};
#endif