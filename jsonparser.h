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
#include <QRegularExpression>
#include <QRegularExpressionMatch>

class JSONparser
{
private:
    QString lastError;
    bool parseJSONAnswerToText(QJsonObject& mainObj, QString& container);
    int checkNextCol(const QString& prev_param, int prevCount);
    int checkNextRow(const QString& prev_param, int prevCount);
public:
    JSONparser();
    ~JSONparser();
    enum answerType{ JSONerror = 0, JSONregularAns = 1, JSONwriteReport =2, JSONseparatedCell = 3 };
    bool parseDataToJSON(const QVector<QVector<QVariant>>& data, const QString& sheetName, QByteArray& container);
    answerType parseJSONToData(const QByteArray& data, QVector<QVector<QVariant>>& container);
    bool parseSepDataToJSON(const QVector<QVector<QVariant> > &data, const QString &sheetName, QByteArray &container);
    bool parseFontsToRequest(const QVector<QVector<QFont>>& data,const QString &sheetName, QByteArray& container);

    QString getLastError() const;

};

#endif // JSONPARSER_H
