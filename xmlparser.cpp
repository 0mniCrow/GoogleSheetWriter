#include "xmlparser.h"

XMLParser::XMLParser()
{

}
bool XMLParser::saveData(const QString& addr, const QMap<QString,QString>& data)
{
    QDomDocument Settings("savefile");
    QDomElement koranj(Settings.createElement("Settings"));
    Settings.appendChild(koranj);

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
    QDomDocument Settings("savefile");
    QFile file(addr);
    if(!file.open(QIODevice::ReadOnly))
    {
        return false;
    }
    if(!Settings.setContent(&file))
    {
        file.close();
        return false;
    }
    file.close();
    container.clear();
    QDomElement docElem(Settings.documentElement());
    QDomNode n = docElem.firstChild();
    while(!n.isNull())
    {
        QDomElement e = n.toElement();
        container.insert(e.tagName(),e.text());
        n = n.nextSibling();
    }
    return true;
}
