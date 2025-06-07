#ifndef GOOGLESHEETSMODIFIER_H
#define GOOGLESHEETSMODIFIER_H

#include <QWidget>
#include <QMainWindow>
#include "googlesheetmodel.h"
#include "httpscommunicator.h"
#include "jsonparser.h"
#include "filemanager.h"
#include "xmlparser.h"
#include <algorithm>
#include <QFileDialog>
#include <QTextStream>
#include <QRadioButton>
#include <QButtonGroup>
#include <QPalette>
#include <QKeyEvent>

namespace Ui {
class GoogleSheetsModifier;
}

class GoogleSheetsModifier : public QWidget
{
    Q_OBJECT
private:
    QButtonGroup radButGroup;
    GoogleSheetModel* model;
    QItemSelectionModel* select_model;
    HTTPScommunicator* communicator;
    JSONparser parser;
    FileManager filemanager;
    //XMLParser xmlparser;
    bool checkFields();
    void createConnections();
protected:
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
//    void contextMenuEvent(QContextMenuEvent* event) override;
public:
    explicit GoogleSheetsModifier(QWidget *parent = 0);
    ~GoogleSheetsModifier();

private slots:
    //Аперацыі з табліцай
    void tableView_addRow();
    void tableView_addColumn();
    void tableView_removeRow();
    void tableView_removeColumn();
    void tableView_saveCurData_toFile();
    void tableView_loadData_fromFile();

    //Аперацыі з уліковымі дадзенымі
    void credentials_APIkey_File_choose();
    void credentials_APIkey_File_load();
    void credentials_OAuth2_File_choose();
    void credentials_OAuth2_File_load();

    //Аперацыі з google API
    void googleSheetAPI_write();
    void googleSheetAPI_read();
    void googleSheetAPI_getFinishSig(const QByteArray& data);
    void googleSheetAPI_OAuth2_authorize();


    void setProgressBar(qint64 val,qint64 total);
    void getErrMsg(const QString& errMsg);



    void saveSettings();
    void loadSettings();

    void checkRadioGroup();
    void setChangesFlash();
    void setWriteOption();
    void setSelectedCellsOptions();
    void setReadWholeSheet();

private:
    Ui::GoogleSheetsModifier *ui;
};

#endif // GOOGLESHEETSMODIFIER_H
