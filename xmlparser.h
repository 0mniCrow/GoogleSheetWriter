#ifndef XMLPARSER_H
#define XMLPARSER_H

#include <QtXml>
/*
class XMLParser
{
public:
    XMLParser();
    static bool saveData(QByteArray& raw_data, const QMap<QString,QString>& data);
    static bool loadData(const QByteArray& raw_data, QMap<QString,QString>& container);
};
*/

void ParseXML_dataToXML(QByteArray& container, const QMap<QString,QString>& data);
bool ParseXML_XMLToData(const QByteArray& raw_data, QMap<QString,QString>& container, QString* errmsg = nullptr);
#endif // XMLPARSER_H


