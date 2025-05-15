#include "googlesheetmodel.h"

GoogleSheetModel::GoogleSheetModel(QObject *tata):QAbstractTableModel(tata)
{
    dataHolder.resize(ROWS);
    loadedData.resize(ROWS);
    for(int i = 0; i<ROWS;i++)
    {
        dataHolder[i].resize(COLUMNS);
        loadedData[i].resize(COLUMNS);
    }
    return;
}

GoogleSheetModel::~GoogleSheetModel()
{
    //?
}

bool GoogleSheetModel::checkIndex(const QModelIndex& index) const
{
    if(!index.isValid())
    {
        return false;
    }
    if(index.row()>=dataHolder.size())
    {
        return false;
    }
    if(index.column()>=dataHolder.at(index.row()).size())
    {
        return false;
    }
    return true;
}

QVariant GoogleSheetModel::data(const QModelIndex &index, int role) const
{
    if(!checkIndex(index))
    {
        return QVariant();
    }
    else if(role==Qt::DisplayRole||role==Qt::EditRole)
    {
        return dataHolder.at(index.row()).at(index.column());
    }
    else if(role==Qt::BackgroundRole)
    {
        QVariant dataInfo(dataHolder.at(index.row()).at(index.column()));

        if(!dataInfo.isNull())
        {
            if(index.row()<loadedData.size())
            {
                if(index.column()<loadedData.at(index.row()).size())
                {
                    QVariant loadedInfo(loadedData.at(index.row()).at(index.column()));
                    if(dataInfo!=loadedInfo)
                    {
                        return QColor(247,111,111);
                    }
                }
            }
        }
        return QVariant();
    }
    return QVariant();
}

bool GoogleSheetModel::setData(const QModelIndex& index, const QVariant &value, int role)
{
    if(!checkIndex(index))
    {
        return false;
    }
    if(role==Qt::EditRole)
    {
        QVariant subValue = value;
        if(value.typeId()==QMetaType::QString)
        {
            if(subValue.toString().isEmpty())
            {
                subValue.clear();
            }
        }
        dataHolder[index.row()][index.column()].setValue(subValue);
        emit dataChanged(index,index);
        return true;
    }
    return false;
}

int GoogleSheetModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)
    return dataHolder.size();
}
int GoogleSheetModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)
    if(dataHolder.size()>0)
    {
        return dataHolder.at(0).size();
    }
    return 0;
}
QVariant GoogleSheetModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role!=Qt::DisplayRole)
    {
        return QVariant();
    }
    QVariant answer;
    switch(orientation)
    {
    case Qt::Vertical:
    {
        answer = QString::number(section+1);
    }
        break;
    case Qt::Horizontal:
    {
        char i = 'A';
        answer = QString(QChar(i+static_cast<char>(section)));
    }
        break;
    default:
        break;
    }
    return answer;

}
Qt::ItemFlags GoogleSheetModel::flags(const QModelIndex& index) const
{
    if(!index.isValid())
    {
        return Qt::ItemIsDropEnabled;
    } else
//    if((index.row()<dataHolder.size())&&
//            (index.column()<dataHolder.at(0).size()))
    {
        return QAbstractTableModel::flags(index)|Qt::ItemIsEditable|Qt::ItemIsEnabled|Qt::ItemIsDragEnabled;
    }
    return QAbstractTableModel::flags(index);
}
bool GoogleSheetModel::insertColumns(int column, int count, const QModelIndex& parent)
{
    if(count)
    {
        beginInsertColumns(parent,column,column+count-1);
        for(QVector<QVariant>& i:dataHolder)
        {
            i.insert(column,count,QVariant());
        }
        endInsertColumns();
        return true;
    }
    return false;
}

bool GoogleSheetModel::insertRows(int row, int count, const QModelIndex& parent)
{
    if(count)
    {
        int columns = 0;
        if(dataHolder.size())
        {
            columns = dataHolder.at(0).size();
        }
        beginInsertRows(parent,row,row+count-1);
        dataHolder.insert(row,count,QVector<QVariant>(columns));
        endInsertRows();
        return true;
    }
    return false;
}

bool GoogleSheetModel::removeColumns(int column, int count, const QModelIndex& parent)
{
    if(dataHolder.isEmpty())
    {
        return false;
    }
    if((column+count)>dataHolder.at(0).size())
    {
        return false;
    }

    beginRemoveColumns(parent,column,column+count-1);
    for(QVector<QVariant>& i:dataHolder)
    {
        i.remove(column,count);
    }
    endRemoveColumns();
    return true;
}

bool GoogleSheetModel::removeRows(int row, int count, const QModelIndex& parent)
{
    if(dataHolder.isEmpty())
    {
        return false;
    }
    if((row+count)>dataHolder.size())
    {
        return false;
    }

    beginRemoveRows(parent,row,row+count-1);
    dataHolder.remove(row,count);
    endRemoveRows();
    return true;
}

bool GoogleSheetModel::loadDataToModel(QVector<QVector<QVariant>>& data)
{
    loadedData = std::move(data);
    if(loadedData.size()>dataHolder.size())
    {
        insertRows(dataHolder.size()-1,(loadedData.size()-dataHolder.size()),
                   QModelIndex());
    }
    else if(loadedData.size()<dataHolder.size())
    {
        removeRows(0,dataHolder.size()-loadedData.size(),QModelIndex());
    }
    if(loadedData.size())
    {
        if(dataHolder.size())
        {
            if(loadedData.at(0).size()>dataHolder.at(0).size())
            {
                insertColumns(dataHolder.at(0).size()-1,
                              loadedData.at(0).size()-dataHolder.at(0).size(),
                              QModelIndex());
            }
            else if(loadedData.at(0).size()<dataHolder.at(0).size())
            {
                removeColumns(0,dataHolder.at(0).size()-loadedData.at(0).size(),
                              QModelIndex());
            }
        }
        else
        {
            insertColumns(0,loadedData.at(0).size(),QModelIndex());
        }
    }
    else if(dataHolder.size())
    {
        removeColumns(0,dataHolder.size(),QModelIndex());
    }
    for(int i = 0; i<dataHolder.size();i++)
    {
        for(int j = 0; j<dataHolder.at(i).size();j++)
        {
            setData(createIndex(i,j),loadedData[i][j],Qt::EditRole);
        }
    }
    return true;
}

bool GoogleSheetModel::downloadDataFromModel(QVector<QVector<QVariant>>& container) const
{
    container=dataHolder;
    return true;
}


Qt::DropActions GoogleSheetModel::supportedDragActions() const
{
    return Qt::MoveAction;
}
Qt::DropActions GoogleSheetModel::supportedDropActions() const
{
    return Qt::MoveAction;
}

QStringList GoogleSheetModel::mimeTypes() const
{
    QStringList mT;
    mT.append(QString::fromLatin1("application/vnd.text.list"));
    mT.append(QString::fromLatin1("application/x-localMovedRow"));
    mT.append(QString::fromLatin1("application/x-localModedColumn"));

    return mT;
}



QMimeData* GoogleSheetModel::mimeData(const QModelIndexList& indexes) const
{
    QMultiMap<int,int> addrmap;
    //Сартаванне вылучаных элементаў у выгляд табліцы
    foreach(const QModelIndex& index, indexes)
    {
        if(index.isValid())
        {
            addrmap.insert(index.row(),index.column());
        }
    } 
    if(dataHolder.isEmpty()||addrmap.isEmpty())
    {
        return nullptr;
    }
    QList<int> rows = addrmap.uniqueKeys();
    int adjustment = addrmap.count(addrmap.firstKey());

    //Рэгуляванне даўжыні вылучаных элементаў (шэраг/слупок)
    foreach(const int row, rows)
    {
        if(addrmap.count(row)!=adjustment)
        {
            return nullptr;
        }
    }
    //Праверка, ці не вылучана табліца цалкам
    bool rowsSelected = adjustment==dataHolder.at(0).size();
    bool columnsSelected = rows.size()==dataHolder.size();
    if(rowsSelected&&columnsSelected)
    {
        return nullptr;
    }
    //Запаўненьне кантэйнера на перадачу вылучанымі элементамі з табліцы
    QVector<QVector<QVariant>> sentData;
    foreach(const int row, rows)
    {
        QVector<QVariant> sentRow;
        QList<int> columns = addrmap.values(row);
        std::sort(columns.begin(),columns.end());
        foreach(const int column,columns)
        {
            sentRow.append(dataHolder.at(row).at(column));
        }
        sentData.append(sentRow);
    }
    QString coding;
    if(columnsSelected)
    {
        coding = QString::fromLatin1("application/x-localModedColumn");
    }
    else if(rowsSelected)
    {
        coding = QString::fromLatin1("application/x-localMovedRow");
    }
    else
    {
        return nullptr;
    }

    QByteArray encoded;
    QDataStream stream(&encoded, QIODevice::WriteOnly);
    foreach(const QVector<QVariant>& dataRow,sentData)
    {
        foreach(const QVariant& cell,dataRow)
        {
            stream<<cell;
        }
        QString endSeq(";-nn-;");
        stream<<QVariant(endSeq);
    }
//    foreach(const QModelIndex& index, indexes)
//    {
//        if(index.isValid())
//        {
//            QString val(this->data(index,Qt::DisplayRole).toString());
//            stream<<val;
//        }
//    }
    QMimeData* mimeData = new QMimeData();
    mimeData->setData(/*"application/vnd.text.list"*/coding,encoded);
    return mimeData;
}

bool GoogleSheetModel::dropMimeData(const QMimeData* data, Qt::DropAction action,
                  int row, int column, const QModelIndex& parent)
{
    if(action == Qt::IgnoreAction)
    {
        return true;
    }
    if(!data)
    {
        return false;
    }

    if(data->hasFormat("application/x-localMovedRow"))
    {
        int beginRow = 0;
        if(row!=-1)
        {
            beginRow = row;
        }
        else if(parent.isValid())
        {
            beginRow = parent.row();
        }
        else
        {
            beginRow = rowCount(QModelIndex());
        }
        QByteArray encoded = data->data("application/x-localMovedRow");
        QDataStream stream(&encoded,QIODevice::ReadOnly);
        QVector<QVector<QVariant>> readData;
        QVector<QVariant> readRow;
        while(!stream.atEnd())
        {
            QVariant cell;
            stream>>cell;
            if(QString(cell.typeName())=="QString")
            {
                if(cell.toString()==";-nn-;")
                {
                    readData.append(readRow);
                    readRow.clear();
                    continue;
                }
            }
            readRow.append(cell);
        }
        insertRows(beginRow,readData.size(),QModelIndex());
        for(int rowindex = 0; rowindex<readData.size();rowindex++)
        {
            for(int colindex = 0; colindex<readData.at(rowindex).size();colindex++)
            {
                QModelIndex idx = index(rowindex+beginRow,colindex,QModelIndex());
                setData(idx,readData.at(rowindex).at(colindex),Qt::EditRole);
            }
        }
    }
    else if(data->hasFormat("application/x-localModedColumn"))
    {
        int beginColumn = 0;
        if(row!=-1)
        {
            beginColumn = column;
        }
        else if(parent.isValid())
        {
            beginColumn = parent.column();
        }
        else
        {
            beginColumn = columnCount(QModelIndex());
        }
        QByteArray encoded = data->data("application/x-localModedColumn");
        QDataStream stream(&encoded,QIODevice::ReadOnly);
        QVector<QVector<QVariant>> readData;
        QVector<QVariant> readRow;
        while(!stream.atEnd())
        {
            QVariant cell;
            stream>>cell;
            if(QString(cell.typeName())=="QString")
            {
                if(cell.toString()==";-nn-;")
                {
                    readData.append(readRow);
                    readRow.clear();
                    continue;
                }
            }
            readRow.append(cell);
        }
        insertColumns(beginColumn,readData.at(0).size(),QModelIndex());
        for(int rowindex = 0; rowindex<readData.size();rowindex++)
        {
            for(int colindex = 0; colindex<readData.at(rowindex).size();colindex++)
            {
                QModelIndex idx = index(rowindex,colindex+beginColumn,QModelIndex());
                setData(idx,readData.at(rowindex).at(colindex),Qt::EditRole);
            }
        }
    }
    return true;
//    int beginRow = 0;
//    if(row!=-1)
//    {
//        beginRow = row;
//    }
//    else if(parent.isValid())
//    {
//        beginRow = parent.row();
//    }
//    else
//    {
//        beginRow = rowCount(QModelIndex());
//    }
//    QByteArray encoded = data->data("application/vnd.text.list");
//    QDataStream stream(&encoded,QIODevice::ReadOnly);
//    QStringList items;
//    int rows = 0;

//    while(!stream.atEnd())
//    {
//        QString text;
//        stream>>text;
//        items<<text;
//        ++rows;
//    }
//    //! напэўна тут трэба карыстацца moveRows з moveColumns
//    insertRows(beginRow,rows,QModelIndex());
//    for(const QString& text : qAsConst(items))
//    {
//        QModelIndex idx = index(beginRow,0,QModelIndex());
//        setData(idx,text,Qt::EditRole);
//        beginRow++;
//    }
//    return true;
}

bool GoogleSheetModel::moveRows(const QModelIndex& sourceParent, int sourceRow, int count,
              const QModelIndex& destParent, int destChild)
{
    if(!beginMoveRows(sourceParent,sourceRow,sourceRow+count-1,destParent,destChild))
    {
        return false;
    }
    for(int i = 0; i<count;i++)
    {
        dataHolder.insert(destChild+i,dataHolder.at(sourceRow));
        int removeIndex = destChild>sourceRow?sourceRow:sourceRow+1;
        dataHolder.removeAt(removeIndex);
    }
    endMoveRows();
    return true;
}
bool GoogleSheetModel::moveColumns(const QModelIndex& sourceParent, int sourceColumn, int count,
                 const QModelIndex& destParent, int destChild)
{
    if(!beginMoveColumns(sourceParent,sourceColumn,sourceColumn+count-1,destParent,destChild))
    {
        return false;
    }
    for(QVector<QVariant>& row:dataHolder)
    {
        for(int i = 0; i<count;i++)
        {
            row.insert(destChild+i,row.at(sourceColumn));
            int removeIndex = destChild>sourceColumn?sourceColumn:sourceColumn+1;
            row.removeAt(removeIndex);
        }
    }
    endMoveColumns();
    return true;
}
