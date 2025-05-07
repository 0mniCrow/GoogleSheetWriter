#ifndef GOOGLESHEETSMODIFIER_H
#define GOOGLESHEETSMODIFIER_H

#include <QWidget>
#include "googlesheetmodel.h"
#include "httpscommunicator.h"
#include "jsonparser.h"
#include <algorithm>
#include <QFileDialog>
#include <QTextStream>
#include <QRadioButton>
#include <QButtonGroup>

namespace Ui {
class GoogleSheetsModifier;
}

class GoogleSheetsModifier : public QWidget
{
    Q_OBJECT
private:
    QButtonGroup radButGroup;
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
    void chooseAPICredentialFile();
    void loadAPICredentialFile();
    void chooseOAuthCredentialFile();
    void loadOAuthCredentialFile();

    void write();
    void read();
    void save();
    void load();
    void setProgressBar(qint64 val,qint64 total);
    void getErrMsg(const QString& errMsg);
    void getFinishedSignal(const QByteArray& data);
    void authificate();

    void checkRadioGroup();

private:
    Ui::GoogleSheetsModifier *ui;
};

#endif // GOOGLESHEETSMODIFIER_H
