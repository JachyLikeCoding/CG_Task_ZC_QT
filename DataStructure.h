#pragma once
#ifndef DATASTRUCTURE_H
#define DATASTRUCTURE_H
#include "Basic.h"

class ClassifiedPolygon;
class ClassifiedEdge;
class ActiveEdge;
class ActivePolygon;

//分类多边形表
class ClassifiedPolygon {
public:
	int polygon_id;		//多边形编号
	GLfloat a, b, c, d;			//多边形所在平面的方程系数
	int dy;						//跨越的扫描线数
	vec3 color;			//多边形颜色
	ClassifiedPolygon(int _polygon_id, GLfloat _a, GLfloat _b, GLfloat _c, GLfloat _d, int _dy, vec3 _color) :
		polygon_id(_polygon_id), a(_a), b(_b), c(_c), d(_d), dy(_dy), color(_color){}
	
};

//分类边表
class ClassifiedEdge {
public:
	GLfloat x;					//边的上端点的x坐标
	GLfloat dx;					//相邻两条扫描线焦点的x坐标差
	int dy;					//边跨越的扫描线数目
	int edge_polygon_id;		//边所属多边形的编号
	int maxY;			//边的上端点y坐标

	ClassifiedEdge() : x(0), dx(0), dy(-1), maxY(0), edge_polygon_id(-1){}
	ClassifiedEdge(int _x, int _dx, int _dy, int _maxY, int _edge_polygon_id = -1) : 
		x(_x), dx(_dx), dy(_dy), maxY(_maxY), edge_polygon_id(_edge_polygon_id){}
	bool isLeftEdge(ClassifiedEdge &edge);
	bool isSamePolygon(ClassifiedEdge &edge);
};


//活化多边形表
class ActivePolygon {
public:
	int polygon_id;		//多边形编号
	GLfloat a, b, c, d;			//多边形所在平面的方程系数
	int remain_dy;		//跨越的"剩余"扫描线数
	//GLfloat color[3];	//多边形颜色
	vec3 color;
	ActivePolygon(int _polygon_id, GLfloat _a, GLfloat _b, GLfloat _c, GLfloat _d, int _dy, vec3 _color) :
		polygon_id(_polygon_id), a(_a), b(_b), c(_c), d(_d), remain_dy(_dy), color(_color){}
};



//活化边表：存放投影多边形边界与扫描线相交的边对
class ActiveEdge {
public:
	GLfloat xl, xr;					//左、右交点的x坐标
	GLfloat dxl, dxr;
	GLfloat dyl, dyr;
	int edge_polygon_id;
	vec3 color;
	GLfloat dzx;
	GLfloat dzy;
	GLfloat zl;
	ActiveEdge(ClassifiedEdge &leftEdge, ClassifiedEdge &rightEdge, ActivePolygon &AP);
	//ActiveEdge(ClassifiedEdge &leftEdge, ClassifiedEdge &rightEdge, ClassifiedPolygon &CP);
	void changeLeftEdge(ClassifiedEdge &celeft);
	void changeRightEdge(ClassifiedEdge &ceright);
};


#endif // !DATASTRUCTURE_H
