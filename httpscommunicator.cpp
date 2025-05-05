#include "httpscommunicator.h"

HTTPScommunicator::HTTPScommunicator(QObject *tata):QObject(tata),replyHandler(8080)
{
    communicator = new QNetworkAccessManager();
    communicator_put = new QNetworkAccessManager();
    connect(communicator,SIGNAL(finished(QNetworkReply*)),this,SLOT(replyFinishedCatch(QNetworkReply*)));
    connect(communicator_put,SIGNAL(finished(QNetworkReply*)),this,SLOT(replyFinishedCatch(QNetworkReply*)));
    connect(&authorazer,&QOAuth2AuthorizationCodeFlow::granted,this,&HTTPScommunicator::authapprove);
    connect(&authorazer,SIGNAL(error(QString,QString,QUrl)),this,SLOT(errcatch(QString,QString,QUrl)));
    authorized = true;
    httpflags = noFlags;
    return;
}

HTTPScommunicator::~HTTPScommunicator()
{
    disconnect(communicator,SIGNAL(finished(QNetworkReply*)),this,SLOT(replyFinishedCatch(QNetworkReply*)));

    delete communicator;
    delete communicator_put;

    return;
}

void HTTPScommunicator::setFlags(unsigned char flags)
{
    httpflags = flags;
    return;
}

unsigned char HTTPScommunicator::getFlags() const
{
    return httpflags;
}

void HTTPScommunicator::errcatch(const QString& error, const QString& errorDescription, const QUrl& uri)
{
    QString err("OAuth error: "+error+"; description: "+errorDescription+"; uri: "+uri.toString());
    emit errormsg(err);
    return;
}

void HTTPScommunicator::replyFinishedCatch(QNetworkReply* reply)
{
    if(reply->error()==QNetworkReply::NoError)
    {
        if((httpflags&oauth2Method)&&(httpflags&OAuthWriteMode))
        {

        }
        else
        {
            emit finished(reply->readAll());
        }
    }
    else
    {
        int err = reply->error();
        QString errstr(reply->errorString()+QString::number(err));
        QVariant redirectAttr(reply->attribute(QNetworkRequest::RedirectionTargetAttribute));
        if(redirectAttr.isValid())
        {
            errstr.append(redirectAttr.toUrl().toString());
        }
        QVariant Location(reply->header(QNetworkRequest::LocationHeader));
        errstr.append(" "+Location.toUrl().toString());
        emit errormsg(errstr);
    }
    disconnect(reply,SIGNAL(downloadProgress(qint64,qint64)),this,SIGNAL(progress(qint64,qint64)));
    reply->deleteLater();
    return;
}

void HTTPScommunicator::AuthorizeRequest(const QString& Client_ID,const QString& Client_Secret)
{
    authorazer.setAuthorizationUrl(QUrl("https://accounts.google.com/o/oauth2/auth"));
    authorazer.setAccessTokenUrl(QUrl("https://oauth2.googleapis.com/token"));
    authorazer.setClientIdentifier(Client_ID);
    authorazer.setClientIdentifierSharedKey(Client_Secret);
    authorazer.setReplyHandler(&replyHandler);
    authorazer.setScope("https://www.googleapis.com/auth/spreadsheets");
    QObject::connect(&authorazer,&QOAuth2AuthorizationCodeFlow::authorizeWithBrowser,&QDesktopServices::openUrl);
    authorazer.grant();
    QString token(authorazer.token());
    emit errormsg(token);
    return;
}

void HTTPScommunicator::authapprove()
{
    authorized = true;
    emit errormsg("User is authorized;");
    return;
}

bool HTTPScommunicator::isAuthorized() const
{
    return authorized;
}

void HTTPScommunicator::writeRequest(const QString& SSID, const QString& SSname,
                                     const QString &range, const QByteArray& data)
{
    if(!communicator_put)
    {
        emit errormsg("Communication object doesn't exist;");
        return;
    }

    if(SSID.isEmpty()||SSname.isEmpty()||range.isEmpty())
    {
        emit errormsg("One of parameters missing;");
        return;
    }
    if(data.isEmpty())
    {
        emit errormsg("Nothing is in the request;");
        return;
    }


    QString urlstr("https://sheets.googleapis.com/v4/spreadsheets/"+SSID+"/values/"+SSname+"!"+range/*+":append"*/);
    if(httpflags&GoogleSheetsAppendMode)
    {
        urlstr.append(":append");
    }
    QUrlQuery query;
    query.addQueryItem("valueInputOption","USER_ENTERED");
    //query.addQueryItem("key",Client_ID);
    QUrl url(urlstr);
    url.setQuery(query);
    QNetworkRequest request;
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
    request.setRawHeader("Expect", "");
    request.setRawHeader("Authorization",QString("Bearer "+authorazer.token()).toUtf8());
    QNetworkReply * reply = nullptr;
    if(httpflags&GoogleSheetsAppendMode)
    {
        reply = communicator_put->post(request,data);
    }
    else
    {
        reply = communicator_put->put(request,data);
    }
    connect(reply,SIGNAL(downloadProgress(qint64,qint64)),this,SIGNAL(progress(qint64,qint64)));
    return;
}

void HTTPScommunicator::readRquest(const QString& SSID, const QString& SSname, const QString& APIkey,
                const QString& range)
{
    if(!communicator)
    {
        emit errormsg("Communication object doesn't exist;");
        return;
    }

    if(SSID.isEmpty()||SSname.isEmpty()||APIkey.isEmpty()||range.isEmpty())
    {
        emit errormsg("One of parameters missing;");
        return;
    }
    QString urlstr("https://sheets.googleapis.com/v4/spreadsheets/"+SSID+"/values/"+SSname+"!"+range);
    QNetworkReply * reply = nullptr;
    if(httpflags&oauth2Method)
    {

    }
    else
    {
        QUrlQuery query;
        query.addQueryItem("key",APIkey);
        QUrl url(urlstr);
        url.setQuery(query);
        QNetworkRequest request;
        request.setUrl(url);
        reply = communicator->get(request);
    }
    connect(reply,SIGNAL(downloadProgress(qint64,qint64)),this,SIGNAL(progress(qint64,qint64)));
    return;
}
