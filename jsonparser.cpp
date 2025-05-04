#include "jsonparser.h"

JSONparser::JSONparser()
{

}

bool JSONparser::parseDataToJSON(const QVector<QVector<QVariant>>& data, const QString& sheetName, QByteArray& container)
{
    if(!data.size())
    {
        lastError = "parseData is empty";
        return false;
    }
    QJsonDocument mainDoc;
    QJsonObject mainObj;
    //int R1=1,C1=1,Rend=data.size(),Cend=data.at(0).size();
    char va = 'A'+data.at(0).size()-1;

    mainObj.insert("range",sheetName+QString("!A1:")+QChar(va)+QString::number(data.size())
                   /*"!R"+QString::number(R1)+"C"+
                   QString::number(C1)+":R"+QString::number(Rend)+
                   "C"+QString::number(Cend)*/);
    mainObj.insert("majorDimension","ROWS");
    QJsonArray values;
    for(int i = 0; i<data.size();i++)
    {
        QJsonArray temp;
        for(int j = 0; j<data.at(i).size();j++)
        {
            QString type(data.at(i).at(j).typeName());
            QJsonValue val;
            if(type=="QString")
            {
                val= data.at(i).at(j).toString();
            }
            else if(type=="int")
            {
                val = data.at(i).at(j).toInt();
            }
            else if(type=="double")
            {
                val = data.at(i).at(j).toDouble();
            }
            else if(type=="bool")
            {
                val = data.at(i).at(j).toBool();
            }
            else
            {
                val = "null";
            }
            temp.append(val);
        }
        values.append(temp);
    }
    mainObj.insert("values",values);
    mainDoc.setObject(mainObj);
    container = mainDoc.toJson(QJsonDocument::Compact);
    //!__________________
//    QString filename(QFileDialog::getOpenFileName(
//                         nullptr,"Open Json file",QDir::currentPath(),
//                         "JSON file (*.json)",nullptr,QFileDialog::DontUseNativeDialog));
//    QFile file(filename);
//    file.open(QIODevice::WriteOnly);
//    file.write(container);
//    file.close();
//    //!_____________________
    return true;
}

bool JSONparser::parseJSONToData(const QByteArray& data, QVector<QVector<QVariant>>& container)
{
    if(data.isEmpty())
    {
        lastError = "Json answer is empty;";
        return false;
    }
    QJsonParseError jsonErr;
    QJsonDocument mainDoc(QJsonDocument::fromJson(data,&jsonErr));
    if(jsonErr.error!=QJsonParseError::NoError)
    {
        lastError = jsonErr.errorString();
        return false;
    }
    QJsonObject mainObj(mainDoc.object());
    QJsonArray rows(mainObj.value("values").toArray());
    container.clear();
    for(int i = 0; i<rows.size();i++)
    {
        QVector<QVariant> temp;
        QJsonArray columns(rows[i].toArray());
        for(int j = 0; j<columns.size();j++)
        {
            QVariant var;
            if(columns.at(j).isString())
            {
                var = columns.at(j).toString();
            }
            else if(columns.at(j).isDouble())
            {
                var = columns.at(j).toDouble();
            }
            else if(columns.at(j).isBool())
            {
                var = columns.at(j).toBool();
            }
            else
            {
                var = columns.at(j).toInt();
            }
            temp.append(var);
        }
        container.append(temp);
    }
    return true;
}

QString JSONparser::getLastError() const
{
    return lastError;
}
