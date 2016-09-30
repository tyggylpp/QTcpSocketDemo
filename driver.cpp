#include "driver.h"

Driver::Driver(const QString &rootIndex)
{
    rootPath=rootIndex;
    if(QDir(rootPath).exists())currentDir.setCurrent(rootPath);
    else currentDir.setCurrent(QDir::current().path());
}

const QString Driver::SetRoot(const QString &path)
{
    if(QDir(path).exists())
    {
        currentDir.setCurrent(path);
    }
    return currentDir.path();

}

const QString Driver::GetRoot() const
{
    return this->currentDir.path();
}

const QString Driver::GetRootAbsolutePath() const
{
    return this->currentDir.absolutePath();
}

bool Driver::cd(const QString &name)
{
    bool flag=currentDir.cd(name);
    if(flag)currentDir.setPath(name);
    return flag;
}

bool Driver::cdUp(QString &path)
{
    bool flag=currentDir.cdUp();
    path=currentDir.path();
    return flag;
}
