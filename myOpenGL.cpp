#include"myOpenGL.h"
#include "ui_CG_Task_ZC_QT.h"

myOpenGL::myOpenGL(QWidget *parent) :QGLWidget(parent) {
}

myOpenGL::~myOpenGL() {}

void myOpenGL::initializeGL() {}

void myOpenGL::paintGL() {
	obj.initObject(objName, winWidth, winHeight, mode);
	obj.test();
	scanlineProcessor.ScanlineZBuffer(obj);
}

void myOpenGL::resizeGL(int width, int height){
	Ortho = obj.getOrtho();
	winWidth = width;
	winHeight = height;
	GLfloat scale = GLfloat(width) / GLfloat(height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(-Ortho * scale, Ortho*scale, -Ortho, Ortho);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
