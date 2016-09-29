#ifndef DRIVER_H
#define DRIVER_H
#include<QDir>
#include<QFile>
#include<QDebug>
#include<QApplication>
class Driver
{
private:
    QString rootPath;
    QDir currentDir;

public:
   explicit Driver(const QString &rootIndex=QApplication::applicationDirPath());
   const QString SetRoot(const QString &path);
   const QString GetRoot()const;
   const QString GetRootAbsolutePath()const;
   bool cd(const QString &name);
   bool cdUp(QString &path);
};

#endif // DRIVER_H
