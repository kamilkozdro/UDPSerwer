#ifndef UDPSERWER_H
#define UDPSERWER_H

#include <QDebug>
#include <QUdpSocket>
#include <QHostAddress>
#include <QNetworkInterface>
#include <QObject>
// domyslne porty
#define LISTEN_PORT 3030
#define COM_PORT 3031
#define CLIENT_PORT 3032
#define LOCAL_ADDRESS "192.168.1.2"

class UDPSerwer : public QObject
{
    Q_OBJECT
public:

    //UDPSerwer();
    UDPSerwer(uint ListenPort=LISTEN_PORT, uint ComPort=COM_PORT, uint ClientPort=CLIENT_PORT);
    ~UDPSerwer();

    //void clearMem(void *ptr);
    QHostAddress whatsMyIP();
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
