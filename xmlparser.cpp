#include "xmlparser.h"

void ParseXML_dataToXML(QByteArray& container, const QMap<QString,QString>& data)
{
    QDomDocument Settings("GoogleSheets_Settings");
    QDomElement root(Settings.createElement("Settings"));
    Settings.appendChild(root);
    for(QMap<QString,QString>::const_iterator it = data.constBegin();it!=data.constEnd();it++)
    {
        QDomElement tag(Settings.createElement(it.key()));
        root.appendChild(tag);
        QDomText text(Settings.createTextNode(it.value()));
        tag.appendChild(text);
    }
    container = Settings.toByteArray();
    return;
}
bool ParseXML_XMLToData(const QByteArray& raw_data, QMap<QString,QString>& container, QString* errmsg)
{
    QDomDocument Settings("GoogleSheets_Settings");
    QString err_msg;
    int err_Line;
    int err_Col;
    if(!Settings.setContent(raw_data,&err_msg,&err_Line,&err_Col))
    {
        if(errmsg)
        {
            *errmsg = "Error at ln["+QString::number(err_Line)+"], col["+QString::number(err_Col)+"]: "+err_msg;
        }
        return false;
    }
    container.clear();
    QDomElement docElem(Settings.documentElement());
    QDomNode n = docElem.firstChild();
    while(!n.isNull())
    {
        QDomElement e = n.toElement();
        container.insert(e.tagName(),e.text());
        n = n.nextSibling();
    }
    return true;
}
