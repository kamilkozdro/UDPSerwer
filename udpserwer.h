#ifndef UDPSERWER_H
#define UDPSERWER_H

#include <QDebug>
#include <QUdpSocket>
#include <QHostAddress>
#include <QNetworkInterface>
#include <QObject>
#include <QFile>
#include <QDataStream>
#include <QBuffer>
#include <QProcess>

class UDPSerwer : public QObject
{
    Q_OBJECT
public:

    UDPSerwer();
    UDPSerwer(quint16 ListenPort, quint16 ComPort, quint16 ClientPort);
    ~UDPSerwer();

    QUdpSocket *listenSocket, *clientSocket;
    QHostAddress localAddress, senderAddress, clientAddress;
    quint16 listenPort, comPort, clientPort;
    bool activeClientFlag;

    //void clearMem(void *ptr);
    QHostAddress whatsMyIP();
    void run();
    void loadConfig();
    void waitForClient();
    void listenClient();
    void setClient(QHostAddress address);
    void removeClient();
/*    void writeData(QUdpSocket *socket, QByteArray *datagram);
    void writeData(QUdpSocket *socket, QByteArray datagram);
    void writeData(QUdpSocket *socket, char string[]);
*/    void writeToSender(char string[]);
    void writeToClient(char string[]);
    void writeToClient(QByteArray datagram);
    void sendImage();
    void broadcast(quint16 port, char string[]);
    int grabFrame();
    QByteArray readData();
    QByteArray readClient();

private:

public slots:

    void proceesSender();
    void proceesClient();

};

#endif UDPSERWER_H
