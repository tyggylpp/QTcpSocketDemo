#ifndef FILEMODEL_H
#define FILEMODEL_H

#include<QDir>
#include<QVariant>
class FileModel
{
public:
    FileModel();
    void setName(const QString  &name){this->name=name;}
    void setPath(const QString  &path){this->path=path;}
    void setIsDir(const bool isDir){this->isDir=isDir;}
    void setSize(const qint64 size){this->size=size;}
    QString getName()const{return this->name;}
    QString getPath()const{return this->path;}
    bool getIsDir()const{return this->isDir;}
    qint64 getSize()const{return this->size;}
    const QString getJsonArray();
    //void setSize(const qint64 &size){this->size=size;}
private:
    QString name;
    QString path;
    bool isDir;
    qint64 size;
    const QString parseJson(const QVariant &string);
    //QFileInfo fileInfo;

};

#endif // FILEMODEL_H
