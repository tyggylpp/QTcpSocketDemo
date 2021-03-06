#include "widget.h"
#include "ui_widget.h"
#include<qdir>
#include<QJsonDocument>

#include<QJsonObject>
#include<QJsonArray>
Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget),isconnect(false)
{
    ui->setupUi(this);
    server=new Server();
    client=new Client();
    threadClient=new QThread();
    threadServer=new QThread();
    server->moveToThread(threadServer);
    client->moveToThread(threadClient);
    threadServer->start();
    threadClient->start();
    table_Files=new QTableView(this);
    table_Files->setGeometry(10,80,651,361);
    tableFilesModel=new TableFilesModel();
    this->table_Files->verticalHeader()->setVisible(false);
    this->table_Files->setModel(tableFilesModel);
    this->table_Files->setShowGrid(false);
    this->table_Files->setSelectionBehavior(QAbstractItemView::SelectRows);
    delegate=new Delegate(table_Files);
    table_Files->setItemDelegate(delegate);
    connect(table_Files,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(slotdoubleClicked(QModelIndex)));
    //connect(this,SIGNAL(destroyed(QObject*)),server,SLOT(deleteLater()));
    //connect(tcpServer,SIGNAL(newConnection()),this,SLOT(newConnectPedding()));
    connect(this,SIGNAL(destroyed(QObject*)),threadClient,SLOT(deleteLater()));
    connect(threadClient,SIGNAL(destroyed(QObject*)) ,threadServer,SLOT(deleteLater()));
    connect(this,SIGNAL(signalSendData(QByteArray)) ,client,SLOT(sendData(QByteArray)));
    connect(server,SIGNAL(signalShowByteArray(QByteArray)),this,SLOT(slotShowByteArray(QByteArray)));
    connect(client,SIGNAL(signalShowByteArray(QByteArray)),this,SLOT(slotShowByteArray(QByteArray)));
    connect(client,SIGNAL(signalRefreshFilesView(QByteArray)),this,SLOT(slotRefreshFilesView(QByteArray)));
}

Widget::~Widget()
{
    if(threadClient->isRunning())threadClient->deleteLater();
    if(threadServer->isRunning())threadServer->deleteLater();
    delete ui;
}

QList<FileModel *> Widget::getModels(const QString &path)
{
    QList<FileModel *>list;
    QDir dir;
    dir.setCurrent(path);
    foreach (QFileInfo info, dir.entryInfoList()) {
        FileModel *model=new FileModel;
        model->setName(info.fileName());
        model->setIsDir(info.isDir());
        model->setPath(info.path());
        model->setSize(info.size());
        list.append(model);
    }

    return list;
}

QList<FileModel> Widget::parseJsonToModel(const QString &json)
{

    this->listFile.clear();
    QJsonParseError error;
    QJsonDocument document=QJsonDocument::fromJson(json.toUtf8(),&error);
    if(error.error==QJsonParseError::NoError)
    {
        if(document.isObject())
        {
            QJsonObject object=document.object();
            if(object.contains("root"))
            {
                QJsonValue value= object.take("root");
                //qDebug()<<"root "<<value;
            }
            if(object.contains("files"))
            {
                QJsonValue value=object.take("files");

                if(value.isArray())
                {
                    QJsonArray array=value.toArray();
                    for (int var = 0; var < array.size(); ++var) {
                        FileModel model;
                        QJsonObject obj=array.at(var).toObject();
                        if(obj.contains("isDir"))
                        {
                            model.setIsDir(obj.take("isDir").toVariant().toBool());
                            //model.setIsDir(obj.take("isDir").toString()=="true");
                        }

                        if(obj.contains("name"))
                        {
                            //qDebug()<<obj.take("name").toString();
                            model.setName(obj.take("name").toString());
                        }
                        if(obj.contains("path")){
                            //qDebug()<<obj.take("path").toString();
                            model.setPath(obj.take("path").toString());
                        }
                        if(obj.contains("size"))
                        {

                            model.setSize((obj.take("size").toVariant().toInt()));
                        }
                        this->listFile.append(model);
                    }
                }

            }
        }
    }else qDebug()<<"error json";

    return this->listFile;
}


void Widget::on_pushButton_clicked()
{
    ui->label->clear();
#if 0
    if(tcpServer->hasPendingConnections())
    {
        return;
    }



    tcpClient->connectToHost(QHostAddress::LocalHost,tcpServer->serverPort());

    ui->label->setText(" connect to host ");
#endif
}

void Widget::on_btn_sersend_clicked()
{
    for (int var = 0; var < 1; ++var) {
        QByteArray array= ParseDataToSocketStream(1,0,QByteArray());
        emit signalSendData(array);
    }

}

void Widget::slotShowByteArray(const QByteArray &array)
{
    this->ui->txt_dir->append(QString(array));
}


void Widget::on_btn_up_clicked()
{
    QByteArray array=ParseDataToSocketStream(2,QByteArray().length(),QByteArray());
    emit signalSendData(array);

}


const QByteArray Widget::ParseDataToSocketStream(const uchar &type, const int &length, const QByteArray &array)
{
    QByteArray stream;
    stream.resize(1+sizeof(int));
    int dataSize=length;
    memcpy(stream.data(),&type,sizeof(uchar));
    memcpy(stream.data()+1,&dataSize,sizeof(int));
    stream.append(array);
    return stream;
}

void Widget::on_btn_refresh_clicked()
{

}

void Widget::slotRefreshFilesView(const QByteArray &json)
{
    QList<FileModel>list =parseJsonToModel(json);
    tableFilesModel->reFreshModel(list);

}

void Widget::slotdoubleClicked(const QModelIndex &index)
{

    if(!index.isValid())return;
    if(index.column()==1)
    {
        if(listFile.at(index.row()).getIsDir())
        {
            QByteArray array=listFile.at(index.row()).getName().toUtf8();
            array =ParseDataToSocketStream(3,array.length(),array);
            emit signalSendData(array);
        }
    }
}

TableFilesModel::TableFilesModel(const QList<FileModel> listModel)
{
    horizonHead.clear();
    horizonHead.append("");
    horizonHead.append("名称");
    horizonHead.append("类型");
    horizonHead.append("大小");
    horizonHead.append("状态");
    reFreshModel(listModel);
}

void TableFilesModel::reFreshModel(const QList<FileModel> list)
{
    this->beginResetModel();
    this->listData=list;
    for (int var = 0; var < list.length(); ++var) {
        listCheck.append(false);
    }
    this->endResetModel();
}

bool TableFilesModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
  if(!index.isValid())return false;
  if(index.column()==0)
  {
      if(role==Qt::CheckStateRole||role==Qt::UserRole)
      {
       listCheck.replace(index.row(),value.toBool());
       emit dataChanged(index,index);
       return true;
      }
  }
  return false;
}

Qt::ItemFlags TableFilesModel::flags(const QModelIndex &index) const
{
  if(!index.isValid())return QAbstractItemModel::flags(index);
  if(index.column()==0)
  {
      return Qt::ItemIsUserCheckable|Qt::ItemIsEnabled;
  }
  Qt::ItemFlags flags=Qt::ItemIsUserCheckable|Qt::ItemIsSelectable|Qt::ItemIsEnabled;
  return flags;
}

QVariant TableFilesModel::headerData(int section, Qt::Orientation orientation, int role) const
{

    if(orientation==Qt::Horizontal&&role==Qt::DisplayRole)
    {

        return horizonHead.at(section);
    }
    return QVariant();

}

int TableFilesModel::rowCount(const QModelIndex &parent) const
{
    return listData.size();
}

int TableFilesModel::columnCount(const QModelIndex &parent) const
{
    return horizonHead.size();
}

QVariant TableFilesModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())return QVariant();
    if(index.column()==0)
    {
          if(role==Qt::UserRole)
          {
              return listCheck.at(index.row());
          }
    }else if(index.column()==1)
    {
        if(role==Qt::DisplayRole)return listData.at(index.row()).getName();
        if(role==Qt::DecorationRole){
            QFileIconProvider icon_provider;
            if(listData.at(index.row()).getIsDir())
            {

                QIcon icon=icon_provider.icon(QFileIconProvider::Folder);
                return icon;
            }else
            {
                QIcon icon=icon_provider.icon(QFileIconProvider::File);
                return icon;
            }
        }
    }else if(index.column()==2)
    {
        if(role==Qt::DisplayRole)return listData.at(index.row()).getIsDir()?"文件夹":"文件";
    }else if(index.column()==3)
    {

        if(role==Qt::DisplayRole)
        {

            if(listData.at(index.row()).getIsDir())return "";
            else{
                if(listData.at(index.row()).getSize()>0)
                {
                    qint64 size=listData.at(index.row()).getSize()/1024;
                    return size>1?QString("%1KB").arg(size):QString("1KB");
                }else
                {
                    return QString("0KB");
                }
            }
        }
    }
    return QVariant();
}

void TableFilesModel::onStateChanged(int state)
{

}
