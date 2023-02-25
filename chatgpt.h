#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_chatgpt.h"
#include <QSystemTrayIcon>
#include "ChatGPTThread.h"
#include "qdir.h"
#include <qmenu.h>
#include "Set.h"
class ChatGpt : public QMainWindow
{
    Q_OBJECT

public:
    ChatGpt(QWidget *parent = nullptr);
    ~ChatGpt();
    ChatGptThread* thread;
    void leaveEvent(QEvent* event);
    void handleResponse(const QString& response);
    QSystemTrayIcon* trayIcon;
    QAction* sendAction;
    const QString start = "\n\n Your : \n\n ChatGpt : ";
    bool isNewChat = true;
    QString path = QCoreApplication::applicationDirPath();//当前工作路径
    bool isTop = false;
private:
    Ui::ChatGptClass ui;
    QMenu* menu;
    Set* set;
private slots:
    void onActivated(QSystemTrayIcon::ActivationReason reason) {
        if (reason == QSystemTrayIcon::Trigger) {
            // 点击托盘图标时，显示窗口
            if (this->isVisible())
            {
                this->hide();
            }
            else
            {
                setWindowFlags(Qt::FramelessWindowHint);
                setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::Tool);

                // 获取托盘图标的矩形区域
                QRect rect = trayIcon->geometry();

                // 计算窗口的位置，使其显示在托盘图标的正上方
                int x = rect.x() - this->width() + rect.width();
                int y = rect.y() - this->height();

                // 将窗口移动到指定位置
                this->move(x, y);
                this->show();     
                this->raise();
            }
            
        }
        else if (reason == QSystemTrayIcon::Context) {
            // 在鼠标右键单击时显示托盘菜单
            menu->exec(QCursor::pos());
        }
    }
};
