#include "Set.h"
#include "Config.h"
#include <QtCore/qfile.h>
#include <QMessageBox>
Set::Set(QWidget* parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	this->setFixedSize(400, 300);
	QMap<QString, QString> data1;
	Config* config = new Config();
	data1 = config->readConfig();
	this->style = data1["style"];
	this->key = data1["key"];
	delete config;
	QString stylePath = ":/qdarkstyle/light/lightstyle.qss";
	if (style == "黑色")
	{
		stylePath = ":/qdarkstyle/dark/darkstyle.qss";
		ui.comboBox->setCurrentIndex(0);
	}
	else
	{
		ui.comboBox->setCurrentIndex(1);
	}
	QFile f(stylePath);
	if (!f.exists()) {
		QMessageBox::warning(this, "警告", "没有找到样式表文件！");
	}
	else {
		f.open(QFile::ReadOnly | QFile::Text);
		QTextStream ts(&f);
		qApp->setStyleSheet(ts.readAll());
	}
	ui.lineEdit->setText(key);
	connect(ui.pushButton, &QPushButton::clicked, [=]()
		{
			QMap<QString, QString> data;
			data.insert("key", this->key);
			data.insert("style", this->style);
			Config* config = new Config(data);
			config->writeConfig();
			delete config;
			QString stylePath = ":/qdarkstyle/light/lightstyle.qss";
			if (style == "黑色")
			{
				stylePath = ":/qdarkstyle/dark/darkstyle.qss";
			}
			else
			{
			}
			QFile f(stylePath);
			if (!f.exists()) {
				QMessageBox::warning(this, "警告", "没有找到样式表文件！");
			}
			else {
				f.open(QFile::ReadOnly | QFile::Text);
				QTextStream ts(&f);
				qApp->setStyleSheet(ts.readAll());
			}
			this->close();
		});
	connect(ui.pushButton_back, &QPushButton::clicked, [=]()
		{
			this->close();
		});
	connect(ui.comboBox, &QComboBox::currentIndexChanged, [=](int index)
		{
			if (index == 0)
			{
				this->style = "黑色";
			}
			else
			{
				this->style = "白色";
			}
		});
}

Set::~Set()
{

}