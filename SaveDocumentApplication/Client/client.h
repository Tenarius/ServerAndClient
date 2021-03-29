#ifndef CLIENT_H
#define CLIENT_H

#include <QtWidgets>
#include <QTcpSocket>
#include <QDataStream>
#include <QDateTime>
#include <QFileInfo>
#include "DataTable.h"
#include "catalog.h"

class Client : public QWidget
{
    Q_OBJECT

    // Сокет и данные.
private:
    bool m_isConnect;
    quint16 m_nextBlockSize;
    int m_port;
    QTcpSocket *m_socket;
    QString *m_name;
    QByteArray *m_data;

    // Интерфейс.
private:
    QTableWidget  *m_table;
    QTableWidgetItem *m_item;
    QStringList *m_list;
    QBoxLayout *m_layoutVertical;
    QBoxLayout *m_layoutHorizont;
    QPushButton *m_openButton;
    QPushButton *m_downloadButton;
    Catalog *m_catalog;

private:
    void initTable();
    void addDocument(const QString &name);
    QString getNameFile();
    QVector<DataTable> getTable();

private slots:
    void slotError(QAbstractSocket::SocketError);
    void slotReadyRead();
    void slotConnected();
    void slotSendToServer();
    void slotOpenCatalog();
    void slotSendServer();
    void slotRecvServer();

public:
    explicit Client(QWidget *parent = nullptr);
    ~Client();

signals:
    void sendServer();

};

#endif // CLIENT_H
