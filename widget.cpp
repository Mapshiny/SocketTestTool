#include "widget.h"
#include "ui_widget.h"
#include "qdebug.h"
#include <QMessageBox>
#include <tcpclient.h>
#include <QDateTime>
#include <QNetworkDatagram>
#include <QAbstractSocket>

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

    clientComboBox->addItem(ipaddr + ":" + portstr, qv);
}
void Widget::sendMsg(QAbstractSocket* socket) {
    qDebug() << socket;

    QString time = "[" + QDateTime::currentDateTime().toString("yyyy.MM.dd.hh.mm.ss") + "]";

    QString strDate = ui->showDate->isChecked() ? time : "";

    QString strText = ui->sendMsg->toPlainText();
    QByteArray data = strText.toStdString().data();
    qint64 len = qint64(strlen(data));
    sendCount += len;
    if(strText == "") {
        QMessageBox::information(nullptr, "提示", "发送数据为空，请输入内容。",QMessageBox::Ok,QMessageBox::Ok);
    }
    if(socket->socketType() == QAbstractSocket::UdpSocket) {
        QUdpSocket* sock = static_cast<QUdpSocket*>(socket);
        QHostAddress hostAddr(ui->IPaddr->text());
        quint16 port = quint16(ui->Port->text().toUInt());
        sock->writeDatagram(data, hostAddr, port);
        QString addr = hostAddr.toString();
        QString Strport = tr("%1").arg(port);
        QString msg = strDate + "SendTo[" + addr + ":" + Strport + "]: " + data;

        ui->sendCount->setText(tr("%1").arg(sendCount));
        ui->dataReceive->append(msg);
        return;
    }
    else {
        socket->write(data, len);
        QString addr = socket->peerAddress().toString();
        QString port = tr("%1").arg(socket->peerPort());
        QString msg = strDate + "SendTo[" + addr + ":" + port + "]: " + data;

        ui->sendCount->setText(tr("%1").arg(sendCount));
        ui->dataReceive->append(msg);
    }
}
void Widget::readMsg(){
    QByteArray qba;
    QString host, port, client;
    QAbstractSocket* socket = static_cast<QAbstractSocket*>(sender());
    qDebug() << socket->socketType();
    if(socket->socketType() == QAbstractSocket::UdpSocket) {
        while (udpServer->hasPendingDatagrams()) {
            QNetworkDatagram datagram = udpServer->receiveDatagram(1024);
            qba = datagram.data();
            host = datagram.senderAddress().toString();
            port = tr("%1").arg(datagram.senderPort());
            client = host + ":" + port;
        }
    }
    else {
        QTcpSocket* MsgFrom = static_cast<QTcpSocket*>(socket);
        // QByteArray qba= MsgFrom->readAll();
        qba = MsgFrom->read(1024);
        host = MsgFrom->peerAddress().toString();
        port = tr("%1").arg(MsgFrom->peerPort());
        client = host + ":" + port;
    }

    receiveCount += qba.length();
    QString ss = QVariant(qba).toString();
    QString msg = "msgFrom[" + client + "]: " + ss;
    ui->dataReceive->append(msg);
    ui->receiveCount->setText(tr("%1").arg(receiveCount));

}
void Widget::clientDisconnect() {
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

void Widget::initForm(){

	QString type = ui->protocolCombox->currentText();
	ui->sendBtn->setDisabled(true);
    sendCount = 0;
    receiveCount = 0;

    ServerStatus = false;
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
void Widget::tcpServerOpen(quint16 port) {
    tcpserver->listen(QHostAddress::AnyIPv4, port);
    if(tcpserver->isListening()){
        QString msg = "listening...";
        ui->dataReceive->append(msg);
        ui->connectBtn->setText(tr("关闭"));
        ui->protocolCombox->setDisabled(true);
        ServerStatus = true;
    }
}
void Widget::tcpServerClose() {
    QList<QTcpSocket *>clients = tcpserver->findChildren<QTcpSocket *>();
    foreach(QTcpSocket *client, clients) {
        client->close();
    }
    tcpserver->close();
    ServerStatus = false;
    QString msg = "tcp server closed...";
    ui->dataReceive->append(msg);
    ui->connectBtn->setText(tr("启动"));
    ui->protocolCombox->setDisabled(false);
}
void Widget::tcpClientConnect(QHostAddress hostAddr, quint16 port) {
    clientSocket = new QTcpSocket();
    connect(clientSocket, &QTcpSocket::readyRead, this, &Widget::readMsg);
    clientSocket->connectToHost(hostAddr, port);
    if(clientSocket->waitForConnected(1000)){
        QString msg = "connect to host[" + hostAddr.toString() + ":" + tr("%1").arg(port) + "]";
        ui->dataReceive->append(msg);
        ui->connectBtn->setText(tr("断开"));
        ui->sendBtn->setEnabled(true);
        ServerStatus = true;
        ui->protocolCombox->setDisabled(true);
    }
    else{
        ui->dataReceive->append("connect error...");
    }
}
void Widget::tcpClientDisconnect() {
    clientSocket->disconnectFromHost();
    ServerStatus = false;
    ui->sendBtn->setDisabled(true);
    if (clientSocket->state() == QAbstractSocket::UnconnectedState || clientSocket->waitForDisconnected(1000)){
        QString msg = "disconnect from host...";
        ui->dataReceive->append(msg);
        ui->connectBtn->setText(tr("连接"));
        ui->protocolCombox->setDisabled(false);
    }
}
void Widget::udpServerOpen(quint16 port) {
    udpServer = new QUdpSocket();
    udpServer->bind(QHostAddress::LocalHost, port);
    connect(udpServer, &QUdpSocket::readyRead, this, &Widget::readMsg);
    QString msg = "udp server listening...";
    ui->dataReceive->append(msg);
    ui->connectBtn->setText(tr("关闭"));
    ui->protocolCombox->setDisabled(true);
    ServerStatus = true;
}
void Widget::udpServerClose() {
    udpServer->close();
    ui->protocolCombox->setEnabled(true);
    ServerStatus = false;
    ui->connectBtn->setText(tr("启动"));
    QString msg = "udp server closed...";
    ui->dataReceive->append(msg);
}

void Widget::displayError(){
    ui->dataReceive->append("display error!");
}
void Widget::on_connectBtn_clicked()
{
    QHostAddress hostAddr(ui->IPaddr->text());
    quint16 port = quint16(ui->Port->text().toUInt());
    int index = ui->protocolCombox->currentIndex();
    if(ServerStatus) {
        switch (index) {
        case 0 : tcpServerClose(); break;
        case 1 : tcpClientDisconnect(); break;
        case 2 : udpServerClose(); break;
//        case 3 : udpClientDisconnect(); break;
        default: return;
        }
    }
    else {
        switch (index) {
        case 0 : tcpServerOpen(port); break;
        case 1 : tcpClientConnect(hostAddr, port); break;
        case 2 : udpServerOpen(port); break;
//        case 3 : udpClientConnect(hostAddr, port); break;
        default: break;
        }
    }
}
void Widget::on_protocolCombox_currentIndexChanged(int index){
    if(ServerStatus){ return; }
    ui->sendBtn->setDisabled(true);
    udpClient = nullptr;
    clientCboxLabel->hide();
    clientComboBox->hide();
    if(index == 0) {
        clientCboxLabel->show();
        clientComboBox->show();
        if(!tcpserver->isListening()){
            ui->connectBtn->setText(tr("启动"));
        }
    }
    if(index == 1) {
        ui->connectBtn->setText(tr("连接"));
    }
    if(index == 2) {
        ui->connectBtn->setText(tr("启动"));
    }
    if(index == 3) {
        udpClient = new QUdpSocket();
        ui->connectBtn->setText(tr("udp无需连接"));
        ui->sendBtn->setEnabled(true);
    }
}
void Widget::on_sendBtn_clicked() {
    if(ui->protocolCombox->currentIndex() == 0) {
        QTcpSocket* currentSocket = clientComboBox->currentData().value<QTcpSocket*>();
        sendMsg(currentSocket);
    }
    if(ui->protocolCombox->currentIndex() == 1) {
        sendMsg(clientSocket);
    }
    if(ui->protocolCombox->currentIndex() == 3) {
        sendMsg(udpClient);
    }
}

void Widget::on_clearMsgBtn_clicked() {
	ui->dataReceive->clear();
}

void Widget::on_pushButton_4_clicked() {
    receiveCount = 0;
    sendCount = 0;
    ui->receiveCount->setText(" ");
    ui->sendCount->setText(" ");
}
