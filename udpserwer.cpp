#include "udpserwer.h"

UDPSerwer::UDPSerwer()
{
    activeClientFlag = false;
    loadConfig();
    //waitForClient();
}

UDPSerwer::UDPSerwer(quint16 ListenPort, quint16 ComPort, quint16 ClientPort)
    :listenPort(ListenPort),comPort(ComPort),clientPort(ClientPort)
{
    activeClientFlag = false;
    localAddress = whatsMyIP();
    //waitForClient();
}

UDPSerwer::~UDPSerwer()
{
    if(clientSocket != NULL) // czy potrzebne?
    {
        delete clientSocket;
        clientSocket = NULL;
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
            if(network.allAddresses().at(i).toString().contains("192.168.1"))
                return network.allAddresses().at(i);
        }
    }
    return QHostAddress::Null;
}

void UDPSerwer::run()
{
    waitForClient();
}

void UDPSerwer::loadConfig()
{
    /* ZAPIS KONFIGURACJI
        #Port na ktorym nasluchuje serwer
        3030 0xBD6
        #Port na ktorym wystepuje komunikacja serwer - zaakceptowany klient
        3030 0xBD7
        #Port na ktorym nasluchuje klient
        3030 0xBD8
        #Adres serwera
        192.168.1.2
    */
    QFile confFile("A:/config.txt");

    if(!confFile.open(QIODevice::ReadOnly | QIODevice::Text))  qDebug() << "Fail to read config file";
    else
    {
        QByteArray buffer;

        buffer = confFile.readLine();
        buffer = confFile.readLine();
        buffer.chop(1);
        listenPort = buffer.toUInt();
        //qDebug() << hex << listenPort;
        buffer = confFile.readLine();
        buffer = confFile.readLine();
        buffer.chop(1);
        comPort = buffer.toUInt();
        //qDebug() << hex << comPort;
        buffer = confFile.readLine();
        buffer = confFile.readLine();
        buffer.chop(1);
        clientPort = buffer.toUInt();
        //qDebug() << hex << clientPort;
        buffer = confFile.readLine();
        buffer = confFile.readLine();
        buffer.chop(1);
        if( buffer == "0")  localAddress = whatsMyIP();
        else    localAddress.setAddress(QString(buffer));
        //qDebug() << buffer;
        confFile.close();
    }
}

// nasluchuje potencjalnych klientow
void UDPSerwer::waitForClient()
{
    listenSocket = new QUdpSocket(this);
    listenSocket->bind(localAddress, listenPort);
    connect(listenSocket,SIGNAL(readyRead()),this,SLOT(proceesSender()));
    qDebug() << "Nasluchuje klientow:" << localAddress<< ":"<< hex << listenPort;
    qDebug() << listenSocket->state();
}
// slucha zaakceptowanego klienta
void UDPSerwer::listenClient()
{
    clientSocket->bind(localAddress, comPort);
    QObject::connect(clientSocket,SIGNAL(readyRead()),this,SLOT(proceesClient()));
    qDebug() << "Nasluchuje zaakceptowanego klienta:" << clientAddress<< ":"<< hex << comPort;
    qDebug() << listenSocket->state() << clientSocket->state();
}

void UDPSerwer::setClient(QHostAddress address)
{
    clientSocket = new QUdpSocket(this);
    clientAddress = address;
    activeClientFlag = true;
    qDebug() << "Klient zapisany";
}

void UDPSerwer::removeClient()
{
    clientSocket->close();
    if(clientSocket != NULL)
    {
        delete clientSocket;
        clientSocket = NULL;
    }
    clientAddress = NULL;
    activeClientFlag = false;
    qDebug() << "Klient usuniety";
}
/*
void UDPSerwer::writeData(QUdpSocket *socket, QByteArray datagram)
{
    socket->writeDatagram(datagram.data(),datagram.size(),clientAddress,clientPort);
    socket->waitForBytesWritten();
    qDebug() << "WYSLANO:" << datagram;
}

void UDPSerwer::writeData(QUdpSocket *socket, QByteArray *datagram)
{
    socket->writeDatagram(datagram->data(),datagram->size(),clientAddress,clientPort);
    socket->waitForBytesWritten();
    qDebug() << "WYSLANO:" << datagram;
}

void UDPSerwer::writeData(QUdpSocket *socket, char string[])
{
    QByteArray datagram = string;
    socket->writeDatagram(datagram.data(),datagram.size(),clientAddress,clientPort);
    socket->waitForBytesWritten();
    qDebug() << "WYSLANO:" << datagram;
}
*/
void UDPSerwer::writeToSender(char string[])
{
    QByteArray datagram = string;
    listenSocket->writeDatagram(datagram.data(),datagram.size(),senderAddress,clientPort);
    listenSocket->waitForBytesWritten();
    //qDebug() << "WYSLANO:" << datagram;
}

void UDPSerwer::writeToClient(char string[])
{
    QByteArray datagram = string;
    clientSocket->writeDatagram(datagram.data(),datagram.size(),clientAddress,comPort);
    clientSocket->waitForBytesWritten();
    //qDebug() << "WYSLANO:" << datagram;
}

void UDPSerwer::writeToClient(QByteArray datagram)
{
    clientSocket->writeDatagram(datagram.data(),datagram.size(),clientAddress,comPort);
    clientSocket->waitForBytesWritten();
    //qDebug() << "WYSLANO:" << datagram;
}

void UDPSerwer::sendImage()
{
    QFile img("test.pgm");
    if (!img.open(QIODevice::ReadOnly | QIODevice::Text))
        qDebug() << "nie otworzono img";
    else
        writeToClient(QByteArray(img.readAll()));
}

void UDPSerwer::broadcast(quint16 port, char string[])
{
    QByteArray datagram = string;
    QUdpSocket tempSocket;
    tempSocket.writeDatagram(datagram.data(),datagram.size(),QHostAddress::Broadcast,port);
    tempSocket.waitForBytesWritten();
    qDebug() << "WYSLANO:" << datagram;
}
// 1 - udane, 0 - blad
int UDPSerwer::grabFrame()
{
    QProcess process;
    if( process.execute("grabFrame.exe") != 0 )
    {
        qDebug() << "cant execute grabFrame.exe";
        return 0;
    }
    else
    {
        return 1;
    }
}

QByteArray UDPSerwer::readData()
{
    QByteArray datagram;
    while (listenSocket->hasPendingDatagrams())
    {
        datagram.resize(listenSocket->pendingDatagramSize());
        listenSocket->readDatagram(datagram.data(),datagram.size(),&senderAddress);
        qDebug() <<"ODEBRANO:"<< datagram << "od:" << senderAddress;
    }
    return datagram;
}

QByteArray UDPSerwer::readClient()
{
    QByteArray datagram;
    while (clientSocket->hasPendingDatagrams())
    {
        datagram.resize(datagram.size()+clientSocket->pendingDatagramSize());
        clientSocket->readDatagram(datagram.data(),datagram.size());
        qDebug() <<"ODEBRANO:"<< datagram;
    }
    return datagram;
}

//SLOTS

void UDPSerwer::proceesSender()
{
    QByteArray inListenData;
    inListenData = readData();
    if(inListenData == "query")
    {
        if(activeClientFlag)    // inne aktywne polaczenie
        {
            writeToSender("N"); // 'N' - odrzucona prosba
            qDebug() << "Prosba odrzucona";
        }
        else
        {
            writeToSender("Y"); // 'Y' - przyjeta prosba
            qDebug() << "Prosba przyjeta";
            setClient(senderAddress);
            listenClient();
        }
    }
}

void UDPSerwer::proceesClient()
{
    QByteArray inClientData;
    inClientData = readClient();
    // USTAL ARGUMENTY DO ODBIORU;  CASE?
    // odlaczenie klienta
    if(inClientData == "logout")
    {
        writeToClient("disconnected");
        removeClient();
    }
    // pobierz obraz z kamery i wyslij do klienta
    else if(inClientData == "grabFrame")
    {
        if(grabFrame() != 1)    writeToClient("grabFrame failed");
        else
        {
            QFile img("test.pgm");
            if (!img.open(QIODevice::ReadOnly | QIODevice::Text))
                qDebug() << "nie otworzono img";
            else
                sendImage();
        }
    }
    // zmiana pozycji platformy
    else if(inClientData[0] == 'X');
        //przeslij pozycje x do silnika
    else if(inClientData[0] == 'Y');
        //przeslij pozycje y do silnika
    else if(inClientData[0] == 'Z');
        //przeslij pozycje z do silnika
}
