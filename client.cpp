#include "client.h"

Client::Client(QObject *parent) : QObject(parent)
{
    tcpClient=new QTcpSocket(this);
     bufferSize=0;
    connect(tcpClient,SIGNAL(readyRead()),this,SLOT(slotReadBytes()));
    connectServer(QHostAddress::LocalHost,8888);
}

void Client::AcceptCommand(const uchar &type, const int &length, const QByteArray &array)
{

    if(type==1)
    {
     emit signalShowByteArray(array);
        emit signalRefreshFilesView(array);
    }else if(type==2)
    {
       emit signalRefreshFilesView(array);
       emit signalShowByteArray(array);
    }else if(type==3)
    {
        emit signalRefreshFilesView(array);
        emit signalShowByteArray(array);
    }

}

void Client::slotReadBytes()
{

    //    1命令
    //    2数据
    //    3文件
    //    1 byte+ 4 int 消息体大小+消息体
    int total=tcpClient->bytesAvailable();

    if(total+bufferSize>BUFFERSIZE)//total + bufferSize more than BUFFERSIZE
    {
        int copySize=BUFFERSIZE-bufferSize;
        QByteArray readData=tcpClient->read(copySize);
        arrayBuffer.append(readData);
        bufferSize+=readData.size();
    }else
    {
        QByteArray readData=tcpClient->read(total);
        arrayBuffer.append(readData);
        bufferSize+=readData.size();
    }
    while(bufferSize>=(1+sizeof(int)))
    {
        int len=0;
        uchar type=0;
        memcpy(&type,arrayBuffer.data(),sizeof(uchar));
        memcpy(&len,arrayBuffer.data()+1,sizeof(int));

        if(bufferSize>=(1+sizeof(int)+len))
        {
            QByteArray array;
            array.resize(len);
            memcpy(array.data(),arrayBuffer.data()+1+sizeof(int),len);
            arrayBuffer.remove(0,1+sizeof(int)+len);
            bufferSize-=(1+sizeof(int)+len);
            AcceptCommand(type,len,array);
        }else break;
    }
}

void Client::connectServer(const QHostAddress &addr,const qint16 &port)
{
    tcpClient->connectToHost(addr,port);
}



void Client::sendData(const QByteArray &data)
{

    tcpClient->write(data);
    tcpClient->waitForBytesWritten();
}
