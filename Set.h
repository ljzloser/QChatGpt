#pragma once

#include <QDialog>
#include "ui_Set.h"

class Set : public QDialog
{
	Q_OBJECT

public:
	Set(QWidget *parent = nullptr);
	~Set();
	QString style;
	QString key;
private:
	Ui::SetClass ui;
};
