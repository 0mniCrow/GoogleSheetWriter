#include "xmlparser.h"

XMLParser::XMLParser()
{

}
bool XMLParser::saveData(QByteArray& raw_data, const QMap<QString,QString>& data)
{
    QDomDocument Settings("savefile");
    QDomElement koranj(Settings.createElement("Settings"));
    Settings.appendChild(koranj);
    for(QMap<QString,QString>::const_iterator it = data.constBegin();it!=data.constEnd();it++)
    {
        QDomElement tag(Settings.createElement(it.key()));
        koranj.appendChild(tag);
        QDomText text(Settings.createTextNode(it.value()));
        tag.appendChild(text);
    }
    raw_data = Settings.toByteArray();
    return true;
}
bool XMLParser::loadData(const QByteArray &raw_data, QMap<QString,QString>& container)
{
    QDomDocument Settings("savefile");
    QString errormsg;
    int errLine;
    int errCol;
    if(!Settings.setContent(raw_data,&errormsg,&errLine,&errCol))
    {
        QString last_err = errormsg;
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
