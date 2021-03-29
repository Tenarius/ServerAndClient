#include "client.h"

Client::Client(QWidget *parent) : QWidget(parent)
{
    // Инициализация для связи с сервером.
    m_port = 3605;
    m_nextBlockSize = 0;
    m_isConnect = false;
    m_name = new QString;
    m_data = new QByteArray;
    m_socket = new QTcpSocket(this);
    m_socket->connectToHost("localhost", m_port);

    // Инициализация интерфейса.
    m_table = new QTableWidget(1, 2, this);
    m_item = new QTableWidgetItem;
    m_list = new QStringList;
    m_openButton = new QPushButton("Загрузить");
    m_downloadButton = new QPushButton("Скачать");
    m_layoutVertical = new QVBoxLayout(this);
    m_layoutHorizont = new QHBoxLayout();
    m_catalog = new Catalog();

    // Растановка элементов управления.
    *m_list  << "Название" << "Дата загрузки";
    m_table->setHorizontalHeaderLabels(*m_list);
    m_table->resize(500, 300);
    m_layoutVertical->addWidget(m_table);
    m_layoutHorizont->addWidget(m_openButton);
    m_layoutHorizont->addWidget(m_downloadButton);
    m_layoutVertical->addLayout(m_layoutHorizont);
    setLayout(m_layoutVertical);
    resize(450, 300);
    initTable();

    connect(m_socket, SIGNAL(connected()), this, SLOT(slotConnected())); // При установке соеденении слот возращается true.
    connect(m_socket, SIGNAL(readyRead()), this, SLOT(slotReadyRead())); // Если есть данные на чтение то слот создает файл и записывает данные.
    connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(slotError(QAbstractSocket::SocketError))); // Проверка на ошибки соеденении.

    connect(m_downloadButton, SIGNAL(clicked()), this, SLOT(slotOpenCatalog()));  // открывает окно каталога для выбора места загрузки файла.
    connect(m_openButton, SIGNAL(clicked()), m_catalog, SLOT(slotOpenFile())); // Открывает окно выбора файла для сохранения.
    connect(m_catalog, SIGNAL(readFile()), this, SLOT(slotSendServer())); // Сохраняем файл на сервере.
    connect(this, SIGNAL(sendServer()), this, SLOT(slotSendToServer())); // Сохраняем файл на сервере.
    connect(m_catalog, SIGNAL(readyRead()), this, SLOT(slotRecvServer())); // Загружаем файл с сервера.
}

// Читтает данные и записывает данные в файл.
void Client::slotReadyRead()
{
    QDataStream in(m_socket);
    in.setVersion(QDataStream::Qt_5_15);  
    in >> *m_name >> *m_data;
    m_nextBlockSize = 0;

    QFile file(m_catalog->getDir() + *m_name);

    if (file.open(QIODevice::WriteOnly))
    {
        QTextStream stream(&file);
        stream << *m_data;
    }

    file.close();
}

// Проверка на ошибки соеденение.
void Client::slotError(QAbstractSocket::SocketError error)
{
    QString strError = "Error: " + (error == QAbstractSocket::HostNotFoundError ?
                                        "The host was not found." : error == QAbstractSocket::RemoteHostClosedError ? "The remote host is closed." :
                                         error == QAbstractSocket::ConnectionRefusedError ? "The connection was refused." :
                                         QString(m_socket->errorString()));
    m_isConnect = false;
    qDebug() << "Error connect " << strError << "\n";
}

// Проверка на соедененеие.
void Client::slotConnected()
{
    qDebug() << "Connect.\n";
    m_isConnect = true;
}

// Отправляем на сервер имя файла, который хотим скачать.
void Client::slotSendToServer()
{
    qDebug() << "Set file: "<< *m_name << "\n";
    QByteArray arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_15);
    out << quint16(0) << *m_name << bool(1) << *m_data;
    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof (quint16));
    m_socket->write(arrBlock);
}

// Открыть директорию.
void Client::slotOpenCatalog()
{
    m_catalog->show();
}

// Выбираем файл для закрузки на сервер.
void Client::slotSendServer()
{
    *m_name = m_catalog->getNameFile();
    *m_data = m_catalog->getDataFile();
    emit sendServer();
    addDocument(*m_name);
    initTable();
}

//Загружаем файл на сервер.
void Client::slotRecvServer()
{
    *m_name = getNameFile();

    if (m_name->isNull() || m_name->isEmpty())
    {
        return;
    }

    qDebug() << "Get file: "<< *m_name << "\n";
    QByteArray arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_15);
    out << quint16(0) << *m_name << bool(0);
    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof (quint16));
    m_socket->write(arrBlock);
}

// Получаем имя фалйа по выбору строки из таблицы.
QString Client::getNameFile()
{
    int i= m_table->currentRow();

    if (m_table->selectedItems().isEmpty())
    {
        return "";
    }

    qDebug() << i;
    int j=0;
    QString nameFile;

    nameFile = m_table->item(i,j)->text();

    qDebug() << nameFile;
    return nameFile;
}

// Заполнение таблицы из файла, который содержит список добавленных данных на сервер.
void Client::initTable()
{
    QVector<DataTable> data = getTable();

    if (data.isEmpty())
        return;

    const int sizeRow {data.size() - 1};
    m_table->setRowCount(sizeRow);

    int row{};
    int col{};

    for (auto i{data.begin()}; i != data.end()-1; ++i)
    {
        m_item = new QTableWidgetItem(QString("%1").arg(i->name));
        m_table->setItem(row, col, m_item);

        ++col;
        m_item = new QTableWidgetItem(QString("%1").arg(i->date));
        m_table->setItem(row, col, m_item);

        ++row;
        --col;
    }
}

// Добавление в файл загруженных документов.
void Client::addDocument(const QString &name)
{
    QFile file("infoSave");

    if (file.open(QIODevice::Append))
    {
        QFileInfo fileInfo(file);
        QTextStream stream(&file);
        stream << name << " " << fileInfo.created().toString("hh:mm/dd.MM.yyyy") + '\n';
    }

    file.close();
}

// Получить абличное представление из файла загруженных данных на сервер.
QVector<DataTable> Client::getTable()
{
    QFile file("infoSave");
    QVector<DataTable> datTable;;
    DataTable nameFile;

    if (file.open(QIODevice::ReadOnly))
    {
        QTextStream stream(&file);

        while (!stream.atEnd())
        {
            stream >> nameFile.name >> nameFile.date;
            datTable.push_back(nameFile);
        }
    }

    file.close();
    return datTable;
}

Client::~Client()
{
    delete m_socket;
    delete m_name;
    delete m_data;
    delete m_table;
    delete m_item;
    delete m_catalog;
    qDebug() << "~Client()\n";
}
