#include "widget.h"
#include "ui_widget.h"
#include "qdebug.h"
#include <QMessageBox>
#include <tcpclient.h>

Widget::Widget(QWidget *parent) :
QWidget(parent),
ui(new Ui::Widget)
{
	ui->setupUi(this);
	this->initForm();
}

Widget::~Widget(){
	delete ui;
}
void Widget::test(){
    ui->dataReceive->append("here is test function!!");
    TcpClient* tc = new TcpClient();
    quint16 port = quint16(ui->Port->text().toUInt());
    qDebug() << port;
    QHostAddress hostAddr(ui->IPaddr->text());
    tc->connectToHost(hostAddr, port);
}

void Widget::onAccept(){
	QTcpSocket* newSocket = tcpserver->nextPendingConnection();
	ui->sendBtn->setDisabled(false);

	connect(newSocket, &QTcpSocket::disconnected, this, &Widget::clientDisconnect);
    connect(newSocket, &QTcpSocket::readyRead, this, &Widget::readMsg);

	QString ipaddr = newSocket->peerAddress().toString();
	QString portstr = tr("%1").arg(newSocket->peerPort());
	QString msg = "new client[" + ipaddr + ":" + portstr + "] connected!";
	ui->dataReceive->append(msg);

	clientList.append(ipaddr + ":" + portstr);
    QVariant qv;
    qv.setValue(newSocket);
    qDebug() << qv;
    clientComboBox->addItem(ipaddr + ":" + portstr, qv);
}
void Widget::sendMsg(){
    qDebug() << "sendMsg!";
}
void Widget::readMsg(){
	QTcpSocket* MsgFrom = static_cast<QTcpSocket*>(sender());

    // QByteArray qba= MsgFrom->readAll();
    QByteArray qba = MsgFrom->read(1024);
    receiveCount += qba.length();
	QString ss = QVariant(qba).toString();
	QString client = MsgFrom->peerAddress().toString() + ":"+ tr("%1").arg(MsgFrom->peerPort());
    QString msg = "msgFrom[" + client + "]: " + ss;

	ui->dataReceive->append(msg);
    ui->receiveCount->setText(tr("%1").arg(receiveCount));
}
void Widget::clientDisconnect(){
	QTcpSocket* disconnectSocket = static_cast<QTcpSocket*>(sender());
	QString disconnectClient = disconnectSocket->peerAddress().toString() + ":"+ tr("%1").arg(disconnectSocket->peerPort());
//    QString msg = "client disconnected;[" + disconnectClient + "]";
	QString msg = "client[" + disconnectClient + "]disconnected;";
	ui->dataReceive->append(msg);

	int index = clientComboBox->findText(disconnectClient);
	clientComboBox->removeItem(index);

	ui->sendBtn->setDisabled(!clientComboBox->count());
	disconnectSocket->deleteLater();
}

void Widget::on_connectBtn_clicked()
{
    QHostAddress hostAddr(ui->IPaddr->text());
    quint16 port = quint16(ui->Port->text().toUInt());
	if(ui->connectBtn->text() == "启动"){
		tcpserver->listen(QHostAddress::AnyIPv4, port);
		if(tcpserver->isListening()){
//            connect(tcpserver, &QTcpServer::newConnection, this, &Widget::onAccept);
//            connect(tcpserver, &QTcpServer::acceptError, this, &Widget::displayError);
			QString msg = "listening...";
			ui->dataReceive->append(msg);
			ui->connectBtn->setText(tr("关闭"));
            ProStatus = true;
		}
		else{
			ui->connectBtn->setText(tr("启动"));
			ui->dataReceive->append(tr("listening error..."));
            ProStatus = false;
		}
	}
	else if(ui->connectBtn->text() == "关闭"){
		QList<QTcpSocket *>clients = tcpserver->findChildren<QTcpSocket *>();

		foreach(QTcpSocket *client, clients){
			client->close();
		}
		tcpserver->close();
        ProStatus = false;
		QString msg = "server closed...";
		ui->dataReceive->append(msg);
		if(ui->protocolCombox->currentIndex() == 0){
			ui->connectBtn->setText(tr("启动"));
            widgetStatus = true;
		}
		else {
			ui->connectBtn->setText(tr("连接"));
			widgetStatus = false;
		}
	}
	else if(ui->connectBtn->text() == "连接"){

		clientSocket = new QTcpSocket();
        connect(clientSocket, &QTcpSocket::readyRead, this, &Widget::readMsg);
        qDebug() << hostAddr << ":" << port;
        clientSocket->connectToHost(hostAddr, port);

        if(clientSocket->waitForConnected(1000)){
            QString msg = "connect to host[" + hostAddr.toString() + ":" + tr("%1").arg(port) + "]";
            ui->dataReceive->append(msg);
            ui->connectBtn->setText(tr("断开"));
            ui->sendBtn->setEnabled(true);
            ProStatus = true;
        }
        else{
            ui->dataReceive->append("connect error...");
        }
	}
    else if(ui->connectBtn->text() == "断开"){
        clientSocket->disconnectFromHost();
        ProStatus = false;
        ui->sendBtn->setDisabled(true);
        if (clientSocket->state() == QAbstractSocket::UnconnectedState || clientSocket->waitForDisconnected(1000)){
            QString msg = "disconnect from host...";
            ui->dataReceive->append(msg);
            if(ui->protocolCombox->currentIndex() == 0){
                ui->connectBtn->setText(tr("启动"));
                widgetStatus = true;
            }
            else {
                ui->connectBtn->setText(tr("连接"));
                widgetStatus = false;
            }
        }
    }
}

void Widget::initForm(){

	QString type = ui->protocolCombox->currentText();
	ui->sendBtn->setDisabled(true);
    sendCount = 0;
    receiveCount = 0;
	widgetStatus = true;
    ProStatus = false;
	tcpserver = new QTcpServer();

	connect(tcpserver, &QTcpServer::newConnection, this, &Widget::onAccept);

	clientCboxLabel = new QLabel(tr("连接客户端:"));
	clientComboBox = new QComboBox();
	ui->horizontalLayout_3->addWidget(clientCboxLabel,2);
	ui->horizontalLayout_3->addWidget(clientComboBox,8);
	ui->IPaddr->setValidator(new QRegExpValidator(
		QRegExp("\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b")));
	ui->IPaddr->setText(tr("127.0.0.1"));
	ui->Port->setText(tr("8000"));
//    test();
}

void Widget::on_protocolCombox_currentIndexChanged(int index){
    if(ProStatus){ return; }
    if(index == 0){
        if(!widgetStatus){
            clientCboxLabel->show();
            clientComboBox->show();

            if(!tcpserver->isListening()){
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
//	if(index == 0){
//		if(!widgetStatus){
//			clientCboxLabel->show();
//			clientComboBox->show();

//			if(!tcpserver->isListening()){
//				ui->connectBtn->setText(tr("启动"));
//			}
//			widgetStatus = true;
//		}
//	}
//	else{
//		if(widgetStatus){
//			clientCboxLabel->hide();
//			clientComboBox->hide();

//			widgetStatus = false;

//			if(!tcpserver->isListening()){
//				ui->connectBtn->setText(tr("连接"));
//			}
//		}
//	}
}

void Widget::on_sendBtn_clicked(){
    QString strText = ui->sendMsg->toPlainText();
    QByteArray data = strText.toStdString().data();
    qint64 len = qint64(strlen(data));
    sendCount += len;
    ui->sendCount->setText(tr("%1").arg(sendCount));
    if(strText == ""){
        QMessageBox::information(nullptr, "提示", "发送数据为空，请输入内容。",QMessageBox::Ok,QMessageBox::Ok);
    }
    else if(ui->connectBtn->text() == "关闭"){
        QTcpSocket* currentSocket = clientComboBox->currentData().value<QTcpSocket*>();
        currentSocket->write(data, len);
        QString addr = currentSocket->peerAddress().toString();
        QString port = tr("%1").arg(currentSocket->peerPort());
        QString msg = "SendToClient[" + addr + ":" + port + "]: " + data;
        ui->dataReceive->append(msg);

    }
    else if(ui->connectBtn->text() == "断开"){
        clientSocket->write(data, len);
        QString addr = clientSocket->peerAddress().toString();
        QString port = tr("%1").arg(clientSocket->peerPort());
        QString msg = "SendToHost[" + addr + ":" + port + "]: " + data;
        ui->dataReceive->append(msg);
    }
}

void Widget::on_clearMsgBtn_clicked(){
	ui->dataReceive->clear();
}
void Widget::displayError(){
    ui->dataReceive->append("error!");
}

void Widget::on_pushButton_4_clicked()
{
    receiveCount = 0;
    sendCount = 0;
    ui->receiveCount->setText(" ");
    ui->sendCount->setText(" ");
}
