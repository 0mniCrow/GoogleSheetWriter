#include "googlesheetsmodifier.h"
#include "ui_googlesheetsmodifier.h"

GoogleSheetsModifier::GoogleSheetsModifier(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GoogleSheetsModifier)
{
    ui->setupUi(this);
    model = new GoogleSheetModel();
    select_model = ui->tableGoogleSheets->selectionModel();
    ui->tableGoogleSheets->setModel(model);
    delete select_model;
    select_model = ui->tableGoogleSheets->selectionModel();
    ui->tableGoogleSheets->setDragDropMode(QAbstractItemView::InternalMove);
    ui->tableGoogleSheets->setDragDropOverwriteMode(false);
    ui->tableGoogleSheets->setDefaultDropAction(Qt::MoveAction);
    ui->tableGoogleSheets->setContextMenuPolicy(Qt::CustomContextMenu);
    communicator = new HTTPScommunicator();
    radButGroup.addButton(ui->radioButtonAPI_key);
    radButGroup.addButton(ui->radioButton_OAuth2);
    checkRadioGroup();
    setWriteOption();
    createConnections();
    int start_rows = 0;
    int start_columns = 0;
    loadSettings(start_rows,start_columns);
    if(start_rows&&start_columns)
    {
        model->rearrangeTable(start_rows,start_columns);
    }
    loadSettings();


    return;
}

GoogleSheetsModifier::~GoogleSheetsModifier()
{
    delete ui;
    if(model)
    {
        delete model;
    }
    if(communicator)
    {
        delete communicator;
    }
    return;
}

void GoogleSheetsModifier::createConnections()
{
    connect(ui->ButtonAddColumn,SIGNAL(clicked(bool)),this,SLOT(tableView_addColumn()));
    connect(ui->ButtonAddRow,SIGNAL(clicked(bool)),this,SLOT(tableView_addRow()));
    connect(ui->ButtonRemoveColumn,SIGNAL(clicked(bool)),this,SLOT(tableView_removeColumn()));
    connect(ui->ButtonRemoveRow,SIGNAL(clicked(bool)),this,SLOT(tableView_removeRow()));
    connect(ui->Button_API_key_file_choose,SIGNAL(clicked(bool)),this,SLOT(credentials_APIkey_File_choose()));
    connect(ui->Button_API_key_file_load,SIGNAL(clicked(bool)),this,SLOT(credentials_APIkey_File_load()));
    connect(ui->Button_OAuth_choosefile,SIGNAL(clicked(bool)),this,SLOT(credentials_OAuth2_File_choose()));
    connect(ui->Button_OAuth_LoadFile,SIGNAL(clicked(bool)),this,SLOT(credentials_OAuth2_File_load()));
    connect(ui->ButtonRead,SIGNAL(clicked(bool)),this,SLOT(googleSheetAPI_read()));
    connect(ui->ButtonWrite,SIGNAL(clicked(bool)),this,SLOT(googleSheetAPI_write()));
    connect(communicator,SIGNAL(progress(qint64,qint64)),this,SLOT(setProgressBar(qint64,qint64)));
    connect(communicator,SIGNAL(errormsg(QString)),this,SLOT(getErrMsg(QString)));
    connect(communicator,SIGNAL(finished(QByteArray)),this,SLOT(googleSheetAPI_getFinishSig(QByteArray)));
    connect(ui->ButtonAuth,SIGNAL(clicked(bool)),this,SLOT(googleSheetAPI_OAuth2_authorize()));
    connect(ui->Button_ReadSheetIDs,SIGNAL(clicked(bool)),this,SLOT(googleSheetAPI_ReadSheetIDs()));
    connect(ui->ButtonSave,SIGNAL(clicked(bool)),this,SLOT(tableView_saveCurData_toFile()));
    connect(ui->ButtonLoad,SIGNAL(clicked(bool)),this,SLOT(tableView_loadData_fromFile()));
    connect(ui->radioButtonAPI_key,SIGNAL(clicked(bool)),this,SLOT(checkRadioGroup()));
    connect(ui->radioButton_OAuth2,SIGNAL(clicked(bool)),this,SLOT(checkRadioGroup()));
    connect(ui->radioButton_sheetName_Manual,SIGNAL(clicked(bool)),this,SLOT(checkRadioGroup()));
    connect(ui->radioButton_sheetName_auto,SIGNAL(clicked(bool)),this,SLOT(checkRadioGroup()));
    connect(ui->Button_Save_settings,SIGNAL(clicked(bool)),this,SLOT(saveSettings()));
    connect(ui->Button_LoadSettings,SIGNAL(clicked(bool)),this,SLOT(loadSettings()));
    connect(ui->checkBox_FlashChanges,SIGNAL(stateChanged(int)),this,SLOT(setChangesFlash()));
    connect(ui->radioButton_option_append,SIGNAL(clicked(bool)),this,SLOT(setWriteOption()));
    connect(ui->radioButton_option_rewrite,SIGNAL(clicked(bool)),this,SLOT(setWriteOption()));
    connect(ui->tableGoogleSheets,SIGNAL(clicked(QModelIndex)),model,SLOT(setNewSelectedIndex(QModelIndex)));
    connect(ui->checkBox_Selected_cells_work,SIGNAL(clicked(bool)),this,SLOT(setSelectedCellsOptions()));
    connect(ui->checkBox_readWholeTable,SIGNAL(clicked(bool)),this,SLOT(setReadWholeSheet()));
    connect(ui->tableGoogleSheets,&QTableView::customContextMenuRequested,
            this,&GoogleSheetsModifier::tableView_catchContextMenuCall);

    return;
}


void GoogleSheetsModifier::setProgressBar(qint64 val,qint64 total)
{
    ui->progressBar->setValue(val*100/total);
    return;
}

void GoogleSheetsModifier::getErrMsg(const QString &errMsg)
{
    ui->textEdit->append(errMsg);
    return;
}

void GoogleSheetsModifier::googleSheetAPI_OAuth2_authorize()
{
    if((!ui->lineClientID->text().isEmpty())&&(!ui->lineClientSecret->text().isEmpty()))
    {
        communicator->AuthorizeRequest(ui->lineClientID->text(),ui->lineClientSecret->text());
        return;
    }
    flashRed(ui->lineClientID);
    flashRed(ui->lineClientSecret);
    getErrMsg("authification fields are empty;");
    return;
}

void GoogleSheetsModifier::tableView_addRow()
{
    QModelIndexList indexList(select_model->selectedIndexes());
    if(indexList.isEmpty())
    {
        model->insertRows(model->rowCount(QModelIndex()),1,QModelIndex());
        return;
    }
    QModelIndexList::iterator max = std::max_element(indexList.begin(),indexList.end(),
                                                     [](const QModelIndex& a, const QModelIndex& b)
    {
        return a.row()<b.row();
    });
    model->insertRows(max->row()+1,1,QModelIndex());
    return;
}

void GoogleSheetsModifier::tableView_addColumn()
{
    QModelIndexList indexList(select_model->selectedIndexes());
    if(indexList.isEmpty())
    {
        model->insertColumns(model->columnCount(QModelIndex()),1,QModelIndex());
        return;
    }
    QModelIndexList::iterator max = std::max_element(indexList.begin(),indexList.end(),
                                                     [](const QModelIndex& a, const QModelIndex& b)
    {
        return a.column()<b.column();
    });
    model->insertColumns(max->column()+1,1,QModelIndex());
    return;
}

void GoogleSheetsModifier::tableView_removeRow()
{
    QModelIndexList indexList(select_model->selectedIndexes());
    if(indexList.isEmpty())
    {
        return;
    }
    QList<int> rowlist;
    for(QList<QModelIndex>::iterator it = indexList.begin();it!=indexList.end();it++)
    {
        if(!rowlist.contains(it->row()))
        {
            rowlist.append(it->row());
        }
    }
    for(int i = rowlist.size()-1;i>=0;i--)
    {
        model->removeRows(rowlist.at(i),1,QModelIndex());
    }
    return;
}

void GoogleSheetsModifier::tableView_removeColumn()
{
    QModelIndexList indexList(select_model->selectedIndexes());
    if(indexList.isEmpty())
    {
        return;
    }
    QList<int> columnlist;
    for(QList<QModelIndex>::iterator it = indexList.begin();it!=indexList.end();it++)
    {
        if(!columnlist.contains(it->column()))
        {
            columnlist.append(it->column());
        }
    }
    for(int i = columnlist.size()-1;i>=0;i--)
    {
        model->removeColumns(columnlist.at(i),1,QModelIndex());
    }
    return;
}

void GoogleSheetsModifier::credentials_APIkey_File_choose()
{
    QString lastpath(filemanager.getlastfilepath());
    if(lastpath.isEmpty())
    {
        lastpath = QDir::currentPath();
    }
    QString filename(QFileDialog::getOpenFileName(
                         this,"Open API key file",lastpath,
                         "Text file (*.txt)",nullptr,QFileDialog::DontUseNativeDialog));

    //QString controlpath = QDir::cleanPath(filename);

    ui->lineAPI_key_filename->setText(filename);
    if(!filename.isEmpty())
    {
        filemanager.setlastfilepath(filename.left(filename.lastIndexOf('.')));
        credentials_APIkey_File_load();
    }
    return;
}

void GoogleSheetsModifier::credentials_APIkey_File_load()
{
    QString filename(ui->lineAPI_key_filename->text());
    if(filename.isEmpty())
    {
        flashRed(ui->lineAPI_key_filename);
        getErrMsg("File name to load API_Key is empty;");
        return;

    }
    QString Api_key;
    if(!filemanager.openAPIfile(filename,Api_key))
    {
        getErrMsg("Can't open ["+filename+"] file to load API_Key;");
    }
    ui->line_API_Key->setText(Api_key);
    return;
}

void GoogleSheetsModifier::credentials_OAuth2_File_choose()
{
    QString lastpath(filemanager.getlastfilepath());
    if(lastpath.isEmpty())
    {
        lastpath = QDir::currentPath();
    }
    QString filename(QFileDialog::getOpenFileName(
                         this,"Open OAuth2 credential file",lastpath,
                         "Text file (*.txt)",nullptr,QFileDialog::DontUseNativeDialog));
    ui->lineEdit_OAuth_filename->setText(filename);

    //QString controlpath = QDir::cleanPath(filename);
    if(!filename.isEmpty())
    {
        filemanager.setlastfilepath(filename.left(filename.lastIndexOf('.')));
        credentials_OAuth2_File_load();
    }
    return;
}

void GoogleSheetsModifier::credentials_OAuth2_File_load()
{
    QString filename(ui->lineEdit_OAuth_filename->text());
    if(filename.isEmpty())
    {
        flashRed(ui->lineEdit_OAuth_filename);
        getErrMsg("File name to load OAuth2 credentials is empty;");
        return;
    }
    QStringList data;
    if(!filemanager.openOAuthFile(filename,data))
    {
        getErrMsg("Can't open ["+filename+"] file to load OAuth2 credentials;");
        return;
    }
    for(const QString& str:data)
    {
        if(str.contains("Client ID:"))
        {
            ui->lineClientID->setText(str.sliced(str.indexOf(':')+1));
        }
        else if(str.contains("Client secret:"))
        {
            ui->lineClientSecret->setText(str.sliced(str.indexOf(':')+1));
        }
    }
    return;
}


bool GoogleSheetsModifier::checkFields(bool ignore_sheetName)
{ 
    if(!ignore_sheetName)
    {
        if(ui->radioButton_sheetName_Manual->isChecked())
        {
            if(ui->lineSheetName->text().isEmpty())
            {
                flashRed(ui->lineSheetName);
                getErrMsg("Credentials missing Error: Sheet Name is missing;");
                return false;
            }
        }
        else if(ui->radioButton_sheetName_auto->isChecked())
        {
            if(!ui->comboBox_SheetNames->count())
            {
                flashRed(ui->comboBox_SheetNames);
                getErrMsg("Credentials missing Error: Sheet Name is missing;");
                return false;
            }
        }
    }
    if(ui->lineSpreadSheetID->text().isEmpty())
    {
        flashRed(ui->lineSpreadSheetID);
        getErrMsg("Credentials missing Error: Spread Sheet ID is empty;");
        return false;
    }
    if(communicator->getFlags()&HTTPScommunicator::oauth2Method)
    {
        if((ui->lineClientID->text().isEmpty())||
            (ui->lineClientSecret->text().isEmpty()))
        {
            flashRed(ui->lineClientID);
            flashRed(ui->lineClientSecret);
            getErrMsg("Credentials missing Error: OAuth2 credentials are empty;");
            return false;
        }

    }
    else
    {
        if(ui->line_API_Key->text().isEmpty())
        {
            flashRed(ui->line_API_Key);
            getErrMsg("Credentials missing Error: API_Key is empty;");
            return false;
        }
    }
    return true;
}

void GoogleSheetsModifier::googleSheetAPI_write()
{
    if(!checkFields())
    {
        getErrMsg("Write method Error: Nessesary fields are empty;");
        return;
    }
    if(!(communicator->isAuthorized()))
    {
        getErrMsg("Write method Error: User wasn't authorized;");
        return;
    }
    QVector<QVector<QVariant>> container;
    QByteArray jsonData;
    QString range;
    QString sheetName(ui->radioButton_sheetName_Manual->isChecked()?
                          ui->lineSheetName->text():
                          ui->comboBox_SheetNames->currentText());
    if(ui->checkBox_Selected_cells_work->isChecked())
    {
//        if(ui->checkBox_writeFormating->isChecked())
//        {
//            getErrMsg("Program can't work both in \"Write Formatting\" and \"Selected Sells\" modes");
//            return;
//        }
        if(!model->downloadDataFromModel(container,true))
        {
            getErrMsg("Can't load separated data from model");
            return;
        }

        if(!parser.parseSepDataToJSON(container,sheetName,jsonData))
        {
            getErrMsg(parser.getLastError());
            return;
        }
        range = "BathUpdate";
    }
    else
    {
        model->downloadDataFromModel(container);

        if(!parser.parseDataToJSON(container,sheetName,jsonData))
        {
            getErrMsg(parser.getLastError());
            return;
        }
        char va = 'A'+container.at(0).size()-1;
        range=QString("A1:")+QChar(va)+QString::number(container.size());
    }
    //QString range("R1C1:R"+QString::number(container.size())+"C"+QString::number(container.at(0).size()));
    communicator->writeRequest(ui->lineSpreadSheetID->text(),
                               sheetName,
                               range,jsonData);
    return;
}

void GoogleSheetsModifier::googleSheetAPI_read()
{
    if(!checkFields())
    {
        getErrMsg("Read method Error: Nessesary fields are empty;");
        return;
    }
    int rows = model->rowCount(QModelIndex());
    int columns = model->columnCount(QModelIndex());
    if((!rows)&&(!columns))
    {
        getErrMsg("Read method Error: Range of reading isn't selected (Create nessesary amount of rows and columns);");
        return;
    }
    QString range;
    if(ui->checkBox_Selected_cells_work->isChecked()&&
            (!(communicator->getFlags()&HTTPScommunicator::w_Fonts)))
    {
        range = model->getSelectedIndexes();
        if(range.isEmpty())
        {
            getErrMsg("Read method Error: selected indexes are empty;");
            return;
        }
    }
    else
    {
        char va = 'A'+ columns -1;
        range = QString("A1:")+QChar(va)+QString::number(rows);
    }
    QString sheetName(ui->radioButton_sheetName_Manual->isChecked()?
                          ui->lineSheetName->text():
                          ui->comboBox_SheetNames->currentText());
    //QString range("R1C1:R"+QString::number(rows)+"C"+QString::number(columns));
    communicator->readRequest(ui->lineSpreadSheetID->text()
                             ,sheetName,
                             range,ui->line_API_Key->text());
    return;
}

void GoogleSheetsModifier::googleSheetAPI_ReadSheetIDs()
{
    if(!checkFields(true))
    {
        getErrMsg("Read method Error: Nessesary fields are empty;");
        return;
    }
    communicator->GetSheetIdsRequest(ui->lineSpreadSheetID->text(),ui->line_API_Key->text());
    return;
}

void GoogleSheetsModifier::tableView_saveCurData_toFile()
{
    QString lastpath(filemanager.getlastfilepath());
    if(lastpath.isEmpty())
    {
        lastpath = QDir::currentPath();
    }
    QString filename(QFileDialog::getSaveFileName(
                         this,"Save JSON file",lastpath,
                         "JSON (*.json)",nullptr,QFileDialog::DontUseNativeDialog));
    QVector<QVector<QVariant>> rawData;
    model->downloadDataFromModel(rawData);
    QByteArray JSONdata;
    if(!parser.parseDataToJSON(rawData,QString(),JSONdata))
    {
        getErrMsg(parser.getLastError());
        return;
    }
    if(!filename.endsWith(".json"))
    {
        filename.append(".json");
    }
    if(!filemanager.saveJSONdataToFile(JSONdata,filename))
    {
        getErrMsg("Can't save JSON data to file ["+filename+"];");
        return;
    }
    filemanager.setlastfilepath(filename.left(filename.lastIndexOf('.')));
    return;
}
void GoogleSheetsModifier::tableView_loadData_fromFile()
{
    QString lastpath(filemanager.getlastfilepath());
    if(lastpath.isEmpty())
    {
        lastpath = QDir::currentPath();
    }
    QString filename(QFileDialog::getOpenFileName(
                         this,"Load JSON file",lastpath,
                         "JSON (*.json)",nullptr,QFileDialog::DontUseNativeDialog));
    QByteArray JSONdata;
    if(!filemanager.loadJSONdataFromFile(JSONdata,filename))
    {
        getErrMsg("Can't load JSON data from file ["+filename+"];");
        return;
    }
    filemanager.setlastfilepath(filename.left(filename.lastIndexOf('.')));
    googleSheetAPI_getFinishSig(JSONdata);
    return;
}

//Метад выклікаецца адказам сервера на HTTP запыт
void GoogleSheetsModifier::googleSheetAPI_getFinishSig(const QByteArray& data)
{
    QVector<QVector<QVariant>> modelData;
    switch(parser.parseJSONToData(data,modelData))
    {
    case JSONparser::JSONerror:
    {
        getErrMsg(parser.getLastError());
    }
        break;
    case JSONparser::JSONwriteReport:
    {
        getErrMsg(parser.getLastError());
        if(ui->checkBox_writeFormating->isChecked())
        {
            if(communicator->getFlags()&HTTPScommunicator::w_Fonts)
            {
                communicator->setFlags(communicator->getFlags()&(~HTTPScommunicator::w_Fonts));
            }
            else if(ui->comboBox_SheetNames->isEnabled()&&ui->comboBox_SheetNames->count())
            {
                int sheetID(sheetIDmap.value(ui->comboBox_SheetNames->currentText()));
                QVector<QVector<QFont>> fonts;
                if(model->loadFontsFromModel(fonts,ui->checkBox_Selected_cells_work->isChecked()))
                {
                    QByteArray request;
                    if(parser.parseFontsToRequest(fonts,sheetID,request))
                    {
                        //filemanager.saveJSONdataToFile(request,QDir::currentPath()+"/fonts.json");

                        communicator->setFlags(communicator->getFlags()|HTTPScommunicator::w_Fonts);
                        communicator->writeRequest(ui->lineSpreadSheetID->text(),
                                                   ui->comboBox_SheetNames->currentText(),
                                                   "bathUpdate",request);
                    }
                    else
                    {
                        getErrMsg(parser.getLastError());
                    }
                }
            }
            else
            {
                getErrMsg("Can't upload fonts: no sheetID was load/selected;");
            }
        }
    }
        break;
    case JSONparser::JSONregularAns:
    {
        model->loadDataToModel(modelData);
        ui->tableGoogleSheets->resizeColumnsToContents();
        if(ui->checkBox_writeFormating->isChecked())
        {
            communicator->setFlags(communicator->getFlags()|HTTPScommunicator::w_Fonts);
            googleSheetAPI_read();
            //! Дадаць апрацоўку запампоўкі шрыфтоў
        }

    }
        break;
    case JSONparser::JSONseparatedCell:
    {
        model->loadSeparatedData(modelData);
        ui->tableGoogleSheets->resizeColumnsToContents();
        if(ui->checkBox_writeFormating->isChecked())
        {
            communicator->setFlags(communicator->getFlags()|HTTPScommunicator::w_Fonts);
            googleSheetAPI_read();
        }

    }
        break;
    case JSONparser::JSONSheets:
    {
        sheetIDmap.clear();
        ui->comboBox_SheetNames->clear();
        QStringList listOfIds(parser.getLastError().split("//",Qt::SkipEmptyParts));
        foreach(const QString& val,listOfIds)
        {
            QStringList pair(val.split(",",Qt::SkipEmptyParts));
            sheetIDmap.insert(pair.at(0),pair.at(1).toInt());
            ui->comboBox_SheetNames->addItem(pair.at(0),pair.at(1));
        }
    }
        break;
    case JSONparser::JSONFonts:
    {
        communicator->setFlags(communicator->getFlags()&(~HTTPScommunicator::w_Fonts));
        model->loadFontsToModel(modelData,ui->checkBox_Selected_cells_work->isChecked());
        ui->tableGoogleSheets->resizeColumnsToContents();
    }
        break;
    default:
    {
        getErrMsg("This json flag is not processed!");
    }
    }

    return;
}

void GoogleSheetsModifier::checkRadioGroup()
{
    if(ui->radioButtonAPI_key->isChecked())
    {
        ui->groupBox_API_KEY->setEnabled(true);
        ui->groupBox_OAuth2->setEnabled(false);
        ui->ButtonWrite->setEnabled(false);
        ui->groupBox_write_option->setEnabled(false);
        communicator->setFlags(communicator->getFlags()&(~HTTPScommunicator::oauth2Method));
    }
    else if(ui->radioButton_OAuth2->isChecked())
    {
        ui->groupBox_OAuth2->setEnabled(true);
        ui->groupBox_API_KEY->setEnabled(false);
        ui->ButtonWrite->setEnabled(true);
        ui->groupBox_write_option->setEnabled(true);
        communicator->setFlags(communicator->getFlags()|HTTPScommunicator::oauth2Method);
    }

    if(ui->radioButton_sheetName_Manual->isChecked())
    {
        ui->lineSheetName->setEnabled(true);
        ui->comboBox_SheetNames->setEnabled(false);
        ui->Button_ReadSheetIDs->setEnabled(false);
    }
    else if(ui->radioButton_sheetName_auto->isChecked())
    {
        ui->lineSheetName->setEnabled(false);
        ui->comboBox_SheetNames->setEnabled(true);
        ui->Button_ReadSheetIDs->setEnabled(true);
    }
    return;
}


void GoogleSheetsModifier::saveSettings()
{
    QMap<QString,QString> settings;
    QByteArray data;
    settings.insert("Table_rows",QString::number(model->rowCount(QModelIndex())));
    settings.insert("Table_columns",QString::number(model->columnCount(QModelIndex())));
    settings.insert("API_Key_filename",ui->lineAPI_key_filename->text());
    settings.insert("OAuth2_filename",ui->lineEdit_OAuth_filename->text());
    settings.insert("Sheet_Name",ui->lineSheetName->text());
    settings.insert("SpreadSheetID",ui->lineSpreadSheetID->text());
    settings.insert("LastDirectoryPath",filemanager.getlastfilepath());
    settings.insert("API_Key_method",ui->radioButtonAPI_key->isChecked()?"Y":"N");
    settings.insert("OAuth2_method",ui->radioButton_OAuth2->isChecked()?"Y":"N");
    settings.insert("SheetName_manualInput",ui->radioButton_sheetName_Manual->isChecked()?"Y":"N");
    settings.insert("SheetName_autoCheck",ui->radioButton_sheetName_auto->isChecked()?"Y":"N");
    settings.insert("Write_rewrite_opt",ui->radioButton_option_rewrite->isChecked()?"Y":"N");
    settings.insert("Write_append_opt",ui->radioButton_option_append->isChecked()?"Y":"N");
    settings.insert("Flash_Changes",ui->checkBox_FlashChanges->isChecked()?"Y":"N");
    settings.insert("Read_Whole_Table",ui->checkBox_readWholeTable->isChecked()?"Y":"N");
    settings.insert("R_W_Separate_Cells",ui->checkBox_Selected_cells_work->isChecked()?"Y":"N");
    settings.insert("R_W_Formatting",ui->checkBox_writeFormating->isChecked()?"Y":"N");
    ParseXML_dataToXML(data,settings);
    if(data.isEmpty())
    {
        getErrMsg("Application can't save settings;");
        return;
    }
    if(!filemanager.savePreferences(data))
    {
        getErrMsg("Application can't save settings;");
    }
    return;
}
void GoogleSheetsModifier::loadSettings(int &rows, int &columns)
{
    QByteArray data;
    QMap<QString,QString> settings;

    if(!filemanager.loadPreferences(data))
    {
        getErrMsg("Application can't load settings;");
        return;
    }
    QString err;
    if(!ParseXML_XMLToData(data,settings,&err))
    {
        getErrMsg(err);
        return;
    }
    for(QMap<QString,QString>::const_iterator it = settings.constBegin();
        it!= settings.constEnd();it++)
    if(it.key()=="Table_rows")
    {
        rows = it.value().toInt();
    }
    else if(it.key()=="Table_columns")
    {
        columns = it.value().toInt();
    }
    return;
}

void GoogleSheetsModifier::loadSettings()
{
    QByteArray data;
    QMap<QString,QString> settings;

    if(!filemanager.loadPreferences(data))
    {
        getErrMsg("Application can't load settings;");
        return;
    }
    QString err;
    if(/*!xmlparser.loadData(data,settings)*/!ParseXML_XMLToData(data,settings,&err))
    {
        getErrMsg(err);
        return;
    }
    for(QMap<QString,QString>::const_iterator it = settings.constBegin();
        it!= settings.constEnd();it++/*const QString& str:settings*/)
    {
        if(it.key()==("API_Key_filename"))
        {
            ui->lineAPI_key_filename->setText(it.value()/*str.sliced(str.indexOf(':')+1)*/);
        }
        else if(it.key()==("OAuth2_filename"))
        {
            ui->lineEdit_OAuth_filename->setText(it.value());
        }
        else if(it.key()=="Sheet_Name")
        {
            ui->lineSheetName->setText(it.value());
        }
        else if(it.key()==("SpreadSheetID"))
        {
            ui->lineSpreadSheetID->setText(it.value());
        }
        else if(it.key()=="LastDirectoryPath")
        {
            filemanager.setlastfilepath(it.value());
        }
        else if(it.key()=="API_Key_method")
        {
            if(it.value()=="Y")
            {
                ui->radioButtonAPI_key->setChecked(true);
            }
            else
            {
                ui->radioButtonAPI_key->setChecked(false);
            }
        }
        else if(it.key()==("OAuth2_method"))
        {
            if(it.value()=="Y")
            {
                ui->radioButton_OAuth2->setChecked(true);
            }
            else
            {
                ui->radioButton_OAuth2->setChecked(false);
            }
        }
        else if(it.key()=="SheetName_manualInput")
        {
            if(it.value()=="Y")
            {
                ui->radioButton_sheetName_Manual->setChecked(true);
            }
            else
            {
                ui->radioButton_sheetName_Manual->setChecked(false);
            }
        }
        else if(it.key()=="SheetName_autoCheck")
        {
            if(it.value()=="Y")
            {
                ui->radioButton_sheetName_auto->setChecked(true);
            }
            else
            {
                ui->radioButton_sheetName_auto->setChecked(false);
            }
        }
        else if(it.key()=="Flash_Changes")
        {
            if(it.value()=="Y")
            {
                ui->checkBox_FlashChanges->setChecked(true);
            }
            else
            {
                ui->checkBox_FlashChanges->setChecked(false);
            }
        }
        else if(it.key()==("Write_rewrite_opt"))
        {
            if(it.value()=="Y")
            {
                ui->radioButton_option_rewrite->setChecked(true);
            }
            else
            {
                ui->radioButton_option_rewrite->setChecked(false);
            }
        }
        else if(it.key()==("Write_append_opt"))
        {
            if(it.value()=="Y")
            {
                ui->radioButton_option_append->setChecked(true);
            }
            else
            {
                ui->radioButton_option_append->setChecked(false);
            }
        }
        else if(it.key()==("Read_Whole_Table"))
        {
            if(it.value()=="Y")
            {
                ui->checkBox_readWholeTable->setChecked(true);
            }
            else
            {
                ui->checkBox_readWholeTable->setChecked(false);
            }
        }
        else if(it.key()==("R_W_Separate_Cells"))
        {
            if(it.value()=="Y")
            {
                ui->checkBox_Selected_cells_work->setChecked(true);
            }
            else
            {
                ui->checkBox_Selected_cells_work->setChecked(false);
            }
        }
        else if(it.key()=="R_W_Formatting")
        {
            if(it.value()=="Y")
            {
                ui->checkBox_writeFormating->setChecked(true);
            }
            else
            {
                ui->checkBox_writeFormating->setChecked(false);
            }
        }
        checkRadioGroup();
        setChangesFlash();
        setWriteOption();
        setReadWholeSheet();
        setSelectedCellsOptions();
    }
    return;
}

void GoogleSheetsModifier::setChangesFlash()
{
    model->setChangesToFlash(ui->checkBox_FlashChanges->isChecked());
    return;
}

void GoogleSheetsModifier::setWriteOption()
{
    if(ui->radioButton_option_append->isChecked())
    {
        communicator->setFlags(communicator->getFlags()|HTTPScommunicator::GoogleSheetsAppendMode);
    }
    else if(ui->radioButton_option_rewrite->isChecked())
    {
        communicator->setFlags(communicator->getFlags()&(~HTTPScommunicator::GoogleSheetsAppendMode));
    }
    return;
}


void GoogleSheetsModifier::keyPressEvent(QKeyEvent * event)
{
    if(event->key()==Qt::Key_Control)
    {
        model->setControlModifier(true);
    }
    return;
}
void GoogleSheetsModifier::keyReleaseEvent(QKeyEvent * event)
{
    if(event->key()==Qt::Key_Control)
    {
        model->setControlModifier(false);
    }
    return;
}

void GoogleSheetsModifier::setSelectedCellsOptions()
{
    if(ui->checkBox_Selected_cells_work->isChecked())
    {
        communicator->setFlags(communicator->getFlags()|HTTPScommunicator::w_r_SeparateCells);
    }
    else
    {
        communicator->setFlags(communicator->getFlags()&(~HTTPScommunicator::w_r_SeparateCells));
    }
    return;
}


void GoogleSheetsModifier::setReadWholeSheet()
{
    if(ui->checkBox_readWholeTable->isChecked())
    {
        communicator->setFlags(communicator->getFlags()|HTTPScommunicator::r_WholeTable);
    }
    else
    {
        communicator->setFlags(communicator->getFlags()&(~HTTPScommunicator::r_WholeTable));
    }
    return;
}


void GoogleSheetsModifier::tableView_catchContextMenuCall(const QPoint& point)
{
    QModelIndex index(ui->tableGoogleSheets->indexAt(point));
    QModelIndexList indexList(select_model->selectedIndexes());
    QMenu contextMenu(this);
    QAction* cutAct = contextMenu.addAction("Cut");
    QAction* copyAct = contextMenu.addAction("Copy");
    QAction* pasteAct = contextMenu.addAction("Paste");
    QMenu* submenu_styles = contextMenu.addMenu("Styles");
    QAction* boldFontAct = submenu_styles->addAction("Bold");
    QAction* italicFontAct = submenu_styles->addAction("Italic");
    QAction* standardFontAct = submenu_styles->addAction("Standard");
    QMenu* submenu_font = contextMenu.addMenu("Fonts");
    QAction* timesFontAct = submenu_font->addAction("Times");
    QAction* helveticaFontAct = submenu_font->addAction("Georgia");
    QAction* arialFontAct = submenu_font->addAction("Arial");
    QAction* comicsansFontAct = submenu_font->addAction("Comic Sans MS");
    QAction* defauldFontAct = submenu_font->addAction("Default");
//    QStringList fonts(QFontDatabase::families(QFontDatabase::Cyrillic));
//    foreach(const QString& font,fonts)
//    {
//        getErrMsg(font);
//    }
    //QList<QFontDatabase::WritingSystem> systems(db.writingSystems());
    QAction* selectedAction(contextMenu.exec(ui->tableGoogleSheets->viewport()->mapToGlobal(point)));
    if(selectedAction == cutAct)
    {
        model->cut(index);
    }
    else if(selectedAction ==copyAct)
    {
        model->copy(index);
    }
    else if(selectedAction == pasteAct)
    {
        model->paste(index);
    }
    else if(selectedAction == boldFontAct)
    {
        foreach(const QModelIndex& sub_index,indexList)
        {
            model->setFontWeight(sub_index,QFont::Bold);
        }
    }
    else if(selectedAction == italicFontAct)
    {
        foreach(const QModelIndex& sub_index,indexList)
        {
            model->setFontStyle(sub_index,QFont::StyleItalic);
        }
    }
    else if(selectedAction == standardFontAct)
    {
        foreach(const QModelIndex& sub_index,indexList)
        {
            model->setFontWeight(sub_index,QFont::Normal);
            model->setFontStyle(sub_index,QFont::StyleNormal);
        }
    }
    else if(selectedAction == timesFontAct)
    {
        foreach(const QModelIndex& sub_index,indexList)
        {
            model->setFont(sub_index,"Times New Roman");
        }
    }
    else if(selectedAction == helveticaFontAct)
    {
        foreach(const QModelIndex& sub_index,indexList)
        {
            model->setFont(sub_index,"Georgia");
        }
    }
    else if(selectedAction == arialFontAct)
    {
        foreach(const QModelIndex& sub_index,indexList)
        {
            model->setFont(sub_index,"Arial");
        }
    }
    else if(selectedAction == comicsansFontAct)
    {
        foreach(const QModelIndex& sub_index,indexList)
        {
            model->setFont(sub_index,"Comic Sans MS");
        }
    }
    else if(selectedAction == defauldFontAct)
    {
        foreach(const QModelIndex& sub_index,indexList)
        {
            model->setFont(sub_index,QString());
        }
    }
    else
    {

    }
    return;
}

void GoogleSheetsModifier::flashRed(QWidget * widgetToFlash)
{
    QColor redColor(247,131,131);
    QPalette palette(widgetToFlash->palette());
    palette.setBrush(QPalette::Base,redColor);
    widgetToFlash->setPalette(palette);
    flashedWidgets.enqueue(widgetToFlash);
    QTimer * timer = new QTimer(this);
    connect(timer,&QTimer::timeout,this,&GoogleSheetsModifier::stopFlashRed);
    connect(timer,&QTimer::timeout,timer,&QTimer::deleteLater);
    timer->start(1000);
    return;
}
void GoogleSheetsModifier::stopFlashRed()
{
    if(flashedWidgets.isEmpty())
    {
        return;
    }
    QWidget* widget_to_de_flash = flashedWidgets.dequeue();
    QColor whiteColor(Qt::white);
    QPalette palette(widget_to_de_flash->palette());
    palette.setBrush(QPalette::Base,whiteColor);
    widget_to_de_flash->setPalette(palette);
    return;
}
