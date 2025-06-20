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
#include <QPoint>
#include <QMenu>
#include <QAction>
#include <QActionGroup>
#include <QFontDatabase>
#include <QTimer>

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
    QMap<QString,int> sheetIDmap;
    QQueue<QWidget*> flashedWidgets;

    bool checkFields(bool ignore_sheetName = false);
    void createConnections();

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;

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
    void tableView_catchContextMenuCall(const QPoint& point);


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
    void googleSheetAPI_ReadSheetIDs();

    //Аперацыі з візуальным інтэрфэйсам
    void setProgressBar(qint64 val,qint64 total);
    void getErrMsg(const QString& errMsg);
    void flashRed(QWidget * widgetToFlash);
    void stopFlashRed();


    //Аперацыі з наладамі
    void saveSettings();
    void loadSettings();
    void loadSettings(int & rows,int & columns);

    void checkRadioGroup();
    void setChangesFlash();
    void setWriteOption();
    void setSelectedCellsOptions();
    void setReadWholeSheet();

private:
    Ui::GoogleSheetsModifier *ui;
};

#endif // GOOGLESHEETSMODIFIER_H
