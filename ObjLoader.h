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
	void initObject(const QString &objName, int width, int height, int mode);
	bool loadObj(const QString &objName);
	void CalFace(int face_id, GLfloat &a, GLfloat &b, GLfloat &c, GLfloat &d, int &maxY, GLfloat &maxZ, int &dy, vec3 &color);
	void CalFaceEdges(int face_id);
	GLfloat maxY, minY, maxX, minX, maxZ, minZ;
	int getWinWidth() { return winWidth; }
	int getWinHeight() { return winHeight; }
	GLfloat getOrtho() { return Ortho; }
	void test();
	void modelUnitize();
private:
	int winWidth, winHeight;
	int mode;
	ivec3 view;		// default (0,0,-1)
	GLfloat Ortho;
	ClassifiedEdge CalEdge(int polygon_id, int v1_id, int v2_id, ClassifiedEdge edge);
	void getScreenPos(vector<vec3> &originalposition, vector<ivec3> &screenposition);

};


#endif // !OBJ_LOADER_H