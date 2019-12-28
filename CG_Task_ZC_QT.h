#pragma once

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QWidget>
#include <QDebug>
#include <QButtonGroup>
#include <QtGui>
#include <QtCore>
#include <QRect>
#include <QFileDialog>
#include <QMessageBox>
#include "ui_CG_Task_ZC_QT.h"

class CG_Task_ZC_QT : public QMainWindow
{
	Q_OBJECT

public:
	CG_Task_ZC_QT(QWidget *parent = Q_NULLPTR);

public slots:
		void displayModeChoose();
		void algorithmChoose();
		void speedUpChoose();
		void getTime();
		//void scanlineZbuffer();
		void zoomin();
		void zoomout();
		void keyPressEvent(QKeyEvent *e);
		//void mousePressEvent(QMouseEvent *event);
		//void mousehandle();
		void openFile();

private:
	Ui::CG_Task_ZC_QTClass ui;
	QButtonGroup* algorithmChooseGroup;
	QButtonGroup* speedUpChooseGroup;
	QButtonGroup* DisplayModeChooseGroup;
	GLfloat rTri;
	bool fullscreen;
};
