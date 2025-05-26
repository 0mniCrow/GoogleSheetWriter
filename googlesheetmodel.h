#ifndef GOOGLESHEETMODEL_H
#define GOOGLESHEETMODEL_H

#include <QAbstractItemModel>
#include <QAbstractTableModel>
#include <QItemSelectionModel>
#include <QMimeData>
#include <QDataStream>
#include <QIODevice>
#include <QMap>
#include <QMultiMap>
#include <QColor>
#define ROWS 6
#define COLUMNS 5


class GoogleSheetModel:public QAbstractTableModel
{
    Q_OBJECT
private:
    QVector<QVector<QVariant>> dataHolder;
    QVector<QVector<QVariant>> loadedData;
    QMultiMap<int,int> selectedCells;
    bool flashChanges;
    bool controlModifier;
    bool checkIndex(const QModelIndex& index) const;
public:
    GoogleSheetModel(QObject * tata = nullptr);
    ~GoogleSheetModel();
    QVariant data(const QModelIndex& index, int role) const override;
    bool setData(const QModelIndex& index, const QVariant &value, int role) override;
    int rowCount(const QModelIndex& parent) const override;
    int columnCount(const QModelIndex& parent) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;
    bool insertColumns(int column, int count, const QModelIndex &parent) override;
    bool insertRows(int row, int count, const QModelIndex& parent) override;
    bool removeColumns(int column, int count, const QModelIndex& parent) override;
    bool removeRows(int row, int count, const QModelIndex& parent) override;
    Qt::DropActions supportedDragActions() const override;
    Qt::DropActions supportedDropActions() const override;
    QStringList mimeTypes() const override;
    bool moveRows(const QModelIndex& sourceParent, int sourceRow, int count,
                  const QModelIndex& destParent, int destChild) override;
    bool moveColumns(const QModelIndex& sourceParent, int sourceColumn, int count,
                     const QModelIndex& destParent, int destChild) override;
    bool dropMimeData(const QMimeData* data, Qt::DropAction action,
                      int row, int column, const QModelIndex& parent) override;
    QMimeData*  mimeData(const QModelIndexList& indexex) const override;
    bool loadDataToModel(QVector<QVector<QVariant>>& data);
    bool loadSeparatedData(QVector<QVector<QVariant>>& data);
    bool downloadDataFromModel(QVector<QVector<QVariant>>& container) const;
    bool downloadSepDataFromModel(QVector<QVector<QVariant>>& container) const;
    void setChangesToFlash(bool parameter);
    void setControlModifier(bool controlmod);
    QString getSelectedIndexes() const;
public slots:
    void setNewSelectedIndex(QModelIndex selectedIndex);
};

#endif // GOOGLESHEETMODEL_H
