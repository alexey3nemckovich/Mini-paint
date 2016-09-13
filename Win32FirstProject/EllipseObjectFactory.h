#include "DrawingObjectFactory.h"
#include "EllipseObject.h"

class EllipseObjectFactory : public DrawingObjectFactory
{
public:
	DrawingObject *newDrawingObject(HPEN hPen, HBRUSH hBrush)
	{
		return new EllipseObject(hPen, hBrush);
	}
};