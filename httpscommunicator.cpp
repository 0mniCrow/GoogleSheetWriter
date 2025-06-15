#include "httpscommunicator.h"

HTTPScommunicator::HTTPScommunicator(QObject *tata):QObject(tata),replyHandler(8080)
{
    communicator = new QNetworkAccessManager();
    //communicator_put = new QNetworkAccessManager();
    connect(communicator,SIGNAL(finished(QNetworkReply*)),this,SLOT(replyFinishedCatch(QNetworkReply*)));
    //connect(communicator_put,SIGNAL(finished(QNetworkReply*)),this,SLOT(replyFinishedCatch(QNetworkReply*)));
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
        emit finished(reply->readAll());
    }
    else
    {
        int err = reply->error();
        QString errstr("Error from server: "+(reply->errorString()+QString::number(err)));
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
    httpflags|=OAuthAuthorized;
    emit errormsg("User is authorized;");
    return;
}

bool HTTPScommunicator::isAuthorized() const
{
    return httpflags&OAuthAuthorized;
}

void HTTPScommunicator::writeRequest(const QString& SSID, const QString& SSname,
                                     const QString &range, const QByteArray& data)
{
    if((httpflags&w_r_SeparateCells)&&(httpflags&httpflags&w_Fonts))
    {
        emit errormsg("Writing to server Error: both \"selected cells\" and \"formatting\" flags were risen;");
        return;
    }
    if(!(httpflags&oauth2Method))
    {
        emit errormsg("Writing to server Error: writing to the server can be only committed with OAuth2 method;");
        return;
    }
    if(!(httpflags&OAuthAuthorized))
    {
        emit errormsg("Writing to server Error: OAuth2 communication hasn't been authorized;");
        return;
    }
    if(!communicator)
    {
        emit errormsg("Writing to server Error: Communicator doesn't exist;");
        return;
    }
    if(SSID.isEmpty()||SSname.isEmpty()||range.isEmpty())
    {
        emit errormsg("Writing to server Error: One of address parameters is missing;");
        return;
    }
    if(data.isEmpty())
    {
        emit errormsg("Writing to server Error: Can't write: user table is empty;");
        return;
    }


    QString urlstr("https://sheets.googleapis.com/v4/spreadsheets/"+SSID);
    if(httpflags&httpflags&w_Fonts)
    {
        urlstr.append(":batchUpdate");
    }
    else if(httpflags&w_r_SeparateCells)
    {
        urlstr.append("/values:batchUpdate");
    }
    else
    {
        urlstr.append("/values/"+SSname+"!"+range);
        if(httpflags&GoogleSheetsAppendMode)
        {
            urlstr.append(":append");
        }
    }
    QUrl url(urlstr);
    if(!(httpflags&w_Fonts))
    {
        QUrlQuery query;
        query.addQueryItem("valueInputOption","USER_ENTERED");
        url.setQuery(query);
    }
    QNetworkRequest request;
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
    request.setRawHeader("Expect", "");
    request.setRawHeader("Authorization",QString("Bearer "+authorazer.token()).toUtf8());
    QNetworkReply * reply = nullptr;
    if((httpflags&GoogleSheetsAppendMode)||(httpflags&w_r_SeparateCells)||(httpflags&w_Fonts))
    {
        reply = communicator->post(request,data);
    }
    else
    {
        reply = communicator->put(request,data);
    }
    connect(reply,SIGNAL(downloadProgress(qint64,qint64)),this,SIGNAL(progress(qint64,qint64)));
    return;
}

void HTTPScommunicator::readRequest(const QString& SSID, const QString& SSname,
                const QString& range, const QString& APIkey)
{
    if(!communicator)
    {
        emit errormsg("Reading from server Error: Communicator doesn't exist;");
        return;
    }

    if(SSID.isEmpty()||SSname.isEmpty()||range.isEmpty())
    {
        emit errormsg("Reading from server Error: One of address parameters is missing;");
        return;
    }
    if(!(httpflags&oauth2Method))
    {
        if(APIkey.isEmpty())
        {
            emit errormsg("Reading from server Error: API key is required when API_key method is used;");
            return;
        }
    }
    QString urlstr("https://sheets.googleapis.com/v4/spreadsheets/"+SSID+"/values");
    QUrlQuery query;
    if(httpflags&w_r_SeparateCells)
    {
        urlstr.append(":batchGet");
        QStringList vals(range.split(',', Qt::SkipEmptyParts));
        for(QStringList::iterator it = vals.begin();it!=vals.end();it++)
        {
            query.addQueryItem("ranges",SSname+"!"+*it+":"+*it);
        }
    }
    else if(httpflags&r_WholeTable)
    {
        urlstr.append("/"+SSname+"!A1:Z1000");
    }
    else
    {
        urlstr.append("/"+SSname+"!"+range);
    }
    QUrl url(urlstr);
    QNetworkRequest request;
    if(httpflags&oauth2Method)
    {
        //query.addQueryItem("valueInputOption","USER_ENTERED");
        //url.setQuery(query);
        request.setUrl(url);
        request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
        request.setRawHeader("Expect", "");
        request.setRawHeader("Authorization",QString("Bearer "+authorazer.token()).toUtf8());
    }
    else
    {
        query.addQueryItem("key",APIkey);
        url.setQuery(query);
        request.setUrl(url);
    }
    QNetworkReply * reply = communicator->get(request);
    connect(reply,SIGNAL(downloadProgress(qint64,qint64)),this,SIGNAL(progress(qint64,qint64)));
    return;
}


void HTTPScommunicator::GetSheetIdsRequest(const QString& SSID,  const QString &APIkey)
{
    if(SSID.isEmpty())
    {
        emit errormsg("Recieving Sheet ID Error: SSID is missing;");
        return;
    }
    if(!(httpflags&oauth2Method))
    {
        if(APIkey.isEmpty())
        {
            emit errormsg("Recieving Sheet ID Error: API key is required when API_key method is used;");
            return;
        }
    }
    QString urlstr("https://sheets.googleapis.com/v4/spreadsheets/"+SSID);
    QUrl url(urlstr);
    QNetworkRequest request;
    QUrlQuery query;
    query.addQueryItem("fields","sheets(properties(title,sheetId))");
    if(httpflags&oauth2Method)
    {
        request.setUrl(url);
        request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
        request.setRawHeader("Expect", "");
        request.setRawHeader("Authorization",QString("Bearer "+authorazer.token()).toUtf8());
    }
    else
    {
        query.addQueryItem("key",APIkey);
        url.setQuery(query);
        request.setUrl(url);
    }
    QNetworkReply * reply = communicator->get(request);
    connect(reply,SIGNAL(downloadProgress(qint64,qint64)),this,SIGNAL(progress(qint64,qint64)));
    return;
}
