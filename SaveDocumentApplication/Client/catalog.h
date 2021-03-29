#ifndef CATALOG_H
#define CATALOG_H

#include <QtWidgets>

class Catalog : public QWidget
{
    Q_OBJECT

private:
    QLineEdit *m_leDir;
    QLabel *m_lbDir;
    QPushButton *m_openDir;
    QPushButton *m_saveFile;
    QGridLayout *m_layout;
    QString *m_nameFile;
    QByteArray *m_dataFile;
    QString *m_dir;

private:
    QString getNameFile(QString dir);

private slots:
    void slotBrowse();
    void slotSaveDir();

public slots:
    void slotOpenFile();

public:
    explicit Catalog(QWidget *parent = nullptr);
    QString getNameFile();
    QByteArray getDataFile();
    QString getDir();
    ~Catalog();

signals:
    void readFile();
    void readyRead();
};

#endif // CATALOG_H
