#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>
#include <QTcpSocket>


class TcpClient :  public QTcpSocket
{
public:
    TcpClient();

private:
	QTcpSocket* clientSocket;
    quint16 port;
    QHostAddress* hostAddr;

signals:

public slots:
};

#endif // TCPCLIENT_H
