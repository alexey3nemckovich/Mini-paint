#include "DrawingObjectFactory.h"
#include "TextObject.h"

class TextObjectFactory : public DrawingObjectFactory
{
public:
	DrawingObject *newDrawingObject(HPEN hPen, HBRUSH hBrush)
	{
		return new TextObject(hPen, hBrush);
	}
};