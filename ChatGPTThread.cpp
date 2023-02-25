#include "ChatGPTThread.h"
#include <qmessagebox.h>
#include <qjsonarray.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include "Config.h"
#include <QtCore/qcoreapplication.h>
ChatGptThread::ChatGptThread(QObject* parent)
	: QThread(parent)
{
}
void ChatGptThread::setQuery(const QString& query, const QString& context)
{
	m_query = query;
	m_context = context;
	if (sizeof(m_context) > 2000)
	{
		this->calContext();
	}
	m_query = m_context;
}

void ChatGptThread::run()
{
	// 构建请求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
	QMap<QString, QString> data1;
	Config* config = new Config();
	data1 = config->readConfig();
	QString key = data1["key"];
	delete config;
	QNetworkAccessManager manager;
	QString path = QCoreApplication::applicationDirPath() + "/config.ini";
	QNetworkRequest request(QUrl("https://api.openai.com/v1/completions"));
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
	request.setRawHeader("Authorization", "Bearer " + key.toUtf8());
	//QMessageBox::warning(NULL, "警告", key);
	QString s = "{\"model\":\"text-davinci-003\",\"prompt\":\"" + m_query.toUtf8() + "\",\"max_tokens\":2000,\"temperature\":0}";
	//QByteArray data = "{\"model\":\"text-davinci-003\",\"prompt\":\"\n\n Your : \n\n ChatGpt : \n\n Your : 我叫李京志\",\"max_tokens\":2000,\"temperature\":0}";
	QJsonObject jsonObject;
	jsonObject.insert("model", "text-davinci-003");
	jsonObject.insert("prompt", m_query);
	jsonObject.insert("max_tokens", 2000);
	jsonObject.insert("temperature", 0);
	QJsonDocument doc(jsonObject);
	QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
	QNetworkReply* reply = manager.post(request, jsonData);

	// 等待请求完成
	QEventLoop loop;
	QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
	loop.exec();

	// 处理响应
	if (reply->error() == QNetworkReply::NoError) {
		QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
		QJsonObject obj = doc.object();
		QJsonArray choices = obj.value("choices").toArray();
		QString text = choices[0].toObject().value("text").toString();
		emit responseReceived(text);
	}
	else {
		//QMessageBox::warning(this,"错误", reply->errorString());
		emit responseReceived(reply->errorString());
	}

	reply->deleteLater();
}

void ChatGptThread::calContext()
{
	//拆分为列表
	QStringList contextList = m_context.split("\n\n");
	int size = sizeof(m_context);//总的size
	int diffSize = size - 2000;//差值
	int removeSize = 0;//已经删除的值
	while (removeSize < diffSize)//如果删除的值小于差值
	{
		removeSize += sizeof(contextList[0]);
		contextList.remove(0);
	}
	m_context = "\n\n" + contextList.join("\n\n");
}
