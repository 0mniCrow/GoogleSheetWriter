#ifndef LOGICMANAGER_H
#define LOGICMANAGER_H

#include <QObject>
#include "httpscommunicator.h"
#include "jsonparser.h"
#include "filemanager.h"
#include "xmlparser.h"

class LogicManager : public QObject
{
    Q_OBJECT
private:
    HTTPScommunicator* communicator;
    JSONparser parser;
    FileManager filemanager;
    QMap<QString,int> sheetIDmap;
public:
    explicit LogicManager(QObject *parent = nullptr);
    ~LogicManager();

    // Праца з файламі
    void tableView_saveCurData_toFile();
    void tableView_loadData_fromFile();

    //Аперацыі з уліковымі дадзенымі
    void credentials_APIkey_File_load();
    void credentials_OAuth2_File_load();

    //Аперацыі з google API
    void googleSheetAPI_write();
    void googleSheetAPI_read();
    void googleSheetAPI_getFinishSig(const QByteArray& data);
    void googleSheetAPI_OAuth2_authorize();
    void googleSheetAPI_ReadSheetIDs();

    //Аперацыі з наладамі
    void saveSettings();
    void loadSettings();
    void loadSettings(int & rows,int & columns);

signals:

};

#endif // LOGICMANAGER_H
