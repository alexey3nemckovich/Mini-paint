#include "DrawingObjectFactory.h"
#include "BrokenLineObject.h"

class BrokenLineObjectFactory : public DrawingObjectFactory
{
public:
	DrawingObject *newDrawingObject(HPEN hPen, HBRUSH hBrush)
	{
		return new BrokenLineObject(hPen, hBrush);
	}
};