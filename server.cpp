#include "server.h"

Server::Server(QObject *parent) : QObject(parent)
{
    driver=new Driver();
    fileWatcher=new QFileSystemWatcher(QStringList(driver->GetRoot()));
    bufferSize=0;
    memset(&buffer,0,sizeof(char)*32);
    tcpServer=new QTcpServer(this);
    tcpServer->close();
    tcpServer->listen(QHostAddress::LocalHost,8888);
    connect(tcpServer,SIGNAL(newConnection()),this,SLOT(slotnewConnectPedding()));
    connect(fileWatcher,SIGNAL(fileChanged(QString)),this,SLOT(slotFilesChanged(QString)));
    connect(fileWatcher,SIGNAL(directoryChanged(QString)),this,SLOT(slotDirChanged(QString)));

}

Server::~Server()
{
    if(driver!=NULL)
    {
        delete driver;
        driver=NULL;
    }
    if(fileWatcher!=NULL)
    {
        delete fileWatcher;
        fileWatcher=NULL;
    }
}

const QString Server::GetCurrentPathFilesJson(const QString &path)
{
    QList<FileModel *>listFile;
    listFile=getModels(path);
    QString modelJson=QString("{\"root\":\"%1\",\"files\":[").arg(path);
    for (int var = 0; var < listFile.size(); ++var) {
        modelJson.append(listFile.at(var)->getJsonArray()).append(",");
    }
    modelJson= modelJson.remove(modelJson.length()-1,1);
    modelJson.append("]}");
    return modelJson;
}
QList<FileModel *> Server::getModels(const QString &path)
{
    QList<FileModel *>list;
    QDir dir;
    dir.setCurrent(path);
    foreach (QFileInfo info, dir.entryInfoList(QDir::AllDirs|QDir::Files|QDir::NoDotAndDotDot)) {
        FileModel *model=new FileModel;
        model->setName(info.fileName());
        model->setIsDir(info.isDir());
        model->setPath(info.path());
        model->setSize(info.size());
        list.append(model);
    }
    return list;
}

void Server::slotFilesChanged(const QString &file)
{
    qDebug()<<"slotFilesChanged "<<file;
}
void Server::slotDirChanged(const QString &dir)
{
    QString json="";
    json=GetCurrentPathFilesJson(driver->GetRoot());

    sendData(1,json.toUtf8().length(),json.toUtf8());
}

void Server::sendData(const uchar &type, const int &length, const QByteArray &array)
{
    tcpNewConnect->write(ParseDataToSocketStream(type,length,array));
    tcpNewConnect->waitForBytesWritten();
}

const QByteArray Server::ParseDataToSocketStream(const uchar &type, const int &length, const QByteArray &array)
{

    QByteArray stream;
    stream.resize(1+sizeof(int));
    int dataSize=array.size();
    memcpy(stream.data(),&type,sizeof(uchar));
    memcpy(stream.data()+1,&dataSize,sizeof(int));
    stream.append(array);
    return stream;
}

void Server::AcceptCommand(const uchar &type, const int &length, const QByteArray &array)
{
    if(type==1)//返回根目录
    {
        QString json="";
        json=GetCurrentPathFilesJson(driver->GetRoot());
        sendData(1,json.toUtf8().length(),json.toUtf8());
    }else if(type==2)
    {
        QString upPath=QString();
        QString json="";
        const QString rootPath= driver->GetRoot();
        //qDebug()<<"rootPath "<<rootPath;
        if(driver->cdUp(upPath))
        {
            fileWatcher->removePath(rootPath);
            fileWatcher->addPath(upPath);
            //qDebug()<<"add watch "<<upPath<<" get root "<<driver->GetRoot();
            json= GetCurrentPathFilesJson(driver->GetRoot());
            sendData(2,json.toUtf8().length(),json.toUtf8());
        }
    }else if(type==3)
    {
        QString path=QString("%1/%2").arg(driver->GetRoot(),QString(array));
        const QString rootPath=driver->GetRoot();
        //qDebug()<<"rootPath "<<rootPath;
       if(driver->cd(path))
       {
           fileWatcher->removePath(rootPath);
           fileWatcher->addPath(path);
           //qDebug()<<"add watch "<<path;
           QString json=GetCurrentPathFilesJson(driver->GetRoot());
           //qDebug()<<"get root "<<driver->GetRoot();
           sendData(3,json.toUtf8().length(),json.toUtf8());
       }



    }
}
void Server::slotReadBytes()
{
    //    1命令
    //    2数据
    //    3文件
    //    1 byte+ 4 int 消息体大小+消息体
    int total=tcpNewConnect->bytesAvailable();
    if(total+bufferSize>BUFFERSIZE)//total + bufferSize more than BUFFERSIZE
    {
        int copySize=BUFFERSIZE-bufferSize;
        QByteArray readData=tcpNewConnect->read(copySize);
        arrayBuffer.append(readData);
        bufferSize+=readData.size();
    }else
    {
        QByteArray readData=tcpNewConnect->read(total);
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
            //emit signalShowByteArray(array);
        }else break;
    }

}

void Server::slotnewConnectPedding()
{
    /*if(tcpServer->hasPendingConnections())
    {
        return;
    }*/
    //qDebug()<<tcpNewConnect->isOpen()<<tcpNewConnect->isValid();
    tcpNewConnect=tcpServer->nextPendingConnection();
    connect(tcpNewConnect,SIGNAL(readyRead()),this,SLOT(slotReadBytes()));
}

