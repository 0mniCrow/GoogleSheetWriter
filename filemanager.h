#ifndef FILEMANAGER_H
#define FILEMANAGER_H
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QTextStream>
#include <QStringList>
#define CONTROL_FILE_NAME "control_file.txt"
#define SETTINGS_NAME "settings.txt"
#define SETTINGS_SUB_DIR "Settings"
class FileManager
{
private:
    QString lastfilepath;
    bool readFile(QByteArray& data, const QString& address);
    bool writeFile(const QByteArray& data, const QString& address);

public:
    FileManager();
    bool saveJSONdataToFile(const QByteArray& data, const QString& filename);
    bool loadJSONdataFromFile(QByteArray& container, const QString& filename);
    bool savePreferences(const QByteArray& data);
    bool loadPreferences(QByteArray& container);
    bool openAPIfile(const QString& filename, QString & API_Key);
    bool openOAuthFile(const QString& filename, QString& Client_ID,QString& Client_secret);

    void setlastfilepath(const QString& filepath);
    QString getlastfilepath() const;




};

#endif // FILEMANAGER_H
