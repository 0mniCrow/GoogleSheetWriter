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
#include <QFileDialog>

class JSONparser
{
private:
    QString lastError;
public:
    JSONparser();

    bool parseDataToJSON(const QVector<QVector<QVariant>>& data, const QString& sheetName, QByteArray& container);
    bool parseJSONToData(const QByteArray& data, QVector<QVector<QVariant>>& container);
    bool parseJSONAnswerToText(const QByteArray& data, QString& container);
    bool saveJsonToFile(const QByteArray& data, const QString& filename);
    QString getLastError() const;

};

#endif // JSONPARSER_H
