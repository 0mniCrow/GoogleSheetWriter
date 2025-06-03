#ifndef XMLPARSER_H
#define XMLPARSER_H

#include <QtXml>

class XMLParser
{
public:
    XMLParser();
    bool saveData(QByteArray& raw_data, const QMap<QString,QString>& data);
    bool loadData(const QByteArray& raw_data, QMap<QString,QString>& container);
};

#endif // XMLPARSER_H
