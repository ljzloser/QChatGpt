#ifndef SINGLEAPPLICATION_H
#define SINGLEAPPLICATION_H

#include <QObject>
#include <QApplication>
class QMainWindow;
class QLocalServer;

class SingleApplication : public QApplication
{
    Q_OBJECT
public:
    SingleApplication(int &args, char **argv);
    bool getInstanceRunning() const; // 是否已经有实例在运行
    QMainWindow *getMainWindow() const;

    void setMainWindow(QMainWindow *value);

private slots:
    void newLocalConnection();

private:
    void initLocalConnection();  // 初始化本地连接
    void newLocalServer();       // 创建服务端
    bool instanceRunning;        // 是否已经有实例在运行
    QLocalServer *localServer;   // 本地socket Server（用于本地进程间通信）
    QString serverName;          // 服务名称
    QMainWindow *mainWindow;  // MainWindow指针
};

#endif // SINGLEAPPLICATION_H

