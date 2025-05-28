#ifndef XMLPARSER_H
#define XMLPARSER_H

#include <QtXml>

class XMLParser
{
public:
    XMLParser();
    bool saveData(const QString& addr, const QMap<QString,QString>& data);
    bool loadData(const QString& addr, QMap<QString,QString>& container);
};

#endif // XMLPARSER_H
