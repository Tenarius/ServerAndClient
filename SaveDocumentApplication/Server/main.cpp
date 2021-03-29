#include <QCoreApplication>
#include "server.h"

#define PORT 3605

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Server server(PORT);

    return a.exec();
}
