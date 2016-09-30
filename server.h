#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include<QtNetwork/QTcpServer>
#include<QtNetwork/QTcpSocket>
#include<QFileSystemWatcher>
#include<filemodel.h>
#include<driver.h>
#define BUFFERSIZE 64*1024
class Server : public QObject
{
    Q_OBJECT
public:
    explicit Server(QObject *parent = 0);
     ~Server();

signals:
    void signalShowByteArray(const QByteArray &array);
private:
    QFileSystemWatcher *fileWatcher;
    QTcpServer *tcpServer;
    QTcpSocket *tcpNewConnect;
    int bufferSize;
    char buffer[BUFFERSIZE];
    QByteArray arrayBuffer;
    Driver *driver;

    const QString GetCurrentPathFilesJson(const QString &path);
    void sendData(const uchar &type,const int &length,const QByteArray &array);
    const QByteArray ParseDataToSocketStream(const uchar &type,const int &length,const QByteArray &array);
    void AcceptCommand(const uchar &type,const int &length,const QByteArray &array);
    QList<FileModel *> getModels(const QString &path);
public slots:
    void slotFilesChanged(const QString &file);
    void slotDirChanged(const QString &dir);
    void slotReadBytes();
    void  slotnewConnectPedding();
};

#endif // SERVER_H
