#include "server.h"

// Отправляем клиенту файл
void Server::sendToClient(QTcpSocket *pSocket, const QString &name)
{
    QByteArray arrBlock; // Отправляем данные в виде бинарных данных.
    QDataStream out(&arrBlock, QIODevice::WriteOnly);// Создаем поток для записи.
    out.setVersion(QDataStream::Qt_5_15); // Определяем версию потока.
    out << quint16(0) << name << sendFileClient(name); // Записываем в поток данные.
    out.device()->seek(0); // Возвращаем указатель в начало.
    out << quint16(arrBlock.size() - sizeof (quint16)); // вычисляем размер отправляемых данных.
    pSocket->write(arrBlock); // Отправляем данные.
}

// Сохраняем файл клиента.
void Server::saveFile(const QString &name, const QByteArray &buffer)
{
    QFile file(name);
    QTextStream out(stdout);

    if (file.open(QIODevice::WriteOnly))
    {
        QDataStream stream(&file);
        stream.setVersion(QDataStream::Qt_5_15);
        stream << buffer;

        if (stream.status() != QDataStream::Ok)
        {
           qDebug() << "Error read file" << name << ".\n";
        }
        QTextStream out(stdout);
        out << buffer << "Save!\n";
    }
     file.close();
     out << buffer << "Save and close!\n";
}

// Запись файла на отправку.
QByteArray Server::sendFileClient(const QString &name)
{
    QFile file(name);
    QByteArray buffer;

    if (file.open(QIODevice::ReadOnly))
    {
        QDataStream stream(&file);
        stream.setVersion(QDataStream::Qt_5_15);
        stream >> buffer;

        if (stream.status() != QDataStream::Ok)
        {
            qDebug() << "Error read file" << name << ".\n";
        }

        QTextStream out(stdout);
        out << buffer;
    }

    file.close();
    return buffer;
}

Server::Server(int nPort, QObject *parent) : QObject{parent}, m_nNextBlockSize{0}
{
    m_ptcpServer = new QTcpServer(this);

    // Слушаем заданный  порт.
    if (!m_ptcpServer->listen(QHostAddress::Any, nPort))
    {
        std::cerr << "Server Error" << "Unable to start the server:"
                  << m_ptcpServer->errorString().toStdString() << std::endl;

        m_ptcpServer->close();
        return;
    }
    // Обработка сигнала новых подключенных клиентов слотом, которые мы определили.
    connect(m_ptcpServer, SIGNAL(newConnection()), this, SLOT(slotNewConnection()) );
}

void Server::slotNewConnection()
{
    // Подтверждаем соеденение и возращаем сокет по средством которго будет просиходит дальнейшее общение.
    QTcpSocket *pClientSocket = m_ptcpServer->nextPendingConnection();
    // Обрабатываем сигнал отключение клиента от сервера слотом удаления подклчения.
    connect(pClientSocket, SIGNAL(disconnected()), pClientSocket, SLOT(deleteLater()) );
    // Обрабатываем сигнал отправки сообщения от клиента серверу слотом, которое мы определили.
    connect(pClientSocket, SIGNAL(readyRead()), this, SLOT(slotReadClient()) );
    QTextStream out(stdout);
    out << "Connect\n";
}

void Server::slotReadClient()
{
    QTcpSocket *pClientSocket = (QTcpSocket*)sender(); // Получаем ссылку на сокет связанный с клиентом.
    QDataStream in(pClientSocket); // Создаем поток для чтения из связанного с клиентом сокета.
    in.setVersion(QDataStream::Qt_5_15); // Определяем версию потока.

    // Читаем данные блоками.
    while(true)
    {
        if (!m_nNextBlockSize) // Размер блок равна 0 значит размер неизвестен.
        {
            if (pClientSocket->bytesAvailable() < sizeof (quint16)) // Если размер получаемых данных не меньше 2 байтов то записываем размер блока.
            {
                break;
            }

            in >> m_nNextBlockSize; // Пишем размер блока.
        }

        if (pClientSocket->bytesAvailable() < m_nNextBlockSize) // Еслм размер отправляеммых данных больше или равно блоку то пишем записываем данные.
        {
            break;
        }

        QString name;
        bool flag;
        QByteArray buffer;
        in >> name >> flag;

        if (flag)
        {
            QTextStream out(stdout);
            out << "Save\n";
            in >> buffer;
            saveFile(name, buffer);
        }
        else
        {
            QTextStream out(stdout);
            out << "Read\n";
            sendToClient(pClientSocket, name);
        }

        m_nNextBlockSize = 0;
    }
}
