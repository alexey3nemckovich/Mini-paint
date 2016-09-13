#include "DrawingObjectFactory.h"
#include "RectangleObject.h"

class RectangleObjectFactory : public DrawingObjectFactory
{
public:
	DrawingObject *newDrawingObject(HPEN hPen, HBRUSH hBrush)
	{
		return new RectangleObject(hPen, hBrush);
	}
};