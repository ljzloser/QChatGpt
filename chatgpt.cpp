#include "chatgpt.h"
#include <QSystemTrayIcon>
#include <qfile.h>
#include <qmessagebox.h>
#include <QCoreApplication>
#include <QShortcut>
#include <QtCore/qsettings.h>
#include <qdir.h>
#include <qmenu.h>
ChatGpt::ChatGpt(QWidget* parent)
	: QMainWindow(parent)
{
	QDir folder("Chat");
	if (!folder.exists()) {
		folder.mkpath(".");
	}
	ui.setupUi(this);
	//创建托盘图标对象
	this->trayIcon = new QSystemTrayIcon(QIcon(":/ChatGpt/icon.png"));
	// 创建线程实例
	this->thread = new ChatGptThread(this);
	ui.textEdit->setInputMethodHints(Qt::ImhNone);//禁用输入法输入
	connect(thread, &ChatGptThread::responseReceived, this, &ChatGpt::handleResponse);
	set = new Set(this);

	// 显示托盘图标
	trayIcon->show();



	//增加托盘菜单
	this->menu = new QMenu(this);
	QAction* setAction = new QAction("设置");
	QAction* quitAction = new QAction("退出");
	menu->addAction(setAction);
	menu->addAction(quitAction);
	connect(setAction, &QAction::triggered, [=]()
		{
			this->hide();//主窗口隐藏
			this->set->exec();//显示设置窗口
		});
	connect(quitAction, &QAction::triggered, [=]()
		{
			QCoreApplication::quit();//退出程序
		});
	connect(trayIcon, &QSystemTrayIcon::activated, this, &ChatGpt::onActivated);
	setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
	this->setWindowIcon(QIcon(":/ChatGpt/icon.png"));
	sendAction = new QAction("发送");
	sendAction->setIcon(QPixmap(":/ChatGpt/send.png"));
	ui.lineEdit->addAction(sendAction, QLineEdit::TrailingPosition);
	connect(sendAction, &QAction::triggered, [=]()
		{
			// 启动线程
			if (!thread->isRunning())
			{
				if (ui.lineEdit->text() != "")
				{
					ui.textEdit->append("\n\n Your : " + ui.lineEdit->text());
					thread->setQuery(ui.lineEdit->text(), start + ui.textEdit->toPlainText());
					ui.lineEdit->clear();
					ui.lineEdit->setReadOnly(true);
					sendAction->setIcon(QPixmap(":/ChatGpt/wait.png"));
					QFile* file;
					if (isNewChat)
					{
						file = new QFile(path + QString("/Chat/chat%1.txt").arg(ui.treeWidget->topLevelItemCount()));
						int row = 0;
						if (ui.treeWidget->topLevelItemCount() > 0)
						{
							row = ui.treeWidget->topLevelItem(ui.treeWidget->topLevelItemCount() - 1)->text(0).split("t")[1].toInt() + 1;
						}
						QString filename = QString("chat%1").arg(row);
						QTreeWidgetItem* item = new QTreeWidgetItem(ui.treeWidget, filename.split(","));
						ui.treeWidget->setCurrentItem(item);
						isNewChat = false;
					}
					else
					{
						file = new QFile(path + "/Chat/" + ui.treeWidget->currentItem()->text(0) + ".txt");
					}
					if (file->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
						QTextStream out(file);
						out << ui.textEdit->toPlainText();
						file->close();
					}
					ui.pushButton->setDisabled(true);
					ui.treeWidget->setDisabled(true);
					ui.pushButton_2->setDisabled(true);
					thread->start();
				}
			}
		});
	//处理QTreeWidget切换事件
	connect(ui.treeWidget, &QTreeWidget::currentItemChanged, [=](QTreeWidgetItem* item)
		{
			if (!this->isNewChat || ui.treeWidget->currentItem())
			{
				QString path = QString(this->path + "/Chat/%1.txt").arg(item->text(0));
				QFile file(path);
				if (file.open(QIODevice::ReadOnly | QIODevice::Text))
				{
					QTextStream in(&file);
					QString content = in.readAll();
					ui.textEdit->setPlainText(content);
					file.close();
				}
				this->isNewChat = false;
			}
		});
	// 创建一个 QShortcut 对象，指定触发回车键
	QShortcut* shortcut = new QShortcut(QKeySequence(Qt::Key_Return), ui.lineEdit);

	// 连接一个槽函数，实现在回车键按下时的操作
	connect(shortcut, &QShortcut::activated, this, [=]() {
		// 启动线程
		if (!thread->isRunning())
		{
			if (ui.lineEdit->text() != "")
			{
				ui.textEdit->append("\n\n Your : " + ui.lineEdit->text());
				thread->setQuery(ui.lineEdit->text(), start + ui.textEdit->toPlainText());
				ui.lineEdit->clear();
				ui.lineEdit->setReadOnly(true);
				sendAction->setIcon(QPixmap(":/ChatGpt/wait.png"));
				QFile* file;
				if (isNewChat)
				{
					file = new QFile(path + QString("/Chat/chat%1.txt").arg(ui.treeWidget->topLevelItemCount()));
					int row = 0;
					if (ui.treeWidget->topLevelItemCount() > 0)
					{
						row = ui.treeWidget->topLevelItem(ui.treeWidget->topLevelItemCount() - 1)->text(0).split("t")[1].toInt() + 1;
					}
					QString filename = QString("chat%1").arg(row);
					QTreeWidgetItem* item = new QTreeWidgetItem(ui.treeWidget, filename.split(","));
					ui.treeWidget->setCurrentItem(item);
					isNewChat = false;
				}
				else
				{
					file = new QFile(path + "/Chat/" + ui.treeWidget->currentItem()->text(0) + ".txt");
				}
				if (file->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
					QTextStream out(file);
					out << ui.textEdit->toPlainText();
					file->close();
				}
				ui.pushButton->setDisabled(true);
				ui.treeWidget->setDisabled(true);
				ui.pushButton_2->setDisabled(true);
				thread->start();
			}
		}
		});


	//ui.textEdit->setLineWrapMode(QTextEdit::NoWrap);
	ui.textEdit->setReadOnly(true);
	QString appName = QApplication::applicationName();//程序名称
	QString appPath = QApplication::applicationFilePath();// 程序路径
	appPath = appPath.replace("/", "\\");

	QSettings* reg = new QSettings(
		"HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
		QSettings::NativeFormat);

	QString val = reg->value(appName).toString();// 如果此键不存在，则返回的是空字符串
	if (val != appPath)
		reg->setValue(appName, appPath);// 如果移除的话，reg->remove(applicationName);

	reg->deleteLater();

	ui.pushButton_3->setIcon(QIcon(":/ChatGpt/down.png"));

	ui.treeWidget->clear();
	ui.treeWidget->setColumnCount(1);
	ui.treeWidget->setHeaderLabels({ "聊天列表" });
	QString chatPath = path + "/Chat";
	QDir dir(chatPath);
	QStringList filename;
	filename << "*.txt";
	QStringList results;
	results = dir.entryList(filename, QDir::Files | QDir::Readable, QDir::Name);
	for (QString var : results)
	{
		QTreeWidgetItem* Item = new QTreeWidgetItem(ui.treeWidget, var.split("."));
	}
	ui.treeWidget->clearSelection();
	ui.treeWidget->setCurrentItem(nullptr);
	//点击就情况一些东西
	connect(ui.pushButton, &QPushButton::clicked, [=]()
		{
			ui.textEdit->clear();
			ui.lineEdit->clear();
			this->isNewChat = true;
			ui.treeWidget->clearSelection();
		});
	connect(ui.pushButton_2, &QPushButton::clicked, [=]()
		{
			if (ui.treeWidget->currentItem())
			{
				QTreeWidgetItem* item = ui.treeWidget->currentItem();
				QString strPath = path + "/Chat/" + item->text(0) + ".txt";
				if (!(strPath.isEmpty() || !QDir().exists(strPath)))
				{
					QFileInfo FileInfo(strPath);

					QFile::remove(strPath);
				}
				delete item;
				ui.textEdit->clear();
				ui.lineEdit->clear();
				this->isNewChat = true;
				ui.treeWidget->clearSelection();
			}
		});
	connect(ui.pushButton_3, &QPushButton::clicked, [=]()
		{
			if (isTop)
			{
				isTop = false;
				ui.pushButton_3->setIcon(QIcon(":/ChatGpt/down.png"));
			}
			else
			{
				isTop = true;
				ui.pushButton_3->setIcon(QIcon(":/ChatGpt/on.png"));
			}
		});
}

ChatGpt::~ChatGpt()
{}
void ChatGpt::leaveEvent(QEvent* event)
{
	Q_UNUSED(event);
	// 鼠标当前位置
	QPoint cursorPos = QCursor::pos();
	// 窗口的位置和大小
	QRect widgetGeometry = geometry();
	// 判断鼠标是否在窗口内
	if (!widgetGeometry.contains(cursorPos) && !isTop) {
		// 隐藏窗口
		hide();
	}
}
void ChatGpt::handleResponse(const QString& response)
{
	ui.textEdit->append(response);
	ui.lineEdit->setReadOnly(false);
	sendAction->setIcon(QPixmap(":/ChatGpt/send.png"));
	QFile* file;
	file = new QFile(path + "/Chat/" + ui.treeWidget->currentItem()->text(0) + ".txt");
	if (file->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
		QTextStream out(file);
		out << ui.textEdit->toPlainText();
		file->close();
		ui.pushButton->setDisabled(false);
		ui.treeWidget->setDisabled(false);
		ui.pushButton_2->setDisabled(false);

	}
}
