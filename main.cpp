#include <QCoreApplication>
#include "udpserwer.h"

// domyslne porty
#define LISTEN_PORT 0xBD6 //3030
#define COM_PORT 0xBD7 //3031
#define CLIENT_PORT 0xBD8 //3032
#define LOCAL_ADDRESS "192.168.1.2"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    UDPSerwer serwer;
    serwer.run();
/*
    QHostAddress address("192.168.1.2");
    quint16 port = 3032;
    serwer.writeData(address,port,"Y");
*/
    return a.exec();
}
