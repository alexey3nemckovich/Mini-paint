#ifndef DRAWING_OBJECT_FACTORY_CLASS_INTERFACE
#define DRAWING_OBJECT_FACTORY_CLASS_INTERFACE
#include "DrawingObject.h"

class DrawingObjectFactory
{
private:
public:
	virtual DrawingObject *newDrawingObject(HPEN hPen, HBRUSH hBrush) = 0;
};

#endif