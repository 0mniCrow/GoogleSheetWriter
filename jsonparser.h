#ifndef JSONPARSER_H
#define JSONPARSER_H

#include <QObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonValueRef>
#include <QJsonParseError>
#include <QJsonObject>
#include <QVariant>
#include <QVector>
#include <QFile>

class JSONparser
{
private:
    QString lastError;
    bool parseJSONAnswerToText(QJsonObject& mainObj, QString& container);
public:
    JSONparser();

    bool parseDataToJSON(const QVector<QVector<QVariant>>& data, const QString& sheetName, QByteArray& container);
    bool parseJSONToData(const QByteArray& data, QVector<QVector<QVariant>>& container);
    //bool saveJsonToFile(const QByteArray& data, const QString& filename);
    //bool loadJsonFromFile(QByteArray& container, const QString& filename);
    QString getLastError() const;

};

#endif // JSONPARSER_H
