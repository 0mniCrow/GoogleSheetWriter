#ifndef FILEMANAGER_H
#define FILEMANAGER_H
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QStringList>

class FileManager
{
private:
    QString lastfilepath;
public:
    FileManager();
    bool saveJSONdataToFile(const QByteArray& data, const QString& filename);
    bool loadJSONdataFromFile(QByteArray& container, const QString& filename);
    bool savePreferences(const QByteArray& data);
    bool loadPreferences(QByteArray& container);
    bool openAPIfile(const QString& filename, QString & container);
    bool openOAuthFile(const QString& filename, QStringList& container);
    void setlastfilepath(const QString& filepath);
    QString getlastfilepath() const;
};

#endif // FILEMANAGER_H
