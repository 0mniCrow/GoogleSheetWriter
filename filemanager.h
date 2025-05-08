#ifndef FILEMANAGER_H
#define FILEMANAGER_H
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QStringList>

class FileManager
{
private:

public:
    FileManager();
    bool saveJSONdataToFile(const QByteArray& data, const QString& filename);
    bool loadJSONdataFromFile(QByteArray& container, const QString& filename);
    bool savePreferences(const QStringList& data);
    bool loadPreferences(QStringList& container);
    bool openAPIfile(const QString& filename, QString & container);
    bool openOAuthFile(const QString& filename, QStringList& container);
};

#endif // FILEMANAGER_H
