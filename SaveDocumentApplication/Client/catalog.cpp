#include "catalog.h"

Catalog::Catalog(QWidget *parent) : QWidget(parent)
{
    // Инициализация элементов интерфейса.
    m_leDir = new QLineEdit(QCoreApplication::applicationDirPath());
    m_lbDir = new QLabel("&Директория");
    m_openDir = new QPushButton("&Открыть");
    m_saveFile = new QPushButton("&Принять");
    m_layout = new QGridLayout;
    m_nameFile = new QString;
    m_dir = new QString;

    // Размещение элементов интерфейса.
    m_lbDir->setBuddy(m_openDir);
    m_layout->setContentsMargins(5, 5, 5, 5);
    m_layout->setSpacing(15);
    m_layout->addWidget(m_lbDir, 0, 0);
    m_layout->addWidget(m_leDir, 0, 1);
    m_layout->addWidget(m_openDir, 0, 2);
    m_layout->addWidget(m_saveFile, 1, 2);
    setLayout(m_layout);
    resize(600, 100);

    connect(m_openDir, SIGNAL(clicked()), this, SLOT(slotBrowse())); // Открывает выбо директории для выгрузки файла.
    connect(m_saveFile, SIGNAL(clicked()), this, SLOT(slotSaveDir())); // Принимает выбранную директорию и закрывает окно.
}

// Возращаем имя файла.
QString Catalog::getNameFile()
{
    return *m_nameFile;
}

// Взращаем данные файла.
QByteArray Catalog::getDataFile()
{
    return *m_dataFile;
}

// Возращаем путь файла.
QString Catalog::getDir()
{
    return *m_dir;
}

// Открываем директорию и получам путь к файлу.
void Catalog::slotBrowse()
{
    *m_dir = QFileDialog::getExistingDirectory(0, "Выберите директорию",  m_leDir->text()) + "/";

    if (!m_dir->isEmpty())
    {
        m_leDir->setText(*m_dir);
    }


}


void Catalog::slotSaveDir()
{
    emit readyRead();
    close();
}


// ПОлучаем имя и данные файла для загрузки на сервер.
void Catalog::slotOpenFile()
{
    m_dataFile = new QByteArray;

    auto fileContentReady = [&](const QString &fileName, const QByteArray &fileContent) {
        if (fileName.isEmpty() || fileContent.isEmpty())
        {
            return;
        }
        else
        {
            *m_nameFile = getNameFile(fileName);
            *m_dataFile = fileContent;
            emit readFile();
        }
    };

    QFileDialog::getOpenFileContent("Text files (*.txt)",  fileContentReady);
}
// Получить имя файла из пути расположения.
QString Catalog::getNameFile(QString dir)
{
    QString name;
    int ind = dir.size()+1;

    for (auto i{dir.end()}; dir.begin() != i; --i )
    {
        if (*i == QChar('/'))
        {
            break;
        }
        --ind;
    }

    name = dir.remove(0, ind);
    return name;
}

Catalog::~Catalog()
{
    delete m_leDir;
    delete m_lbDir;
    delete m_openDir;
    delete m_saveFile;
    delete m_layout;
    delete m_nameFile;
    delete m_dataFile;
    delete m_dir;
}
