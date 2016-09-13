#include "DrawingObjectFactory.h"
#include "PolygonObject.h"

class PolygonObjectFactory : public DrawingObjectFactory
{
public:
	DrawingObject *newDrawingObject(HPEN hPen, HBRUSH hBrush)
	{
		return new PolygonObject(hPen, hBrush);
	}
};