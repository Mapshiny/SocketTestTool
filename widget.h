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
    void initForm();
    void onAccept();
    void clientDisconnect();
    void readMsg();
    void displayError();
    ~Widget();
    void test();

private slots:
    void on_connectBtn_clicked();
    void on_protocolCombox_currentIndexChanged(int index);
    void on_clearMsgBtn_clicked();

protected:
    // void incomingConnection(qintptr socketDescriptor);

private:
    bool widgetStatus;
    QLabel* clientCboxLabel;
	QComboBox* clientComboBox;
    QTimer* timer;
    QList<QString> clientList;
//    QMap<QString, QString> clientMap;
    Ui::Widget *ui;
};

#endif // WIDGET_H
