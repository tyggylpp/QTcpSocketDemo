#include "filemodel.h"

FileModel::FileModel()
{
    this->isDir=false;
    this->name="";
    this->path="";
    this->size=0;
}

const QString FileModel::getJsonArray()
{
    QString json="";
    json.append("{");
    json.append(parseJson("name")).append(":").append(parseJson(getName())).append(",");
    json.append(parseJson("path")).append(":").append(parseJson(getPath())).append(",");
    json.append(parseJson("isDir")).append(":").append(parseJson(getIsDir())).append(",");
    json.append(parseJson("size")).append(":").append(parseJson(getSize()));
    json.append("}");
    return json;
}



const QString FileModel::parseJson(const QVariant &string)
{
    if(string.isNull())return "\"\"";
    if(string.type()==QVariant::LongLong)return "\""+string.toString()+"\"";
    if(string.type()==QVariant::Bool)return "\""+string.toString()+"\"";
    if(string.type()==QVariant::String)return "\""+string.toString()+"\"";
    return "";
}
