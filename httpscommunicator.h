#ifndef HTTPSCOMMUNICATOR_H
#define HTTPSCOMMUNICATOR_H

#include <QObject>
#include <QByteArray>
#include <QNetworkAccessManager>
#include <QDesktopServices>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QOAuth2AuthorizationCodeFlow>
#include <QOAuthHttpServerReplyHandler>
#include <QUrl>
#include <QUrlQuery>
#include <QDesktopServices>


class HTTPScommunicator:public QObject
{
    Q_OBJECT
private:

    QNetworkAccessManager * communicator;
    //QNetworkAccessManager* communicator_put;
    QOAuth2AuthorizationCodeFlow authorazer;
    QOAuthHttpServerReplyHandler replyHandler;
    unsigned char httpflags;
    bool authorized;
public:
    enum StatusFlags{noFlags = 0x00, oauth2Method = 0x01 /*перастасаванне паміж API_key(0) і OAuth2(1)*/,
                    OAuthAuthorized = 0x02,OAuthWriteMode = 0x04 /*сцяг запісу каб не разбіраць адказ сервера*/,
                    GoogleSheetsAppendMode = 0x08/*падняты зьмяняе запыт з "put" на "post" з дадаткам ":append"*/,
                    w_r_SeparateCells=0x10/*падняты зьмяняе форму запыта з "адзінага" на "шматлікі"*/,
                    r_WholeTable=0x20,
                    w_Fonts = 0x40};
    HTTPScommunicator(QObject * tata = nullptr);
    ~HTTPScommunicator();
    bool isAuthorized() const;
    unsigned char getFlags() const;
private slots:
    void replyFinishedCatch(QNetworkReply* reply);
    void authapprove();
    void errcatch(const QString& error, const QString& errorDescription, const QUrl& uri);
public slots:
    void writeRequest(const QString& SSID, const QString& SSname,
                      const QString& range, const QByteArray& data);
    void readRequest(const QString& SSID, const QString& SSname,
                    const QString& range, const QString& APIkey = QString());
    void AuthorizeRequest(const QString& Client_ID,const QString& Client_Secret);
    void GetSheetIdsRequest(const QString& SSID, const QString& APIkey = QString());
    void setFlags(unsigned char flags);
signals:
    void finished(const QByteArray& answer);
    void progress(qint64 recieved, qint64 total);
    void errormsg(const QString& err);

};

#endif // HTTPSCOMMUNICATOR_H
