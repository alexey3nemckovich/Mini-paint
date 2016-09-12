#ifndef DRAWING_OBJECT_CLASS_INTERFACE
#define DRAWING_OBJECT_CLASS_INTERFACE
#include <windows.h>
#include <vector>
#include <xstring>
using namespace std;

typedef std::basic_string<TCHAR, std::char_traits<TCHAR>,
	std::allocator<TCHAR> > String;

enum DRAWING_OBJECTS {
	MULTI_LINE,
	LINE,
	BROKEN_LINE,
	RECTANGLE,
	POLYGON,
	ELLIPSE,
	TEXT,
};

enum DRAWING_TYPE {
	SEPARATED,
	UNSEPARATED,
};

class DrawingObject
{
public:
	DrawingObject(HPEN, HBRUSH);
	bool isReadyToDraw();
	bool hasStaticCoordinates();
	bool hasLimitedCoordinatesCount();
	void addNewOrChangeLastCoordinate(POINT);
	void changeLastOrAddNewCoordinate(POINT);
	virtual void draw(HDC, bool) = 0;
	DRAWING_TYPE getDrawingType();
	~DrawingObject();
	void addDataCell(DWORD data);
	void removeDataCell();
	void recalculateCoordinates(int deltaX, int deltaY);
protected:
	String objectData;
	void setDrawingConditions(int minCoordinatesCount, int maxCoordinatesCount, bool unlimitedCoordinatesCount, DRAWING_TYPE drawingType, bool staticCoordinates);
	HPEN hPen;
	HBRUSH hBrush;
	vector<POINT> coordinates;
	int maxCoordinatesCount = 0;
	int minCoordinatesCount = 0;
	bool readyToDraw = false;
	bool limitedCoordinatesCount = true;
	bool staticCoordinates = false;
	DRAWING_TYPE drawingType = SEPARATED;
};
#endif