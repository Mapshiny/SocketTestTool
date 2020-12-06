#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTcpServer>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QTimer>

class QComboBox;
class QLabel;
namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    QTcpServer* tcpserver;
    QTcpSocket* clientSocket;
    QUdpSocket* udpServer;
    QUdpSocket* udpClient;
    void initForm();
    void onAccept();
    void clientDisconnect();
    void readMsg();
    void displayError();
    void test();
    void sendMsg(QAbstractSocket*);
    void tcpServerOpen(quint16);
    void tcpServerClose();
    void tcpClientConnect(QHostAddress, quint16);
    void tcpClientDisconnect();
    void udpServerOpen(quint16);
    void udpServerClose();
    void udpClientConnect(QHostAddress, quint16);
    ~Widget();

private slots:
    void on_connectBtn_clicked();
    void on_protocolCombox_currentIndexChanged(int index);
    void on_clearMsgBtn_clicked();
    void on_sendBtn_clicked();

    void on_pushButton_4_clicked();

protected:
    // void incomingConnection(qintptr socketDescriptor);

private:
    qint32 sendCount;
    qint32 receiveCount;

    bool ServerStatus;
    QLabel* clientCboxLabel;
	QComboBox* clientComboBox;
    QTimer* timer;
    QMap<QString, QTcpSocket*> clientMap;
    QList<QString> clientList;
//    QMap<QString, QString> clientMap;
    Ui::Widget *ui;
};

#endif // WIDGET_H
