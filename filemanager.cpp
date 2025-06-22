#include "filemanager.h"

FileManager::FileManager()
{

}

void FileManager::setlastfilepath(const QString& filepath)
{
    lastfilepath = filepath;
    return;
}

QString FileManager::getlastfilepath() const
{
    return lastfilepath;
}

bool FileManager::saveJSONdataToFile(const QByteArray& data, const QString& filename)
{
    return writeFile(data,filename);
}

bool FileManager::loadJSONdataFromFile(QByteArray& container, const QString& filename)
{
    return readFile(container,filename);
}

bool FileManager::savePreferences(const QByteArray &data)
{
    return writeFile(data,QDir::currentPath()+'/'+
                     SETTINGS_SUB_DIR+'/'+SETTINGS_NAME);
}

bool FileManager::loadPreferences(QByteArray &container)
{
    return readFile(container,QDir::currentPath()+'/'+
                    SETTINGS_SUB_DIR+'/'+SETTINGS_NAME);
}

bool FileManager::openAPIfile(const QString& filename, QString & API_Key)
{
    QByteArray data;
    if(readFile(data,filename))
    {
        QTextStream stream(data,QIODeviceBase::ReadOnly);
        while(!stream.atEnd())
        {
            QString line(stream.readLine());
            if(line.contains("API_KEY:"))
            {
                API_Key = line.sliced(line.indexOf(':')+1);
                return true;
            }
        }
    }
    return false;
}

bool FileManager::openOAuthFile(const QString& filename,
                                QString& Client_ID,
                                QString& Client_secret)
{
    Client_ID.clear();
    Client_secret.clear();
    QByteArray data;
    if(readFile(data,filename))
    {
        QTextStream stream(data,QIODeviceBase::ReadOnly);
        while(!stream.atEnd())
        {
            QString line(stream.readLine());
            if(line.contains("Client ID:"))
            {
                Client_ID = line.sliced(line.indexOf(':')+1);
            }
            else if(line.contains("Client secret:"))
            {
                Client_secret = line.sliced(line.indexOf(':')+1);
            }
        }
        if((!Client_ID.isNull())&&(!Client_secret.isNull()))
        {
            return true;
        }
    }
    return false;
}

bool FileManager::readFile(QByteArray& data, const QString& address)
{
    QString working_addr;
    if(address.isEmpty())
    {
        return false;
    }
    else if(!address.contains('/'))
    {
        if(lastfilepath.isEmpty())
        {
            working_addr=QDir::currentPath()+"/"+address;
        }
        else
        {
            working_addr=lastfilepath+"/"+address;
        }
    }
    else
    {
        working_addr = address;
    }
    QFileInfo file_info(working_addr);
    if(!file_info.exists())
    {
        return false;
    }
    lastfilepath = file_info.absolutePath();
    QFile file(file_info.absoluteFilePath());
    if(!file.open(QIODevice::ReadOnly))
    {
        return false;
    }
    data = file.readAll();
    file.close();
    return true;
}
bool FileManager::writeFile(const QByteArray& data, const QString& address)
{
    QString working_addr;
    bool answer = true;
    if(address.isEmpty())
    {
        working_addr= QDir::currentPath()+"/" +CONTROL_FILE_NAME;
        answer = false;
    }
    else if(!address.contains('/'))
    {
        if(lastfilepath.isEmpty())
        {
            working_addr=QDir::currentPath()+"/"+address;
        }
        else
        {
            working_addr=lastfilepath+"/"+address;
        }
    }
    else
    {
        working_addr = address;
    }
    QFileInfo file_info(working_addr);
    lastfilepath = file_info.absolutePath();
    QDir dir(lastfilepath);
    dir.mkdir(lastfilepath+'/');
    QFile file(file_info.absoluteFilePath());
    if(!file.open(QIODevice::WriteOnly))
    {
        return false;
    }
    file.write(data);
    file.close();
    return answer;
}
