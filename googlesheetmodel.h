#ifndef GOOGLESHEETMODEL_H
#define GOOGLESHEETMODEL_H

#include <QAbstractItemModel>
#include <QAbstractTableModel>
#include <QItemSelectionModel>
#include <QColor>
#define ROWS 6
#define COLUMNS 5


class GoogleSheetModel:public QAbstractTableModel
{
    Q_OBJECT
private:
    QVector<QVector<QVariant>> dataHolder;
    QVector<QVector<QVariant>> loadedData;
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

    bool loadDataToModel(QVector<QVector<QVariant>>& data);
    bool downloadDataFromModel(QVector<QVector<QVariant>>& container) const;

};

#endif // GOOGLESHEETMODEL_H
