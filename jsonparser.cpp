#include "jsonparser.h"

JSONparser::JSONparser()
{

}

JSONparser::~JSONparser()
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
bool JSONparser::parseSepDataToJSON(const QVector<QVector<QVariant>>& data, const QString& sheetName, QByteArray& container)
{
    if(!data.size())
    {
        lastError = "parseData is empty";
        return false;
    }
    QJsonDocument mainDoc;
    QJsonObject mainObj;
    QJsonArray cells;
    mainObj.insert("valueInputOption","USER_ENTERED");
    char startCol = 'A';
    for(int row = 0; row<data.size();row++)
    {
        for(int col = 0; col<data.at(row).size();col++)
        {
            if(data[row][col].isValid())
            {
                QJsonObject rowObj;
                rowObj.insert("range",sheetName+"!"+QChar(startCol+col)+QString::number(row+1));
                rowObj.insert("majorDimension","ROWS");
                QJsonArray rowarr;
                QJsonArray colarr;
                colarr.append(QJsonValue::fromVariant(data.at(row).at(col)));
                rowarr.append(colarr);
                rowObj.insert("values",rowarr);
                cells.append(rowObj);
            }
        }
    }

    mainObj.insert("data",cells);
    mainDoc.setObject(mainObj);
    container = mainDoc.toJson();
    return true;
}

JSONparser::answerType JSONparser::parseJSONToData(const QByteArray& data, QVector<QVector<QVariant>>& container)
{
    if(data.isEmpty())
    {
        lastError = "Json answer is empty;";
        return JSONerror;
    }
    QJsonParseError jsonErr;
    QJsonDocument mainDoc(QJsonDocument::fromJson(data,&jsonErr));
    if(jsonErr.error!=QJsonParseError::NoError)
    {
        lastError = jsonErr.errorString();
        return JSONerror;
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
        return JSONregularAns;
    }
    else if(mainObj.contains("valueRanges"))
    {
        QJsonArray ranges(mainObj.value("valueRanges").toArray());
        for(int i =0; i<ranges.size();i++)
        {
            QJsonObject cell(ranges.at(i).toObject());
            QRegularExpression exp("!([A-Z]+)(\\d+):?([A-Z]+)?(\\d+)?");
            QString range(cell.value("range").toString());
            QRegularExpressionMatch match = exp.match(range);
            if(match.hasMatch())
            {
                QString startColLetter = match.captured(1);
                QString startRowNum = match.captured(2);
                QString endColLetter = match.captured(3);
                QString endRowNum = match.captured(4);
                if(endColLetter.isEmpty())
                {
                    endColLetter = startColLetter;
                }
                if(endRowNum.isEmpty())
                {
                    endRowNum = startRowNum;
                }
                char start = 'A';
                int firstCol = 0;
                int lastCol = 0;
                foreach(const QChar ch, startColLetter)
                {
                    char next = ch.toLatin1();
                    firstCol += std::abs(next - start);
                }
                foreach(const QChar ch, endColLetter)
                {
                    char next = ch.toLatin1();
                    lastCol+= std::abs(next-start);
                }
                int firstRow = startRowNum.toInt()-1;   //У гугла шэрагі пачынаюцца з 1, а ў масіве
                int lastRow = endRowNum.toInt()-1;      //з 0, таму -1 для карэктыруючага зруху
                if(container.size()<(lastRow+1))
                {
                    container.resize(lastRow+1);
                }
                for(QVector<QVariant>& row:container)
                {
                    if(row.size()<(lastCol+1))
                    {
                        row.resize(lastCol+1);
                    }
                }
                QJsonArray rows = cell.value("values").toArray();
                for(int i = 0; i<rows.size();i++)
                {
                    QJsonArray columns = rows.at(i).toArray();
                    int control_col = firstCol;
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
                        container[firstRow][control_col] = var;
                    }
                    firstRow++;
                }

            }
            else
            {
                lastError="JSON Parser can't initialize string ["+range+"]";
                return JSONerror;
            }
        }
        return JSONseparatedCell;
    }
    else if(mainObj.contains("sheets"))
    {
        lastError.clear();
        QJsonArray sheets(mainObj.value("sheets").toArray());
        for(int i =0;i<sheets.size();i++)
        {
            QJsonObject sheet(sheets.at(i).toObject());
            if(sheet.contains("data"))
            {
                QJsonArray font_data(sheet.value("data").toArray());
                if(!font_data.size())
                {
                    lastError = "JSON Parser can't find font data in the read array;";
                    return JSONerror;
                }
                QJsonObject data_obj(font_data.at(0).toObject());
                QJsonArray row_data(data_obj.value("rowData").toArray());
                container.clear();
                for(int i = 0; i<row_data.size();i++)
                {
                    QVector<QVariant> row;
                    QJsonArray values(row_data.at(i).toObject().value("values").toArray());
                    for(int j = 0; j<values.size();j++)
                    {
                        QJsonObject value(values.at(j).toObject());
                        QJsonObject effective_format(value.value("effectiveFormat").toObject());
                        QJsonObject text_format(effective_format.value("textFormat").toObject());
                        QFont new_font;
                        new_font.setFamily(text_format.value("fontFamily").toString());
                        new_font.setBold(text_format.value("bold").toBool());
                        new_font.setItalic(text_format.value("italic").toBool());
                        row.append(new_font);
                    }
                    container.append(row);
                }
                return JSONFonts;
            }
            QJsonObject properties(sheet.value("properties").toObject());
            lastError.append(properties.value("title").toString()+","+QString::number(properties.value("sheetId").toInt())+"//");
        }
        return JSONSheets;
    }
    else
    {
        parseJSONAnswerToText(mainObj, lastError);
        return JSONwriteReport;
    }
    return JSONerror;
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

bool JSONparser::parseFontsToRequest(const QVector<QVector<QFont>>& data, int sheetID, QByteArray& container)
{
    if(!data.size())
    {
        lastError = "parseData is empty";
        return false;
    }
    QJsonDocument mainDoc;
    QJsonObject mainObj;
    QJsonArray requests;
    QFont defaultFont;
    for(int i = 0; i<data.size();i++)
    {
        for(int j = 0; j<data.at(i).size();j++)
        {
            if(data.at(i).at(j)!=defaultFont)
            {
                QJsonObject styleUpdate;
                QJsonObject range;
                QJsonObject cell;
                QJsonObject userFormat;
                QJsonObject textFormat;
                QString fields("userEnteredFormat.textFormat");
                range.insert("sheetId",sheetID);
                range.insert("startRowIndex",i);
                range.insert("endRowIndex",i+1);
                range.insert("startColumnIndex",j);
                range.insert("endColumnIndex",j+1);
                styleUpdate.insert("range",range);
                if(data.at(i).at(j).italic())
                {
                    textFormat.insert("italic",true);
                }
                if(data.at(i).at(j).bold())
                {
                    textFormat.insert("bold",true);
                }
                if(data.at(i).at(j).family()!=defaultFont.family())
                {
                    textFormat.insert("fontFamily",data.at(i).at(j).family());
                }
                userFormat.insert("textFormat",textFormat);
                cell.insert("userEnteredFormat",userFormat);
                styleUpdate.insert("cell",cell);
                styleUpdate.insert("fields",fields);
                QJsonObject textStyleUpdate;
                textStyleUpdate.insert("repeatCell",styleUpdate);
                requests.append(textStyleUpdate);
            }
        }
    }
    if(requests.isEmpty())
    {
        lastError = "There are no fonts to change;";
        return false;
    }
    mainObj.insert("requests",requests);
    mainDoc.setObject(mainObj);
    container = mainDoc.toJson();
    return true;
}
