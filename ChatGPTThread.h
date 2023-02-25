#pragma once
#include <qthread.h>
#include <QtCore/qeventloop.h>
#include <QtNetwork/qnetworkaccessmanager.h>
#include<QtNetwork/qnetworkreply.h>


// 定义一个线程类
class ChatGptThread : public QThread
{
    Q_OBJECT

public:
    ChatGptThread(QObject* parent = nullptr);

    void setQuery(const QString& query, const QString& context);

signals:
    void responseReceived(const QString& response);

protected:
    void run() override;
    void calContext();

private:
    QString m_query;
    QString m_context;
};

