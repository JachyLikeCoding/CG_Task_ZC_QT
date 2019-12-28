#include"myOpenGL.h"
#include "ui_CG_Task_ZC_QT.h"

myOpenGL::myOpenGL(QWidget *parent) :QGLWidget(parent) {
}

myOpenGL::~myOpenGL() {}

void myOpenGL::initializeGL() {}

void myOpenGL::paintGL() {
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	scanlineProcessor.CPT.clear();
	scanlineProcessor.CET.clear();
	obj.initObject(objName, winWidth, winHeight, algorithmChoose);
	if (obj.loadObj(objName)) {
		//开始计时
		clock_t startTime, endTime;
		startTime = clock();
		if (!algorithmChoose) {
			scanlineProcessor.ScanlineZBuffer(obj);
			qDebug() << "scanline z buffer mode......" << endl;
		}
		else {
			//TODO:区间扫描线
			qDebug() << "regional scanline mode......" << endl;
		}
		//结束计时
		endTime = clock();
		time = (endTime - startTime) * 1000.0 / CLOCKS_PER_SEC;
	}
}

void myOpenGL::resizeGL(int width, int height){
	Ortho = obj.getOrtho();
	winWidth = width;
	winHeight = height;
	GLfloat scale = GLfloat(width) / GLfloat(height);
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(-Ortho * scale, Ortho*scale, -Ortho, Ortho);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
