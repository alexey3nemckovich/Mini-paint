#include "DrawingObjectFactory.h"
#include "MultiLineObject.h"

class MultiLineObjectFactory : public DrawingObjectFactory
{
public:
	DrawingObject *newDrawingObject(HPEN hPen, HBRUSH hBrush)
	{
		return new MultiLineObject(hPen, hBrush);
	}
};