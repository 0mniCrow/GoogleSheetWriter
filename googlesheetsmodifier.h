#ifndef GOOGLESHEETSMODIFIER_H
#define GOOGLESHEETSMODIFIER_H

#include <QWidget>
#include "googlesheetmodel.h"
#include "httpscommunicator.h"
#include "jsonparser.h"
#include <algorithm>
#include <QFileDialog>

namespace Ui {
class GoogleSheetsModifier;
}

class GoogleSheetsModifier : public QWidget
{
    Q_OBJECT
private:
    GoogleSheetModel * model;
    QItemSelectionModel * select_model;
    HTTPScommunicator * communicator;
    JSONparser parser;
    bool readMode;
    bool checkFields();
public:
    explicit GoogleSheetsModifier(QWidget *parent = 0);
    ~GoogleSheetsModifier();

private slots:
    void addRow();
    void addColumn();
    void removeRow();
    void removeColumn();
    void chooseFile();
    void loadFile();
    void write();
    void read();
    void setProgressBar(qint64 val,qint64 total);
    void getErrMsg(const QString& errMsg);
    void getFinishedSignal(const QByteArray& data);
    void authificate();

private:
    Ui::GoogleSheetsModifier *ui;
};

#endif // GOOGLESHEETSMODIFIER_H
