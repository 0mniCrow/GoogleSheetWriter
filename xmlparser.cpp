#include "xmlparser.h"

XMLParser::XMLParser()
{

}
bool XMLParser::saveData(const QString& addr, const QMap<QString,QString>& data)
{
    QDomDocument saveFile("savefile");
    QFile file(addr);
    if(!file.open(QIODevice::WriteOnly))
    {
        return false;
    }

    /**/
    return true;
}
bool XMLParser::loadData(const QString& addr, QMap<QString,QString>& container)
{
    QDomDocument saveFile("savefile");
    QFile file(addr);
    if(!file.open(QIODevice::ReadOnly))
    {
        return false;
    }
    if(!saveFile.setContent(&file))
    {
        file.close();
        return false;
    }
    file.close();

    QDomElement docElem(saveFile.documentElement());
    return true;
}
