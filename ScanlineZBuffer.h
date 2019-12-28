#pragma once
#ifndef SCANLINE_Z_BUFFER_H
#define SCANLINE_Z_BUFFER
#include "Basic.h"
#include "DataStructure.h"
#include "ObjLoader.h"
class ScanlineZBufferProcessor {
public:
	
	QVector<QVector<ClassifiedPolygon>> CPT;
	QVector<QVector<ClassifiedEdge>> CET;
	QVector<ActivePolygon> APT;
	QVector<ActiveEdge> AET;
	void initProcessor(Object &_obj);
	
	void ScanlineZBuffer(Object &_obj);
	
	void resizeWindow(int width, int height);
	QVector<GLfloat> getframebuffer() { return framebuffer; }
	int findNextCE(ActiveEdge &AE, GLfloat x, QVector<ClassifiedEdge> &CET_y);



private:
	Object obj;
	int winWidth, winHeight;
	vec3 getColor(int polygon_id, QVector<ActivePolygon> &APT);
	QVector<GLfloat> zbuffer;//z»º³åÆ÷
	QVector<int> coloridbuffer;//Ö¡»º³åÆ÷
	QVector<GLfloat> framebuffer;
	
	void buildCET();
	void buildCPT();
	bool addPolygonToAPT(int maxY, QVector<ActivePolygon> &APT, QVector<QVector<ClassifiedPolygon>> &CPT);
	//void addEdgeToAET(QVector<ClassifiedEdge> &CET_y, QVector<ActiveEdge> &AET, QVector<ClassifiedPolygon> &CPT_y);
	void addEdgeToAET(QVector<ClassifiedEdge> &CET_y, QVector<ActiveEdge> &AET, QVector<ActivePolygon> &APT);
	void clearDS();
	bool updateBuffer(QVector<ActiveEdge> &AET, int y);
	bool update_APTAET(QVector<ActivePolygon> &APT, QVector<ActiveEdge> &AET, QVector<ClassifiedEdge> &CET_y);
	bool update_AET(ActiveEdge &AE, QVector<ClassifiedEdge> &CET_y);
	bool update_APT(ActivePolygon &AP);
	//just for debug
	void test();
	void printAET(QVector<ActiveEdge> &AET);
	void printAPT(QVector<ActivePolygon> &APT);
};

#endif // !SCANLINE_Z_BUFFER_H
