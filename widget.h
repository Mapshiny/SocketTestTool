#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTcpServer>
#include <QTcpSocket>
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
    void initForm();
    void onAccept();
    void clientDisconnect();
    void readMsg();
    void sendMsg();
    void displayError();
    ~Widget();
    void test();

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
    bool widgetStatus;
    bool ProStatus;
    QLabel* clientCboxLabel;
	QComboBox* clientComboBox;
    QTimer* timer;
    QMap<QString, QTcpSocket*> clientMap;
    QList<QString> clientList;
//    QMap<QString, QString> clientMap;
    Ui::Widget *ui;
};

#endif // WIDGET_H
