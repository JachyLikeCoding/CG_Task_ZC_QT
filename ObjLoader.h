#pragma once
#ifndef OBJ_LOADER_H
#define OBJ_LOADER_H
#include "Basic.h"
#include "DataStructure.h"

class Object {
public:
	Object(){}
	QVector<ivec3> vertices;
	QVector<vec3> originvertices;
	QVector<vec3> colorlist;
	QVector<QVector<int>> faces;
	QVector<ClassifiedEdge> edges;
	void initObject(const QString &objName, int width, int height, int mode);
	
	void CalFace(int face_id, GLfloat &a, GLfloat &b, GLfloat &c, GLfloat &d, int &maxY, GLfloat &maxZ, int &dy, vec3 &color);
	void CalFaceEdges(int face_id);
	GLfloat maxY, minY, maxX, minX, maxZ, minZ;
	int getWinWidth() { return winWidth; }
	int getWinHeight() { return winHeight; }
	GLfloat getOrtho() { return Ortho; }
	void test();

private:
	int winWidth, winHeight;
	int mode;
	ivec3 view;		// default (0,0,-1)
	GLfloat Ortho;
	bool loadObj(const QString &objName);
	ClassifiedEdge CalEdge(int polygon_id, int v1_id, int v2_id, ClassifiedEdge edge);
	void getScreenPos(QVector<vec3> &originalposition, QVector<ivec3> &screenposition);
	void ChangeOriginvertices();
	void ChangeScreenSize();
};


#endif // !OBJ_LOADER_H