#pragma once
#include<GL/glut.h>
#include<QGLWidget>
#include "DataStructure.h"
#include "ObjLoader.h"
#include"ScanlineZBuffer.h"

class myOpenGL : public QGLWidget
{
	Q_OBJECT
public:
	explicit myOpenGL(QWidget *parent = nullptr);
	~myOpenGL();
	bool isSpeedUp = true; //true: speed up;  false: not speed up
	bool algorithmChoose = false;	//true:scanline-z-buffer;  false:regional-scanline
	int vcount = 0;
	int fcount = 0;
	double time = 0;
	QString objName = "models/cat.obj";

protected:
	void initializeGL();
	void paintGL();
	void resizeGL(int width, int height);

private:
	Object obj;
	int winWidth = 800;
	int winHeight = 600;
	GLfloat Ortho = 5;
	bool in;
	int angleX = 0, angleY = 0;
	void displayModel();
	ScanlineZBufferProcessor scanlineProcessor;
};
