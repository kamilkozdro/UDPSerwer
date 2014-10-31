#include <QCoreApplication>
#include "udpserwer.h"

// domyslne porty
#define LISTEN_PORT 3030//0xBD6 //
#define COM_PORT 3031//0xBD7 //
#define CLIENT_PORT 3032//0xBD8 //
#define LOCAL_ADDRESS "192.168.1.2"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    UDPSerwer serwer;
    serwer.run();
    return a.exec();
}
