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

//    QString ermsg = mainDoc.toJson();
//    lastError = ermsg;
//    return false;

    QJsonObject mainObj(mainDoc.object());
    if(mainObj.contains("values"))
    {
        QJsonArray rows(mainObj.value("values").toArray());
        container.clear();
        int maxcolumnsize=0;
        for(int i = 0; i<rows.size();i++)
        {
            QVector<QVariant> temp;
            QJsonArray columns(rows[i].toArray());
            if(maxcolumnsize<columns.size())
            {
                maxcolumnsize = columns.size();
            }
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
        for(int i =0; i<container.size();i++)
        {
            if(container.at(i).size()<maxcolumnsize)
            {
                container[i].resize(maxcolumnsize);
            }
        }
    }
    else
    {
        parseJSONAnswerToText(mainObj, lastError);
        return false;
    }
    return true;
}

bool JSONparser::parseJSONAnswerToText(QJsonObject &mainObj, QString& container)
{

    container.clear();
    container.append("SpreadSheet ID:"+mainObj.value("spreadsheetId").toString()+";\n");
    if(mainObj.contains("updatedRange")||mainObj.contains("tableRange"))
    {
        bool meta = mainObj.contains("tableRange");
        QJsonObject metaObj;
        if(meta)
        {
            metaObj = mainObj.value("updates").toObject();
            container.append("Appended: \n");
        }
        else
        {
            container.append("Updated: \n");
        }
        QString updRange(meta?metaObj.value("updatedRange").toString():mainObj.value("updatedRange").toString());;
        QStringList list(updRange.split(QLatin1Char('!')));
        container.append("\tUpdated sheet: "+list.at(0)+";\n");
        container.append("\tUpdated range: "+list.at(1)+";\n");
        container.append("\tUpdated rows: "+QString::number(meta?metaObj.value("updatedRows").toInt():
                                               mainObj.value("updatedRows").toInt())+";\n");
        container.append("\tUpdated columns: "+QString::number(meta?metaObj.value("updatedColumns").toInt():
                                                   mainObj.value("updatedColumns").toInt())+";\n");
        container.append("\tUpdated cells: "+QString::number(meta?metaObj.value("updatedCells").toInt():
                                                 mainObj.value("updatedCells").toInt())+";\n");
    }
    else if(mainObj.contains("responses"))
    {
        container.append("Multiple ranges updated: \n");
        container.append("Total updated sheets:"+QString::number(mainObj.value("totalUpdatedSheets").toInt())+";\n\n");
        QJsonArray ranges(mainObj.value("responses").toArray());
        for(int i = 0; i<ranges.size();i++)
        {
            QJsonObject metaObj(ranges.at(i).toObject());
            QStringList list(metaObj.value("updatedRange").toString().split(QLatin1Char('!')));
            container.append("\tUpdated sheet: "+list.at(0)+";\n");
            container.append("\tUpdated range: "+list.at(1)+";\n");
            container.append("\tUpdated rows: "+QString::number(metaObj.value("updatedRows").toInt())+";\n");
            container.append("\tUpdated columns: "+QString::number(metaObj.value("updatedColumns").toInt())+";\n");
            container.append("\tUpdated cells: "+QString::number(metaObj.value("updatedCells").toInt())+";\n\n");
        }
    }
    return true;
}

QString JSONparser::getLastError() const
{
    return lastError;
}

