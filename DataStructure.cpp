#include"DataStructure.h"

//ActiveEdge::ActiveEdge(ClassifiedEdge &leftEdge, ClassifiedEdge &rightEdge, ClassifiedPolygon &CP) {
//	xl = leftEdge.x;
//	xr = rightEdge.x;
//	//debug
//	if (xl < 0 || xr < 0) {
//		qDebug() << "xl < 0 || xr < 0" << endl;
//		system("pause");
//	}
//	dxl = leftEdge.dx;
//	dyl = leftEdge.dy;
//	dxr = rightEdge.dx;
//	dyr = rightEdge.dy;
//	edge_polygon_id = CP.polygon_id;
//	if (abs(CP.c) > 1e-5)//if c != 0
//	{
//		dzx = -CP.a / CP.c;
//		dzy = CP.b / CP.c;
//		zl = dzx * xl - dzy * leftEdge.maxY - CP.d / CP.c;
//	}
//	else {
//		dzx = -INT_MAX;
//		dzy = INT_MAX;
//		zl = -INT_MAX;
//	}
//}


ActiveEdge::ActiveEdge(ClassifiedEdge &leftEdge, ClassifiedEdge &rightEdge, ActivePolygon &AP) {
	xl = leftEdge.x;
	xr = rightEdge.x;
	//debug
	if (xl < 0 || xr < 0) {
		qDebug() << "xl < 0 || xr < 0" << endl;
		system("pause");
	}
	dxl = leftEdge.dx;
	dyl = leftEdge.dy;
	dxr = rightEdge.dx;
	dyr = rightEdge.dy;
	edge_polygon_id = AP.polygon_id;
	if (abs(AP.c) > 1e-5)//if c != 0
	{
		dzx = -AP.a / AP.c;
		dzy = AP.b / AP.c;
		zl = dzx * xl - dzy * leftEdge.maxY - AP.d / AP.c;
	}
	else {
		dzx = -INT_MAX;
		dzy = INT_MAX;
		zl = -INT_MAX;
	}
}

void ActiveEdge::changeLeftEdge(ClassifiedEdge &celeft)
{
	xl = celeft.x;
	dxl = celeft.dx;
	dyl = celeft.dy;

}

void ActiveEdge::changeRightEdge(ClassifiedEdge &ceright)
{
	xr = ceright.x;
	dxr = ceright.dx;
	dyr = ceright.dy;
}

bool ClassifiedEdge::isSamePolygon(ClassifiedEdge &edge){
	if (edge_polygon_id == edge.edge_polygon_id) 
		return true;
	else
		return false;
}

bool ClassifiedEdge::isLeftEdge(ClassifiedEdge &edge) {
	//先看是不是同一个多边形的边才能比较
	if (edge_polygon_id == edge.edge_polygon_id) {
		if (x < edge.x)
		{
			return true;
		}
		else if(x == edge.x){
			if (dx < edge.dx || edge.dx == -INT_MAX) {
				return true;
			}
		}
	}
	return false;
}