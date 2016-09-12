#ifndef TEXT_CLASS_INTERFACE 
#define TEXT_CLASS_INTERFACE
#include "DrawingObject.h"
#include <windows.h>

class TextObject : public DrawingObject
{
protected:
	int minCoordinatesCount = 1;
	int maxCoordinatesCount = -1;
	bool limitedCoordinatesCount = false;
	DRAWING_TYPE drawingType = SEPARATED;
	bool staticCoordinates = true;
public:
	TextObject(HPEN hPen, HBRUSH hBrush);
	void draw(HDC, bool);
};
#endif
