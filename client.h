#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include<QtNetwork/QTcpSocket>
#include<QHostAddress>
#define BUFFERSIZE 64*1024
class Client : public QObject
{
    Q_OBJECT
public:
    explicit Client(QObject *parent = 0);

signals:
    void signalShowByteArray(const QByteArray &array);
    void signalRefreshFilesView(const QByteArray &json);
private:
   QTcpSocket *tcpClient;
   int bufferSize;
   char buffer[BUFFERSIZE];
   QByteArray arrayBuffer;
   void AcceptCommand(const uchar &type,const int &length,const QByteArray &array);

   void  connectServer(const QHostAddress &addr, const qint16 &port);
   const QByteArray ParseDataToSocketStream(const uchar &type,const int &length,const QByteArray &array);
public slots:
    void slotReadBytes();
   void sendData(const QByteArray &data);
};

#endif // CLIENT_H
