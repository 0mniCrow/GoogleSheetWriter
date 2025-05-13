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
    }
    if(dataHolder.isEmpty())
    {
        return Qt::NoItemFlags;
    }
    if((index.row()<dataHolder.size())&&
            (index.column()<dataHolder.at(0).size()))
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
    mT.append(QString::fromLatin1("application/x-qabstractitemmodeldatalist"));
    return mT;
}

bool GoogleSheetModel::moveRows(const QModelIndex& sourceParent, int sourceRow, int count,
              const QModelIndex& destParent, int destChild)
{
    if(!beginMoveRows(sourceParent,sourceRow,sourceRow+count-1,destParent,destChild))
    {
        return false;
    }
    dataHolder.swapItemsAt(sourceRow,destChild);
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
    for(int i = 0; i<dataHolder.size(); i++)
    {
        dataHolder[i].swapItemsAt(sourceColumn,destChild);
    }
    endMoveColumns();
    return true;
}
