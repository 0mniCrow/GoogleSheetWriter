#include "filemanager.h"

FileManager::FileManager()
{

}

bool FileManager::saveJSONdataToFile(const QByteArray& data, const QString& filename)
{
    QFile file(filename);
    if(!file.open(QIODevice::WriteOnly))
    {
        return false;
    }
    file.write(data);
    file.close();
    return true;
}
bool FileManager::loadJSONdataFromFile(QByteArray& container, const QString& filename)
{
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly))
    {
        return false;
    }
    container = file.readAll();
    file.close();
    return true;
}
bool FileManager::savePreferences(const QStringList& data)
{
    QDir dir(QDir::currentPath());
    QString curpath(QDir::currentPath()+"/Settings/");
    dir.mkdir(curpath);
    QFile file(curpath+"settings.txt");
    if(!file.open(QIODevice::WriteOnly|QIODevice::Text))
    {
        return false;
    }
    QTextStream stream(&file);
    for(const QString& str:data)
    {
        stream<<str<<"\n";
    }
    file.close();
    return true;
}

bool FileManager::loadPreferences(QStringList& container)
{
    QFile file(QDir::currentPath()+"/Settings/settings.txt");
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        return false;
    }
    QTextStream stream(&file);
    container.clear();
    while(!stream.atEnd())
    {
        container.append(stream.readLine());
    }
    file.close();
    return true;
}

bool FileManager::openAPIfile(const QString& filename, QString & container)
{
    if(!QFile::exists(filename))
    {
        return false;
    }
    QFile apiFile(filename);
    if(!apiFile.open(QIODevice::ReadOnly))
    {
        return false;
    }
    container = apiFile.readAll();
    apiFile.close();
    return true;
}

bool FileManager::openOAuthFile(const QString& filename, QStringList& container)
{
    if(!QFile::exists(filename))
    {
        return false;
    }
    QFile oauthFile(filename);
    if(!oauthFile.open(QIODevice::ReadOnly))
    {
        return false;
    }
    QTextStream out(&oauthFile);

    while(!out.atEnd())
    {
        container.append(out.readLine());
    }
    oauthFile.close();
    return true;
}
