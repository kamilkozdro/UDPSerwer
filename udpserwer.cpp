#include "udpserwer.h"

#define LISTEN_PORT 3030
#define COM_PORT 3031
#define CLIENT_PORT 3032
#define LOCAL_ADDRESS "192.168.1.2"

UDPSerwer::UDPSerwer()
{

}

UDPSerwer::UDPSerwer(QString arg)
{
    outListenSocket = new QUdpSocket(this);
    outClientSocket = new QUdpSocket(this);
    activeClientFlag = false;

    if(arg == "default")
    {
        localAddress.setAddress(LOCAL_ADDRESS); // ZROB: wykryj adres w lokalnej sieci
        listenPort = LISTEN_PORT;
        comPort = COM_PORT;
        clientPort = CLIENT_PORT;
        qDebug() << "sluchaj na:" << localAddress.toString()<< ":"<< listenPort << endl;
    }
}

UDPSerwer::~UDPSerwer()
{
    if(outListenSocket != NULL) // czy potrzebne?
    {
        delete outListenSocket;
        outListenSocket = NULL;
    }
    if(outClientSocket != NULL) // czy potrzebne?
    {
        delete outClientSocket;
        outClientSocket = NULL;
    }
}

void UDPSerwer::waitForClient()
{
    inListenSocket = new QUdpSocket(this);
    inListenSocket->bind(listenPort);
    QObject::connect(inListenSocket,SIGNAL(readyRead()),this,SLOT(proceesSender()));
}

void UDPSerwer::listenClient()
{
    inClientSocket->bind(comPort);
    QObject::connect(inClientSocket,SIGNAL(readyRead()),this,SLOT(proceesClient()));
}

void UDPSerwer::setClient(QHostAddress address)
{
    inClientSocket = new QUdpSocket(this);
    clientAddress = address;
    activeClientFlag = true;
}

void UDPSerwer::removeClient()
{
    inClientSocket->close();
    if(inClientSocket != NULL)
    {
        delete inClientSocket;
        inClientSocket = NULL;
    }
    if(outClientSocket != NULL)
    {
        delete outClientSocket;
        outClientSocket = NULL;
    }
    clientAddress = NULL;
    clientPort = NULL;
    activeClientFlag = false;
}

void UDPSerwer::writeData(QUdpSocket *socket, QByteArray datagram)
{
    socket->bind(clientAddress,clientPort);
    socket->writeDatagram(datagram.data(),datagram.size(),clientAddress,clientPort);
}

void UDPSerwer::writeData(QUdpSocket *socket, QByteArray *datagram)
{
    socket->bind(clientAddress,clientPort);
    socket->writeDatagram(datagram->data(),datagram->size(),clientAddress,clientPort);
}

void UDPSerwer::writeData(QUdpSocket *socket, char string[])
{
    QByteArray datagram;
    datagram.append(string);
    socket->bind(clientAddress,clientPort);
    socket->writeDatagram(datagram.data(),datagram.size(),clientAddress,clientPort);
}

QByteArray UDPSerwer::readData(QUdpSocket *socket)
{
    QByteArray datagram;
    while (socket->hasPendingDatagrams())
    {
            datagram.resize(socket->pendingDatagramSize());
            socket->readDatagram(datagram.data(),datagram.size(),&senderAddress); //,&senderAddress -> zapisuje IP sendera
    }
    return datagram;
}

//SLOTS

void UDPSerwer::proceesSender()
{
    QByteArray inListenData;
    inListenData = readData(inListenSocket);
    if(inListenData.data() == "Q")
    {
        if(activeClientFlag)    // inne aktywne polaczenie
        {
            writeData(outListenSocket,"N"); // 'N' - odrzucona prosba
        }
        else
        {
            writeData(outListenSocket,"Y"); // 'Y' - przyjeta prosba
            setClient(senderAddress);
            listenClient();
        }
    }
}

void UDPSerwer::proceesClient()
{
    QByteArray inClientData;
    inClientData = readData(inClientSocket);
    // USTAL ARGUMENTY DO ODBIORU; CASE?
    if(inClientData.data() == "logout")
    {
        writeData(outClientSocket,"Disconnected");
        removeClient();
    }
}
