#ifndef ADUSER_H
#define ADUSER_H

#include <QObject>
#include <QHash>
#include <QStringList>



namespace HEHUI {


class ADUser : public QObject
{
    Q_OBJECT
public:
    explicit ADUser(QObject *parent = 0);

    static QString CommonAttributeName(const QString &attributeName);

    static void setOUList(const QStringList &ouList);
    static QStringList getOUList();

    static void setADDefaultNamingContext(const QString &adDefaultNamingContext);
    static QString getADDefaultNamingContext();


    void setAttribute(const QString &attributeName, const QString &attributeValue);
    QString getAttribute(const QString &attributeName);





signals:


public slots:


private:
    static QHash<QString/*AD Attribute Name*/, QString/*Common Attribute Name*/> *commonAttributeNameHash;
    static QStringList m_ouList;
    static QString m_ADDefaultNamingContext;

    QHash<QString/*AD Attribute Name*/, QString/*Attribute Value*/> attributeHash;
    



};

} //namespace HEHUI

#endif // ADUSER_H
