#pragma once
#include <qsettings.h>
class Config
{
public:
	QSettings* iniFile;
	QMap<QString, QString> configData;
	void writeConfig();
	//读配置文件
	QMap<QString, QString> readConfig();
	Config(QMap<QString, QString> &configData);
	Config();
	~Config();
};

