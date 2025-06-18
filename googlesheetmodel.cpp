#include "googlesheetmodel.h"

GoogleSheetModel::GoogleSheetModel(QObject *tata):QAbstractTableModel(tata)
{
    rearrangeTable(ROWS,COLUMNS);
    flashChanges = false;
    controlModifier=false;
    return;
}

GoogleSheetModel::GoogleSheetModel(unsigned int rows, unsigned int columns, QObject* tata):QAbstractTableModel(tata)
{
    displayData.resize(rows);
    loadedData.resize(rows);
    for(unsigned int i = 0; i<rows;i++)
    {
        displayData[i].resize(columns);
        loadedData[i].resize(columns);
    }
    flashChanges = false;
    controlModifier=false;
    return;
}

void GoogleSheetModel::rearrangeTable(int rows, int columns)
{
    displayData.resize(rows);
    loadedData.resize(rows);
    for(int i = 0; i<rows;i++)
    {
        displayData[i].resize(columns);
        loadedData[i].resize(columns);
    }
    return;
}

GoogleSheetModel::~GoogleSheetModel()
{
    //?
}

void GoogleSheetModel::setChangesToFlash(bool parameter)
{
    //Каб падсветка запрацавала імгненна, абвяшчаем мадель, што ўсе налады скідваюцца
    beginResetModel();
    flashChanges = parameter;
    endResetModel();
}

bool GoogleSheetModel::checkIndex(const QModelIndex& index) const
{
    if(!index.isValid())
    {
        return false;
    }
    if(index.row()>=displayData.size())
    {
        return false;
    }
    if(index.column()>=displayData.at(index.row()).size())
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
        return displayData.at(index.row()).at(index.column()).data;
    }
    else if(role==Qt::FontRole)
    {
        return displayData.at(index.row()).at(index.column()).font;
    }
    else if(role==Qt::BackgroundRole)
    {
        if(displayData.at(index.row()).at(index.column()).isSelected)
        {                                           //Калі элемент выбраны з клавішай Ctrl
            return QColor(255,253,158);             //ён трапляе ў спіс на індывідуальны запіс
        }
        else if(flashChanges)
        {
            QVariant dataInfo(displayData.at(index.row()).at(index.column()).data);
            if(!dataInfo.isNull())                      //Калі вуза не пустая
            {
                if(index.row()<loadedData.size())       //Калі індэкс запытваемай вузы меньш за даўжыню табліцы,
                {                                       //папярэдне запампованай з сервера
                    if(index.column()<loadedData.at(index.row()).size())    //Калі індэкс вузы меньш за шырыню
                    {                                                       //табліцы, запампованай з сервера
                        QVariant loadedInfo(loadedData.at(index.row()).at(index.column()));
                        if(dataInfo!=loadedInfo)        //Калі дадзеныя ў галоўнай табліцы адрозніваюцца ад
                        {                               //дадзеных у табліцы, запампованай з сервера
                            return QColor(247,111,111); //Фарбуем шпалеры гэтай вузы ў чырвоны колер
                        }
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
        displayData[index.row()][index.column()].data.setValue(subValue);
        emit dataChanged(index,index);
        return true;
    }
    return false;
}

int GoogleSheetModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)
    return displayData.size();
}
int GoogleSheetModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)
    if(displayData.size()>0)
    {
        return displayData.at(0).size();
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
    else
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
        for(QVector<CellObj>&i:displayData)
        {
            i.insert(column,count,CellObj());
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
        if(displayData.size())
        {
            columns = displayData.at(0).size();
        }
        beginInsertRows(parent,row,row+count-1);
        displayData.insert(row,count,QVector<CellObj>(columns));
        endInsertRows();
        return true;
    }
    return false;
}

bool GoogleSheetModel::removeColumns(int column, int count, const QModelIndex& parent)
{
    if(displayData.isEmpty())
    {
        return false;
    }
    if((column+count)>displayData.at(0).size())
    {
        return false;
    }

    beginRemoveColumns(parent,column,column+count-1);
    for(QVector<CellObj>& i:displayData)
    {
        i.remove(column,count);
    }
    endRemoveColumns();
    return true;
}

bool GoogleSheetModel::removeRows(int row, int count, const QModelIndex& parent)
{
    if(displayData.isEmpty())
    {
        return false;
    }
    if((row+count)>displayData.size())
    {
        return false;
    }

    beginRemoveRows(parent,row,row+count-1);
    displayData.remove(row,count);
    endRemoveRows();
    return true;
}

void GoogleSheetModel::loadDataToModel(QVector<QVector<QVariant>>& data)
{
    loadedData = std::move(data);
    if(loadedData.size()>displayData.size())
    {
        insertRows(displayData.size()-1,(loadedData.size()-displayData.size()),
                   QModelIndex());
    }
    else if(loadedData.size()<displayData.size())
    {
        removeRows(0,displayData.size()-loadedData.size(),QModelIndex());
    }
    if(loadedData.size())       //Наладжваецца памер галоўнай табліцы адносна да запампованай
    {
        if(displayData.size())
        {
            if(loadedData.at(0).size()>displayData.at(0).size())
            {
                insertColumns(displayData.at(0).size()-1,
                              loadedData.at(0).size()-displayData.at(0).size(),
                              QModelIndex());
            }
            else if(loadedData.at(0).size()<displayData.at(0).size())
            {
                removeColumns(0,displayData.at(0).size()-loadedData.at(0).size(),
                              QModelIndex());
            }
        }
        else
        {
            insertColumns(0,loadedData.at(0).size(),QModelIndex());
        }
    }
    else if(displayData.size())
    {
        removeColumns(0,displayData.size(),QModelIndex());
    }
    for(int i = 0; i<displayData.size();i++)
    {
        for(int j = 0; j<displayData.at(i).size();j++)
        {
            setData(createIndex(i,j),loadedData[i][j],Qt::EditRole);
        }
    }
    return;
}

void GoogleSheetModel::loadSeparatedData(QVector<QVector<QVariant>>& data)
{
    for(int i = 0; i<data.size();i++)
    {
        for(int j = 0; j<data.at(i).size();j++)
        {
            if(data.at(i).at(j).isValid())
            {
                displayData[i][j].data = loadedData[i][j] = data.at(i).at(j);
                emit dataChanged(createIndex(i,j),createIndex(i,j));
            }
        }
    }
    return;
}

bool GoogleSheetModel::loadFontsFromModel(QVector<QVector<QFont>>& data, bool selectedOnly) const
{
    data.clear();
    bool isChanged = false;
    QFont defaultFont;
    for(int i = 0; i<displayData.size();i++)
    {
        QVector<QFont> row;
        for(int j = 0; j<displayData.at(i).size();j++)
        {
            if(selectedOnly)
            {
                if(displayData.at(i).at(j).isSelected)
                {
                    row.append(displayData.at(i).at(j).font);
                    if(displayData.at(i).at(j).font!=defaultFont)
                    {
                        isChanged = true;
                    }
                }
                else
                {
                    row.append(QFont());
                }
            }
            else
            {
                row.append(displayData.at(i).at(j).font);
                if(displayData.at(i).at(j).font!=defaultFont)
                {
                    isChanged = true;
                }
            }
        }
        data.append(row);
    }
    return isChanged;
}

bool GoogleSheetModel::loadFontsToModel(QVector<QVector<QVariant>>& fonts, bool selectedOnly)
{
    for(int i = 0; i<fonts.size();i++)
    {
        if(i>=displayData.size())
        {
            break;
        }
        for(int j = 0; j<fonts.at(i).size();j++)
        {
            if(j>=displayData.at(i).size())
            {
                break;
            }
            if(selectedOnly)
            {
                if(displayData.at(i).at(j).isSelected)
                {
                    displayData[i][j].font=fonts.at(i).at(j).value<QFont>();
                    emit dataChanged(createIndex(i,j),createIndex(i,j));
                }
                else
                {
                    continue;
                }
            }
            else
            {
                displayData[i][j].font=fonts.at(i).at(j).value<QFont>();
                emit dataChanged(createIndex(i,j),createIndex(i,j));
            }
        }
    }
    //!TODO : выпампаваць дадзеныя шрыфты для табліцы, не зьмяняць памер табліцы, а кантраліраваць рамкіж
    return true;
}

bool GoogleSheetModel::downloadDataFromModel(QVector<QVector<QVariant>>& container, bool selectedOnly) const
{
    if(displayData.isEmpty())
    {
        return false;
    }
    container.clear();
    container.resize(displayData.size());
    for(QVector<QVariant>& row:container)
    {
        row.resize(displayData.at(0).size());
    }
    for(int row = 0; row<displayData.size();row++)
    {
        for(int col = 0; col<displayData.at(row).size();col++)
        {
            if(selectedOnly)
            {
                if(displayData.at(row).at(col).isSelected)
                {
                    container[row][col]=displayData.at(row).at(col).data;
                }
            }
            else
            {
                container[row][col]=displayData.at(row).at(col).data;
            }
        }
    }
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
    if(displayData.isEmpty()||addrmap.isEmpty())
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
    bool rowsSelected = adjustment==displayData.at(0).size();
    bool columnsSelected = rows.size()==displayData.size();
    if(rowsSelected&&columnsSelected)
    {
        return nullptr;
    }
    //Запаўненьне кантэйнера на перадачу вылучанымі элементамі з табліцы
    QByteArray encoded;
    QString coding;
    if(rowsSelected)
    {
        //Калі адзначана, што рухаюцца шэрагі, кантэйнер напаўняецца
        //дадзеннымі, якія трэба ўставіць у новае месца
        QVector<QVector<QVariant>> sentData;
        coding = QString::fromLatin1("application/x-localMovedRow");
        foreach(const int row, rows)
        {
            QVector<QVariant> sentRow;
            QList<int> columns = addrmap.values(row);
            std::sort(columns.begin(),columns.end());
            foreach(const int column,columns)
            {
                sentRow.append(displayData.at(row).at(column).data);
            }
            sentData.append(sentRow);
        }
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
    }
    else if(columnsSelected)
    {
        //Калі высвятлілася, што рухаюцца слупкі, кантэйнер запаўняецца
        //нумарамі слупкоў, якія рухаюцьмуць.
        coding = QString::fromLatin1("application/x-localModedColumn");
        QList<int> columns = addrmap.values(rows.at(0));
        std::sort(columns.begin(),columns.end());
        QDataStream stream(&encoded, QIODevice::WriteOnly);
        foreach(const int column,columns)
        {
            stream<<QVariant(column);
        }
    }
    else
    {
        return nullptr;
    }
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
        if(row!=-1)             //Калі нумар шэрага можна выкарыстаць
        {                       //задаем яго, як пачатковы для ўстаўкі
            beginRow = row;
        }
        else if(parent.isValid())   //Калі карыстальнік "кідае" шэраг на іншы шэраг
        {                           //мы вызначаем індэкс гэтага "бацькі" як пачатковы
            beginRow = parent.row();
        }
        else                        //Калі нельга атрымаць індэкс, пачатковым шэрагам
        {                           //лічыцца канец табліцы
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
        QList<int> sourceColumns;
        while(!stream.atEnd())
        {
            QVariant cell;
            stream>>cell;
            sourceColumns.append(cell.toInt());
        }

        int count = 1;
        int moveShift = 0;
        int sourceColumn = -1;
        for(int i = 0; i<sourceColumns.size();i++)
        {
            if(sourceColumn==-1)
            {
                sourceColumn = sourceColumns.at(i);
            }
            else if(sourceColumns.at(i)!=(sourceColumn+count-1))
            {
                moveColumns(QModelIndex(),sourceColumn+moveShift,count,QModelIndex(),beginColumn);
                if(beginColumn<sourceColumn)
                {
                    moveShift+=count;
                }
                sourceColumn = sourceColumns.at(i);
                count = 1;
            }

            if(sourceColumns.size()==(i+1))
            {
                moveColumns(QModelIndex(),sourceColumn,count,QModelIndex(),beginColumn);
            }
            else
            {
                count++;
            }
        }
        /*
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
        */
    }
    return true;
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
        displayData.insert(destChild+i,displayData.at(sourceRow));
        int removeIndex = destChild>sourceRow?sourceRow:sourceRow+1;
        displayData.removeAt(removeIndex);
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
    for(QVector<CellObj>& row:displayData)
    {
        for(int i = 0; i<count;i++)
        {
            row.insert(destChild>sourceColumn?destChild:destChild+i,
                       row.at(destChild>sourceColumn?sourceColumn:sourceColumn+i));
            int removeIndex = destChild>sourceColumn?sourceColumn:sourceColumn+1+i;
            row.removeAt(removeIndex);
        }
    }
    endMoveColumns();
    return true;
}

void GoogleSheetModel::setControlModifier(bool controlmod)
{
    controlModifier = controlmod;
    return;
}

void GoogleSheetModel::setNewSelectedIndex(QModelIndex selectedIndex)
{
    if(controlModifier)
    {
        if(displayData.at(selectedIndex.row()).at(selectedIndex.column()).isSelected)
        {
            displayData[selectedIndex.row()][selectedIndex.column()].isSelected = false;
        }
        else
        {
            displayData[selectedIndex.row()][selectedIndex.column()].isSelected = true;
        }
        emit dataChanged(selectedIndex,selectedIndex);
    }
    return;
}

QString GoogleSheetModel::getSelectedIndexes() const
{
    QString answer;
    const char frst_ltr = 'A';
    for(int row = 0; row<displayData.size();row++)
    {
        for(int col = 0; col<displayData.at(row).size();col++)
        {
            if(displayData.at(row).at(col).isSelected)
            {
                char letter = frst_ltr+col;
                QString cell(QString(letter)+QString::number(row+1/*У гугла шэрагі пачынаюцца з 1, а ў масіве з 0, таму "+1"*/)+',');
                answer.append(cell);
            }
        }
    }
    return answer;
}


void GoogleSheetModel::cut(const QModelIndex& index)
{
    copy(index);
    displayData[index.row()][index.column()].data.clear();
    emit dataChanged(index,index);
    return;
}

void GoogleSheetModel::copy(const QModelIndex& index)
{
    QClipboard * clipboard(QApplication::clipboard());
    clipboard->setText(displayData[index.row()][index.column()].data.toString());
    return;
}

void GoogleSheetModel::paste(const QModelIndex& index)
{
    QClipboard * clipboard(QApplication::clipboard());
    const QMimeData * mimedata(clipboard->mimeData());
    if(mimedata->hasText())
    {
        displayData[index.row()][index.column()].data.setValue(mimedata->text());
        emit dataChanged(index,index);
    }
    return;
}
void GoogleSheetModel::setFontWeight(const QModelIndex& index, QFont::Weight font_type)
{
    if(displayData.at(index.row()).at(index.column()).font.weight()==font_type)
    {
        displayData[index.row()][index.column()].font.setWeight(QFont::Normal);
    }
    else
    {
        displayData[index.row()][index.column()].font.setWeight(font_type);
    }
    emit dataChanged(index,index);
    return;
}

void GoogleSheetModel::setFontStyle(const QModelIndex& index, QFont::Style font_style)
{
    if(displayData.at(index.row()).at(index.column()).font.style()==font_style)
    {
        displayData[index.row()][index.column()].font.setStyle(QFont::StyleNormal);
    }
    else
    {
        displayData[index.row()][index.column()].font.setStyle(font_style);
    }
    emit dataChanged(index,index);
    return;
}

void GoogleSheetModel::setFont(const QModelIndex& index, const QString& font_name)
{
    if(font_name.isEmpty())
    {
        displayData[index.row()][index.column()].font.setFamily(QFont().family());
    }
    else
    {
        displayData[index.row()][index.column()].font.setFamily(font_name);
    }
    emit dataChanged(index,index);
    return;
}
