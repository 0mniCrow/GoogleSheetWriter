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
    communicator = new HTTPScommunicator();
    radButGroup.addButton(ui->radioButtonAPI_key);
    radButGroup.addButton(ui->radioButton_OAuth2);
    checkRadioGroup();
    connect(ui->ButtonAddColumn,SIGNAL(clicked(bool)),this,SLOT(addColumn()));
    connect(ui->ButtonAddRow,SIGNAL(clicked(bool)),this,SLOT(addRow()));
    connect(ui->ButtonRemoveColumn,SIGNAL(clicked(bool)),this,SLOT(removeColumn()));
    connect(ui->ButtonRemoveRow,SIGNAL(clicked(bool)),this,SLOT(removeRow()));
    connect(ui->Button_API_key_file_choose,SIGNAL(clicked(bool)),this,SLOT(chooseAPICredentialFile()));
    connect(ui->Button_API_key_file_load,SIGNAL(clicked(bool)),this,SLOT(loadAPICredentialFile()));
    connect(ui->Button_OAuth_choosefile,SIGNAL(clicked(bool)),this,SLOT(chooseOAuthCredentialFile()));
    connect(ui->Button_OAuth_LoadFile,SIGNAL(clicked(bool)),this,SLOT(loadOAuthCredentialFile()));
    connect(ui->ButtonRead,SIGNAL(clicked(bool)),this,SLOT(read()));
    connect(ui->ButtonWrite,SIGNAL(clicked(bool)),this,SLOT(write()));
    connect(communicator,SIGNAL(progress(qint64,qint64)),this,SLOT(setProgressBar(qint64,qint64)));
    connect(communicator,SIGNAL(errormsg(QString)),this,SLOT(getErrMsg(QString)));
    connect(communicator,SIGNAL(finished(QByteArray)),this,SLOT(getFinishedSignal(QByteArray)));
    connect(ui->ButtonAuth,SIGNAL(clicked(bool)),this,SLOT(authificate()));
    connect(ui->ButtonSave,SIGNAL(clicked(bool)),this,SLOT(save()));
    connect(ui->ButtonLoad,SIGNAL(clicked(bool)),this,SLOT(load()));
    connect(ui->radioButtonAPI_key,SIGNAL(clicked(bool)),this,SLOT(checkRadioGroup()));
    connect(ui->radioButton_OAuth2,SIGNAL(clicked(bool)),this,SLOT(checkRadioGroup()));
    readMode=false;
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
}
void GoogleSheetsModifier::setProgressBar(qint64 val,qint64 total)
{
    ui->progressBar->setValue(val*100/total);
}

void GoogleSheetsModifier::getErrMsg(const QString &errMsg)
{
    ui->textEdit->append(errMsg);
}

void GoogleSheetsModifier::authificate()
{
    if((!ui->lineClientID->text().isEmpty())&&(!ui->lineClientSecret->text().isEmpty()))
    {
        communicator->AuthorizeRequest(ui->lineClientID->text(),ui->lineClientSecret->text());
        return;
    }
    ui->InfoLabel->setText("authification fields are empty;");
    return;
}

void GoogleSheetsModifier::addRow()
{
    QModelIndexList indexList(select_model->selectedIndexes());
    if(indexList.isEmpty())
    {
        model->insertRows(model->rowCount(QModelIndex())-1,1,QModelIndex());
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

void GoogleSheetsModifier::addColumn()
{
    QModelIndexList indexList(select_model->selectedIndexes());
    if(indexList.isEmpty())
    {
        model->insertColumns(model->columnCount(QModelIndex())-1,1,QModelIndex());
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

void GoogleSheetsModifier::removeRow()
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

void GoogleSheetsModifier::removeColumn()
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

void GoogleSheetsModifier::chooseAPICredentialFile()
{
    QString filename(QFileDialog::getOpenFileName(
                         this,"Open API key file",QDir::currentPath(),
                         "Text file (*.txt)",nullptr,QFileDialog::DontUseNativeDialog));
    ui->lineAPI_key_filename->setText(filename);
    loadAPICredentialFile();
    return;
}

void GoogleSheetsModifier::loadAPICredentialFile()
{
    if(!QFile::exists(ui->lineAPI_key_filename->text()))
    {
        getErrMsg("Address ["+ui->lineAPI_key_filename->text()+"] doesn't exist;");
        return;
    }
    QFile apiFile(ui->lineAPI_key_filename->text());
    if(!apiFile.open(QIODevice::ReadOnly))
    {
        getErrMsg("Can't open ["+ui->lineAPI_key_filename->text()+"] file;");
        return;
    }
    ui->line_API_Key->setText(QString(apiFile.readAll()));
    apiFile.close();
    return;
}

void GoogleSheetsModifier::chooseOAuthCredentialFile()
{
    QString filename(QFileDialog::getOpenFileName(
                         this,"Open OAuth2 credential file",QDir::currentPath(),
                         "Text file (*.txt)",nullptr,QFileDialog::DontUseNativeDialog));
    ui->lineEdit_OAuth_filename->setText(filename);
    loadOAuthCredentialFile();
    return;
}
void GoogleSheetsModifier::loadOAuthCredentialFile()
{
    if(!QFile::exists(ui->lineEdit_OAuth_filename->text()))
    {
        getErrMsg("Address ["+ui->lineAPI_key_filename->text()+"] doesn't exist;");
        return;
    }
    QFile oauthFile(ui->lineAPI_key_filename->text());
    if(!oauthFile.open(QIODevice::ReadOnly))
    {
        getErrMsg("Can't open ["+ui->lineAPI_key_filename->text()+"] file;");
        return;
    }
    QTextStream out(&oauthFile);

    while(!out.atEnd())
    {
        QString credentials(out.readLine());
        if(credentials.contains("Client ID:"))
        {
            ui->lineClientID->setText(credentials.sliced(credentials.indexOf(':')+1));
        }
        else if(credentials.contains("Client secret:"))
        {
            ui->lineClientSecret->setText(credentials.sliced(credentials.indexOf(':')+1));
        }
    }
    oauthFile.close();
    return;
}


bool GoogleSheetsModifier::checkFields()
{
    return (!ui->line_API_Key->text().isEmpty())&&
            (!ui->lineSheetName->text().isEmpty())&&
            (!ui->lineSpreadSheetID->text().isEmpty());
}

void GoogleSheetsModifier::write()
{
    if(!checkFields())
    {
        ui->InfoLabel->setText("Nessesary fields are empty;");
        return;
    }
    if(!(communicator->isAuthorized()))
    {
        ui->InfoLabel->setText("User wasn't authorized;");
        return;
    }
    QVector<QVector<QVariant>> container;
    QByteArray jsonData;
    model->downloadDataFromModel(container);
    if(!parser.parseDataToJSON(container,ui->lineSheetName->text(),jsonData))
    {
        ui->InfoLabel->setText(parser.getLastError());
        return;
    }
    char va = 'A'+container.at(0).size()-1;

    QString range1(QString("A1:")+QChar(va)+QString::number(container.size()));
    //QString range("R1C1:R"+QString::number(container.size())+"C"+QString::number(container.at(0).size()));
    communicator->writeRequest(ui->lineSpreadSheetID->text(),
                               ui->lineSheetName->text(),
                               range1,jsonData);
    return;
}

void GoogleSheetsModifier::read()
{
    if(!checkFields())
    {
        ui->InfoLabel->setText("Nessesary fields are empty;");
        return;
    }
    int rows = model->rowCount(QModelIndex());
    int columns = model->columnCount(QModelIndex());
    if((!rows)&&(!columns))
    {
        ui->InfoLabel->setText("Range is empty;");
        return;
    }
    QString range("R1C1:R"+QString::number(rows)+"C"+QString::number(columns));
    readMode=true;
    communicator->readRquest(ui->lineSpreadSheetID->text()
                             ,ui->lineSheetName->text(),
                             ui->line_API_Key->text(),range);
    return;
}

void GoogleSheetsModifier::save()
{
    QString filename(QFileDialog::getOpenFileName(
                         this,"Save JSON file",QDir::currentPath(),
                         "JSON (*.json)",nullptr,QFileDialog::DontUseNativeDialog));
    QVector<QVector<QVariant>> rawData;
    model->downloadDataFromModel(rawData);
    QByteArray JSONdata;
    if(!parser.parseDataToJSON(rawData,QString(),JSONdata))
    {
        getErrMsg(parser.getLastError());
        return;
    }
    if(!parser.saveJsonToFile(JSONdata,filename))
    {
        getErrMsg(parser.getLastError());
        return;
    }
    return;
}
void GoogleSheetsModifier::load()
{

    QString filename(QFileDialog::getOpenFileName(
                         this,"Load JSON file",QDir::currentPath(),
                         "JSON (*.json)",nullptr,QFileDialog::DontUseNativeDialog));
    QByteArray JSONdata;
    QVector<QVector<QVariant>> rawData;
    if(!parser.loadJsonFromFile(JSONdata,filename))
    {
        getErrMsg(parser.getLastError());
        return;
    }
    if(!parser.parseJSONToData(JSONdata,rawData))
    {
        getErrMsg(parser.getLastError());
        return;
    }
    model->loadDataToModel(rawData);
    return;
}

void GoogleSheetsModifier::getFinishedSignal(const QByteArray& data)
{
    if(!readMode)
    {
        return;
    }
    QVector<QVector<QVariant>> modelData;
    if(!parser.parseJSONToData(data,modelData))
    {
        ui->InfoLabel->setText(parser.getLastError());
        return;
    }
    model->loadDataToModel(modelData);
    return;
}

void GoogleSheetsModifier::checkRadioGroup()
{
    if(ui->radioButtonAPI_key->isChecked())
    {
        ui->groupBox_API_KEY->setEnabled(true);
        ui->groupBox_OAuth2->setEnabled(false);
        communicator->setFlags(communicator->getFlags()&(~HTTPScommunicator::oauth2Method));
    }
    else if(ui->radioButton_OAuth2->isChecked())
    {
        ui->groupBox_OAuth2->setEnabled(true);
        ui->groupBox_API_KEY->setEnabled(false);
        communicator->setFlags(communicator->getFlags()|HTTPScommunicator::oauth2Method);
    }
}
