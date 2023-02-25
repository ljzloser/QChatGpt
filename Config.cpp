#include "Config.h"
#include <QDirIterator>
#include <QtCore/qcoreapplication.h>


void Config::writeConfig()
{
    QList<QString> allKeys = configData.keys();
    iniFile->beginGroup("set");
    for (QString key : allKeys)
    {
        iniFile->setValue(key, configData[key]);
    }
    //创建ini配置文件
    iniFile->endGroup();
}

QMap<QString, QString> Config::readConfig()
{
    QMap<QString, QString>  result;
    QStringList groupList = iniFile->childGroups();
    foreach(QString group, groupList) {
        iniFile->beginGroup(group);
        QStringList keyList = iniFile->allKeys();
        foreach(QString key, keyList) {
            result.insert(key, iniFile->value(key).toString());
        }
        iniFile->endGroup();
    }
    return result;
}

Config::Config(QMap<QString, QString> &configData)
{
    this->configData = configData;
    QString path = QCoreApplication::applicationDirPath() + "/config.ini";
    iniFile = new QSettings(path, QSettings::IniFormat);
}

Config::Config()
{
    QString path = QCoreApplication::applicationDirPath() + "/config.ini";
    iniFile = new QSettings(path, QSettings::IniFormat);
}

Config::~Config()
{

}
