#include "DrawingObjectFactory.h"
#include "LineObject.h"

class LineObjectFactory : public DrawingObjectFactory
{
public:
	DrawingObject *newDrawingObject(HPEN hPen, HBRUSH hBrush)
	{
		return new LineObject(hPen, hBrush);
	}
};