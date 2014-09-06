#ifndef UDPSERWER_H
#define UDPSERWER_H

#include <QDebug>
#include <QUdpSocket>
#include <QHostAddress>
#include <QObject>

class UDPSerwer : public QObject
{
    Q_OBJECT
public:

    UDPSerwer();
    UDPSerwer(QString arg);
    ~UDPSerwer();

    //void clearMem(void *ptr);
    void waitForClient();
    void listenClient();
    void setClient(QHostAddress address);
    void removeClient();
    void writeData(QUdpSocket *socket, QByteArray *datagram);
    void writeData(QUdpSocket *socket, QByteArray datagram);
    void writeData(QUdpSocket *socket, char string[]);
    QByteArray readData(QUdpSocket *socket);

    QUdpSocket *inListenSocket, *outListenSocket, *inClientSocket, *outClientSocket;
    QHostAddress localAddress, senderAddress, clientAddress;
    quint16 listenPort, comPort, clientPort;
    QByteArray outClientData;
    bool activeClientFlag;

private:

public slots:

    void proceesSender();
    void proceesClient();

};

#endif UDPSERWER_H
