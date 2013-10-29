/*
 ****************************************************************************
 * filename
 *
 * Created on: 2013-10-28
 *     Author: 贺辉
 *    License: LGPL
 *    Comment:
 *
 *
 *    =============================  Usage  =============================
 *|
 *|
 *    ===================================================================
 *
 *
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 ****************************************************************************
 */

/*
 ***************************************************************************
 * Last Modified on: 2013-10-28
 * Last Modified by: 贺辉
 ***************************************************************************
 */





#ifndef CONTACTBOX_H
#define CONTACTBOX_H

#include <QTreeWidget>
#include <QContextMenuEvent>




#include "contactwidget.h"
#include "../contactsmanager/contact.h"
#include "../contactsmanager/contactsmanager.h"
#include "../informationtips/userinfotipwindow.h"
#include "../packetmanager/imclientpacketsparser.h"


namespace HEHUI {



class ContactBox : public QTreeWidget
{
    Q_OBJECT
public:
    explicit ContactBox(QWidget *parent = 0);
    ~ContactBox();

    void loadAllContacts();

    void addOrRemoveContactItem(Contact *contact, bool add);
    void updateContactItemInfo(Contact *contact);

    void addOrRemoveContactGroupItem(ContactGroupBase *contactGroup, bool add);
    void updateContactGroupItemInfo(ContactGroupBase *contactGroup);

protected:
    void contextMenuEvent(QContextMenuEvent *e);
    void resizeEvent(QResizeEvent *e);


signals:
    void signalRenameContactGroup(quint32 groupID, const QString &newGroupName);
    void signalCreateOrDeleteContactGroup(quint32 groupID, const QString &newGroupName, bool create);

public slots:



private slots:
    void handleMousePress(QTreeWidgetItem* item);
    void handleMouseDoubleClick(QTreeWidgetItem* item);

    void handleContextMenuEventOnContactGroup(QTreeWidgetItem* item, const QPoint &global_mouse_pos);
    void handleContextMenuEventOnContact(QTreeWidgetItem* item, const QPoint &global_mouse_pos);
    void handleTooltipEventOnContact(QTreeWidgetItem* item, const QPoint &global_item_topLeft_pos, const QPoint &global_mouse_pos);



private:

    ContactsManager *m_contactsManager;
    IMUser *m_myself;

    UserInfoTipWindow *m_userInfoTipWindow;



    QHash<Contact*, QTreeWidgetItem*> contactsHash;
    QHash<ContactGroupBase*, QTreeWidgetItem*> contactGroupsHash;



};

} //namespace HEHUI

#endif // CONTACTBOX_H