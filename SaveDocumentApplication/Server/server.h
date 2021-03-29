#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDataStream>
#include <QTime>
#include <QFile>
#include <QMap>
#include <iostream>

class Server : public QObject
{
    Q_OBJECT

private:
    QTcpServer *m_ptcpServer; // Основной класс для у правлением сервера.
    quint16 m_nNextBlockSize; // Хранит размер файла.

private:
    void sendToClient(QTcpSocket *pSocket, const QString &name);
    void recvFile(const QString &name, const QByteArray &buffer);
    void saveFile(const QString &name, const QByteArray &buffer);
    QByteArray sendFileClient(const QString &name);

public:
    explicit Server(int nPort, QObject *parent = nullptr);

public slots:
    virtual void slotNewConnection();
    void slotReadClient();
};

#endif // SERVER_H
