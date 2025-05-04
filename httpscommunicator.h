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
    QNetworkAccessManager* communicator_put;
    QOAuth2AuthorizationCodeFlow authorazer;
    QOAuthHttpServerReplyHandler replyHandler;
    bool authorized;
public:
    HTTPScommunicator(QObject * tata = nullptr);
    ~HTTPScommunicator();
    bool isAuthorized() const;
private slots:
    void replyFinishedCatch(QNetworkReply* reply);
    void authapprove();
    void errcatch(const QString& error, const QString& errorDescription, const QUrl& uri);

public slots:
    void writeRequest(const QString& SSID, const QString& SSname,
                      const QString& range, const QByteArray& data);
    void readRquest(const QString& SSID, const QString& SSname, const QString& APIkey,
                    const QString& range);
    void AuthorizeRequest(const QString& Client_ID,const QString& Client_Secret);
signals:
    void finished(const QByteArray& answer);
    void progress(qint64 recieved, qint64 total);
    void errormsg(const QString& err);

};

#endif // HTTPSCOMMUNICATOR_H
