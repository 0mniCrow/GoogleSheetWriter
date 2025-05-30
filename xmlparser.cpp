#include "xmlparser.h"

XMLParser::XMLParser()
{

}
bool XMLParser::saveData(const QString& addr, const QMap<QString,QString>& data)
{
    QDomDocument saveFile("savefile");
    QFile file(addr);
    /**/
    return true;
}
bool XMLParser::loadData(const QString& addr, QMap<QString,QString>& container)
{
    QDomDocument saveFile("savefile");
    QFile file(addr);
    return true;
}
