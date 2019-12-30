#include"myOpenGL.h"
#include "ui_CG_Task_ZC_QT.h"

myOpenGL::myOpenGL(QWidget *parent) :QGLWidget(parent) {
}

myOpenGL::~myOpenGL() {}

void myOpenGL::initializeGL() {}

void myOpenGL::paintGL() {
	qDebug() << "paintGL......" << endl;
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	scanlineProcessor.CPT.clear();
	scanlineProcessor.CET.clear();
	obj.initObject(objName, winWidth, winHeight, algorithmChoose, isSpeedUp);
	qDebug() << "obj has been loaded......" << endl;
	bool loadObj = obj.loadObj(objName);
	fcount = obj.faces.size();
	vcount = obj.vertices.size();
	if (loadObj) {
		clock_t startTime, endTime;
		if (algorithmChoose == 1) {
			//��ʼ��ʱ
			startTime = clock();
			scanlineProcessor.ScanlineZBuffer(obj);
			qDebug() << "scanline z buffer mode......" << endl;
			displayModel();
		}
		else if (algorithmChoose == 2) {
			//��ʼ��ʱ
			startTime = clock();
			//TODO:����ɨ����
			qDebug() << "regional scanline mode......" << endl;
		}
		else {
			//û��ѡ�񣬲���Ҫ���в���
		}
		//������ʱ
		endTime = clock();
		time = (endTime - startTime) * 1000.0 / CLOCKS_PER_SEC;
	}
}

void myOpenGL::resizeGL(int width, int height){
	winWidth= width;
	winHeight = height;
	glViewport(0, 0, width, height);
	glEnable(GL_DEPTH_TEST);
}


void myOpenGL::displayModel() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); //�����ɫ�����Լ���Ȼ���
	//����ͼ��
	vector<GLfloat> framebuffer = scanlineProcessor.getframebuffer();
	glDrawPixels(obj.getWinWidth(), obj.getWinHeight(), GL_RGB, GL_FLOAT, &framebuffer[0]);
}

void myOpenGL::displayModel2() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); //�����ɫ�����Լ���Ȼ���
	//����ͼ��
	vector<GLfloat> framebuffer = scanlineProcessor.getframebuffer();
	glDrawPixels(obj.getWinWidth(), obj.getWinHeight(), GL_RGB, GL_FLOAT, &framebuffer[0]);
}