#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include"server.h"
#include"client.h"
#include"filemodel.h"
#include<QThread>
#include<driver.h>
#include<QAbstractTableModel>
#include<QTableView>
#include<QFileIconProvider>
#include<delegate.h>

class TableFilesModel :public QAbstractTableModel
{
    Q_OBJECT
public:

    TableFilesModel(QList<FileModel> const list=QList<FileModel>());
    void reFreshModel(QList<FileModel> const list=QList<FileModel>());
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
private:
    QList<bool>listCheck;
    QStringList horizonHead;
    QList<FileModel>listData;
public slots:
    void onStateChanged(int state);
};
namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();
private:
    QList<FileModel*> getModels(const QString &path);
    QList<FileModel> parseJsonToModel(const QString &json);

signals:
    void signalSendData(const QByteArray &data);

private slots:
    void on_pushButton_clicked();
    void on_btn_sersend_clicked();
    void slotShowByteArray(const QByteArray &array);
    void on_btn_up_clicked();
    void on_btn_refresh_clicked();
    void slotRefreshFilesView(const QByteArray &json);
    void slotdoubleClicked(const QModelIndex &index);
private:
    QThread *threadServer;
    QThread *threadClient;
    bool isconnect;
    Ui::Widget *ui;
    Server *server;
    Client *client;
    QList<FileModel>listFile;
    QTableView *table_Files;
    TableFilesModel *tableFilesModel;
    Delegate *delegate;
    const QByteArray ParseDataToSocketStream(const uchar &type, const int &length, const QByteArray &array);
};

#endif // WIDGET_H
