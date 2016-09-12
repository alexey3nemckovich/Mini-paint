#ifndef LINE_CLASS_INTERFACE 
#define LINE_CLASS_INTERFACE
#include "DrawingObject.h"

class LineObject : public DrawingObject
{
protected:
	int minCoordinatesCount = 2;
	int maxCoordinatesCount = 2;
	bool limitedCoordinatesCount = true;
	DRAWING_TYPE drawingType = SEPARATED;
public:
	LineObject(HPEN hPen, HBRUSH hBrush);
	void draw(HDC, bool);
};
#endif