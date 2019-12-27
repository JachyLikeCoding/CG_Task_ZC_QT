#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_CG_Task_ZC_QT.h"

class CG_Task_ZC_QT : public QMainWindow
{
	Q_OBJECT

public:
	CG_Task_ZC_QT(QWidget *parent = Q_NULLPTR);

private:
	Ui::CG_Task_ZC_QTClass ui;
};
