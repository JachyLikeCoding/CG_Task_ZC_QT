#pragma once
#ifndef OBJ_LOADER_H
#define OBJ_LOADER_H
#include "Basic.h"
#include "DataStructure.h"

class Object {
public:
	Object(){}
	vector<ivec3> vertices;
	vector<vec3> originvertices;
	vector<vec3> colorlist;
	vector<vector<int>> faces;
	vector<ClassifiedEdge> edges;
	bool isSpeedUp;
	void initObject(const QString &objName, int width, int height, int mode, bool isSpeed);
	bool loadObj(const QString &objName);
	void CalFace(int face_id, GLfloat &a, GLfloat &b, GLfloat &c, GLfloat &d, int &maxY, GLfloat &maxZ, int &dy, vec3 &color);
	void CalFaceEdges(int face_id);
	GLfloat maxY, minY, maxX, minX, maxZ, minZ;
	int getWinWidth() { return winWidth; }
	int getWinHeight() { return winHeight; }
	void test();
	void changeToScreenSize();
	void SpeedUp(bool isSpeedUp);
	vector<bool> isBackFace;

private:
	int winWidth, winHeight;
	int mode;
	ivec3 view;		// default (0,0,-1)
	ClassifiedEdge CalEdge(int polygon_id, int v1_id, int v2_id, ClassifiedEdge edge);
};


#endif // !OBJ_LOADER_H