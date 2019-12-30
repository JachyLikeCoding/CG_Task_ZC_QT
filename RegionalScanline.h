#pragma once
#ifndef REGIONAL_SCANLINE_Z_BUFFER
#define REGIONAL_SCANLINE_Z_BUFFER
#include "Basic.h"
#include "DataStructure.h"
#include "ObjLoader.h"

class RegionalScanlineProcessor {
public:
	vector<vector<ClassifiedPolygon>> CPT;
	vector<vector<ClassifiedEdge>> CET;
	vector<ActivePolygon> APT;
	vector<ActiveEdge> AET;
	void initProcessor(Object &_obj);
	void RegionalScanline(Object &_obj);
	void resizeWindow(int width, int height);
	vector<GLfloat> getframebuffer() { return framebuffer; }
	int findNextCE(ActiveEdge &AE, GLfloat x, vector<ClassifiedEdge> &CET_y);

private:
	Object obj;
	int winWidth, winHeight;
	vec3 getColor(int polygon_id, vector<ActivePolygon> &APT);
	bool cmp(const ActivePolygon &a, const ActivePolygon &b);
	vector<int> coloridbuffer;//Ö¡»º³åÆ÷
	vector<GLfloat> framebuffer;
	void buildCET();
	void buildCPT();
	bool addPolygonToAPT(int maxY, vector<ActivePolygon> &APT, vector<vector<ClassifiedPolygon>> &CPT, vector<ClassifiedEdge> &CET_y);
	bool addEdgeToAET(vector<ClassifiedEdge> &CET_y, vector<ActiveEdge> &AET, vector<ActivePolygon> &APT);
	bool updateBuffer(vector<ActiveEdge> &AET, int y);
	bool update_APTAET(vector<ActivePolygon> &APT, vector<ActiveEdge> &AET, vector<ClassifiedEdge> &CET_y);
	bool update_AET(ActiveEdge &AE, vector<ClassifiedEdge> &CET_y);
	bool update_APT(ActivePolygon &AP);
	void clearDS();
	//just for debug
	void test();
	void printAET(vector<ActiveEdge> &AET);
	void printAPT(vector<ActivePolygon> &APT);
};


#endif // !REGIONAL_SCANLINE_Z_BUFFER