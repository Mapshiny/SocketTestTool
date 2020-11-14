#include "widget.h"
#include "ui_widget.h"
#include "qdebug.h"


Widget::Widget(QWidget *parent) :
QWidget(parent),
ui(new Ui::Widget)
{
	ui->setupUi(this);
	this->initForm();
}

Widget::~Widget()
{
	delete ui;
}
void Widget::onAccept(){
	QTcpSocket* newSocket = tcpserver->nextPendingConnection();
	if(newSocket == nullptr){
		return;
	}
	connect(newSocket, &QTcpSocket::disconnected, this, &Widget::clientDisconnect);
	connect(newSocket, &QTcpSocket::readyRead, this, &Widget::readMsg);
	QString ipaddr = newSocket->peerAddress().toString();
	QString portstr = tr("%1").arg(newSocket->peerPort());
	QString msg = "new client[" + ipaddr + ":" + portstr + "] connected!";
	ui->dataReceive->append(msg);

	clientList.append(ipaddr + ":" + portstr);

	clientComboBox->addItem(ipaddr + ":" + tr("%1").arg(newSocket->peerPort()));
}
void Widget::readMsg(){
	QTcpSocket* MsgFrom = static_cast<QTcpSocket*>(sender());

    // QByteArray qba= MsgFrom->readAll();
	QByteArray qba = MsgFrom->read(20);

	QString ss = QVariant(qba).toString();
	QString client = MsgFrom->peerAddress().toString() + ":"+ tr("%1").arg(MsgFrom->peerPort());
	QString msg = "msgFrom:" + client + ":" + ss;

	ui->dataReceive->append(msg);
}
void Widget::clientDisconnect(){
	QTcpSocket* disconnectSocket = static_cast<QTcpSocket*>(sender());
	QString disconnectClient = disconnectSocket->peerAddress().toString() + ":"+ tr("%1").arg(disconnectSocket->peerPort());
//    QString msg = "client disconnected;[" + disconnectClient + "]";
	QString msg = "client[" + disconnectClient + "]disconnected;";
	ui->dataReceive->append(msg);

	int index = clientComboBox->findText(disconnectClient);
	clientComboBox->removeItem(index);
	disconnectSocket->deleteLater();
}

void Widget::on_connectBtn_clicked()
{
	if(ui->connectBtn->text() == "启动"){
		quint16 port = quint16(ui->Port->text().toUInt());
		qDebug() << port;
		tcpserver->listen(QHostAddress::AnyIPv4, port);
		if(tcpserver->isListening()){
//            connect(tcpserver, &QTcpServer::newConnection, this, &Widget::onAccept);
//            connect(tcpserver, &QTcpServer::acceptError, this, &Widget::displayError);
			QString msg = "listening...";
			ui->dataReceive->append(msg);
			ui->connectBtn->setText(tr("关闭"));
		}
		else{
			ui->connectBtn->setText(tr("启动"));
			ui->dataReceive->append(tr("listening error..."));
		}
	}
	else if(ui->connectBtn->text() == "关闭"){
		QList<QTcpSocket *>clients = tcpserver->findChildren<QTcpSocket *>();

		foreach(QTcpSocket *client, clients){
			client->close();
		}
		tcpserver->close();

		QString msg = "server closed...";
		ui->dataReceive->append(msg);
		if(ui->protocolCombox->currentIndex() == 0){
			ui->connectBtn->setText(tr("启动"));
		}
		else {
			ui->connectBtn->setText(tr("连接"));
			widgetStatus = false;
		}
	}
}

void Widget::initForm(){

	QString type = ui->protocolCombox->currentText();
	ui->sendBtn->setDisabled(true);
	widgetStatus = true;
	tcpserver = new QTcpServer();
	connect(tcpserver, &QTcpServer::newConnection, this, &Widget::onAccept);
	qDebug() << "init server:" << tcpserver;
	clientCboxLabel = new QLabel(tr("连接客户端:"));
	clientComboBox = new QComboBox();
	ui->horizontalLayout_3->addWidget(clientCboxLabel,2);
	ui->horizontalLayout_3->addWidget(clientComboBox,8);
	ui->IPaddr->setValidator(new QRegExpValidator(
		QRegExp("\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b")));
	ui->IPaddr->setText(tr("127.0.0.1"));
	ui->Port->setText(tr("8000"));
}

void Widget::on_protocolCombox_currentIndexChanged(int index)
{
	if(index == 0){
		if(!widgetStatus){
			clientCboxLabel->show();
			clientComboBox->show();

			if(!tcpserver->isListening()){
				qDebug() << tcpserver->isListening();
				ui->connectBtn->setText(tr("启动"));
			}

			widgetStatus = true;
		}
	}
	else{
		if(widgetStatus){
			clientCboxLabel->hide();
			clientComboBox->hide();

			widgetStatus = false;

			if(!tcpserver->isListening()){
				ui->connectBtn->setText(tr("连接"));
			}
		}

	}
}

void Widget::displayError(){
	ui->dataReceive->append("error!");
}

void Widget::on_clearMsgBtn_clicked()
{
	ui->dataReceive->clear();
}
