#include "CG_Task_ZC_QT.h"

CG_Task_ZC_QT::CG_Task_ZC_QT(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	rTri = 0.0;
	fullscreen = false;
	algorithmChooseGroup = new QButtonGroup(this);
	algorithmChooseGroup->addButton(ui.scanlineZBuffer, 0);
	algorithmChooseGroup->addButton(ui.regionalScanline, 1);

	speedUpChooseGroup = new QButtonGroup(this);
	speedUpChooseGroup->addButton(ui.noSpeedUp, 0);
	speedUpChooseGroup->addButton(ui.speedUp, 1);
	ui.speedUp->setChecked(true);

	connect(ui.speedUp, SIGNAL(clicked(bool)), this, SLOT(speedUpChoose()));
	connect(ui.noSpeedUp, SIGNAL(clicked(bool)), this, SLOT(speedUpChoose()));
	connect(ui.scanlineZBuffer, SIGNAL(clicked(bool)), this, SLOT(algorithmChoose()));
	connect(ui.regionalScanline, SIGNAL(clicked(bool)), this, SLOT(algorithmChoose()));
	connect(ui.startDraw, SIGNAL(clicked()), this, SLOT(getTime()));
	connect(ui.startDraw, SIGNAL(clicked()), this, SLOT(drawResult()));
	connect(ui.zoomin, SIGNAL(clicked()), this, SLOT(zoomin()));
	connect(ui.zoomout, SIGNAL(clicked()), this, SLOT(zoomout()));
	connect(ui.modelChoose, SIGNAL(clicked()), this, SLOT(openFile()));

	ui.modelChoose->setStyleSheet("border:2px groove gray;border-radius:10px;padding:2px 4px;");
	ui.zoomin->setStyleSheet("border:2px groove gray;border-radius:10px;padding:2px 4px;");
	ui.zoomout->setStyleSheet("border:2px groove gray;border-radius:10px;padding:2px 4px;");
	ui.startDraw->setStyleSheet(
		"border:2px navajowhite;border-radius:10px;padding:2px 4px;"
		"background-color: navajowhite;"
		"selection-color: gray;"
		"selection-background-color: gray;");
}

void CG_Task_ZC_QT::algorithmChoose()
{
	switch (algorithmChooseGroup->checkedId())
	{
	case 0:
		ui.openGLWidget->algorithmChoose = 1;
		qDebug() << "scanline-z-buffer" << endl;
		break;
	case 1:
		ui.openGLWidget->algorithmChoose = 2;
		qDebug() << "regional-scanline" << endl;
		break;
	default:
		break;
	}
}



void CG_Task_ZC_QT::speedUpChoose()
{
	switch (speedUpChooseGroup->checkedId())
	{
	case 0:
		ui.openGLWidget->isSpeedUp = 0;
		qDebug() << "no speed up" << endl;
		break;
	case 1:
		ui.openGLWidget->isSpeedUp = 1;
		qDebug() << "speed up! hurry up!" << endl;
		break;
	default:
		break;
	}
}

void CG_Task_ZC_QT::keyPressEvent(QKeyEvent *e)
{
	switch (e->key())
	{
		//F1键为全屏和普通屏显示切换键
	case Qt::Key_F1:
		fullscreen = !fullscreen;
		if (fullscreen)
			showFullScreen();
		else
			showNormal();
		ui.openGLWidget->updateGL();
		break;
		//Ese为退出程序键
	case Qt::Key_Escape:
		close();
	}
}

void CG_Task_ZC_QT::openFile()
{
	QString openFile = "";
	QString s = QDir::currentPath();
	//QString s = QCoreApplication::applicationDirPath();
	//文件对话框获取要打开的文件名
	openFile = QFileDialog::getOpenFileName(this, "Open", s, "model file(*.obj *.txt)");
	//打印要打开的文件名
	QMessageBox::information(this, "Open", openFile);
	ui.openGLWidget->objName = openFile;
}

void CG_Task_ZC_QT::zoomout() {
	qDebug() << "zoomout" << endl;
	ui.openGLWidget->updateGL();
}

void CG_Task_ZC_QT::zoomin() {
	qDebug() << "zoomin" << endl;
	glScalef(2, 2, 2);
	ui.openGLWidget->updateGL();
}

void CG_Task_ZC_QT::getTime()
{
	QString time = QString::number(ui.openGLWidget->time);
	ui.timeNum->setText(time + " ms");
	QString faceCount = QString::number(ui.openGLWidget->fcount);
	qDebug() << "face Count = " << faceCount << endl;
	ui.faceNum->setText(faceCount);
	QString vCount = QString::number(ui.openGLWidget->vcount);
	qDebug() << "vertex Count = " << vCount << endl;
	ui.verticesNum->setText(vCount);
}

void CG_Task_ZC_QT::drawResult() {
	ui.openGLWidget->updateGL();
	qDebug() << "drawResult! " << endl;
}