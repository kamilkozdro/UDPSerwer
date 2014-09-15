#include "udpserwer.h"
/*
UDPSerwer::UDPSerwer()
{

}
*/

UDPSerwer::UDPSerwer(uint ListenPort, uint ComPort, uint ClientPort)
    :listenPort(ListenPort),comPort(ComPort),clientPort(ClientPort)
{
    outListenSocket = new QUdpSocket(this);
    outClientSocket = new QUdpSocket(this);
    activeClientFlag = false;

    localAddress = whatsMyIP();

    waitForClient();
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
    // zwraca prawdopodobny 'lokalny' adres IP
QHostAddress UDPSerwer::whatsMyIP()
{
    QNetworkInterface network;

    for(int i=0; i<network.allAddresses().count(); i++)
    {
        if(network.allAddresses().at(i).protocol() == QAbstractSocket::IPv4Protocol)
        {
            if(network.allAddresses().at(i).toString().contains("192.168."))
                return network.allAddresses().at(i);
        }
    }
    return QHostAddress::Null;
}
    // nasluchuje potencjalnych klientow
void UDPSerwer::waitForClient()
{
    inListenSocket = new QUdpSocket(this);
    inListenSocket->bind(localAddress,listenPort);
    QObject::connect(inListenSocket,SIGNAL(readyRead()),this,SLOT(proceesSender()));
    qDebug() << "Nasluchuje klientow:" << localAddress.toString()<< ":"<< listenPort;
}
    // slucha zaakceptowanego klienta
void UDPSerwer::listenClient()
{
    inClientSocket->bind(comPort);
    QObject::connect(inClientSocket,SIGNAL(readyRead()),this,SLOT(proceesClient()));
    qDebug() << "Nasluchuje zaakceptowanego klienta:" << localAddress.toString()<< ":"<< comPort;
}

void UDPSerwer::setClient(QHostAddress address)
{
    inClientSocket = new QUdpSocket(this);
    clientAddress = address;
    activeClientFlag = true;
    qDebug() << "Klient zapisany";
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
    qDebug() << "Klient usuniety";
}

void UDPSerwer::writeData(QUdpSocket *socket, QByteArray datagram)
{
    socket->bind(clientAddress,clientPort);
    socket->writeDatagram(datagram.data(),datagram.size(),clientAddress,clientPort);
    qDebug() << "WYSLANO:" << datagram;
}

void UDPSerwer::writeData(QUdpSocket *socket, QByteArray *datagram)
{
    socket->bind(clientAddress,clientPort);
    socket->writeDatagram(datagram->data(),datagram->size(),clientAddress,clientPort);
    qDebug() << "WYSLANO:" << datagram;
}

void UDPSerwer::writeData(QUdpSocket *socket, char string[])
{
    QByteArray datagram = string;
    socket->bind(clientAddress,clientPort);
    socket->writeDatagram(datagram.data(),datagram.size(),clientAddress,clientPort);
    socket->waitForBytesWritten();
    qDebug() << "WYSLANO:" << datagram;
}

QByteArray UDPSerwer::readData(QUdpSocket *socket)
{
    QByteArray datagram;
    while (socket->hasPendingDatagrams())
    {
            datagram.resize(socket->pendingDatagramSize());
            socket->readDatagram(datagram.data(),datagram.size(),&senderAddress); //,&senderAddress -> zapisuje IP sendera
            qDebug() <<"ODEBRANO:"<< datagram;
    }
    return datagram;
}

//SLOTS

void UDPSerwer::proceesSender()
{
    QByteArray inListenData;
    inListenData = readData(inListenSocket);
    if(inListenData == "query")
    {
        if(activeClientFlag)    // inne aktywne polaczenie
        {
            writeData(outListenSocket,"N"); // 'N' - odrzucona prosba
            qDebug() << "Prosba odrzucona";
        }
        else
        {
            writeData(outListenSocket,"Y"); // 'Y' - przyjeta prosba
            qDebug() << "Prosba przyjeta";
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
    if(inClientData == "logout")
    {
        writeData(outClientSocket,"disconnected");
        removeClient();
    }
}
