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


#include <QMessageBox>


#include "contactbox.h"

#include "sheet_delegate_p.h"


namespace HEHUI{

ContactBox::ContactBox(QWidget *parent) :
    QTreeWidget(parent)
{

    //setFocusPolicy(Qt::NoFocus);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setIndentation(0);
    setRootIsDecorated(false);
    setColumnCount(1);
    header()->hide();
    header()->setSectionResizeMode(QHeaderView::Stretch);
    setTextElideMode(Qt::ElideMiddle);
    setVerticalScrollMode(ScrollPerPixel);

    setItemDelegate(new SheetDelegate(this, this));


    connect(this, SIGNAL(itemPressed(QTreeWidgetItem*,int)), this, SLOT(handleMousePress(QTreeWidgetItem*)));
    connect(this, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT(handleMouseDoubleClick(QTreeWidgetItem*)));

    connect(this, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)), this, SLOT(handleCurrentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)));


    m_contactsManager = ContactsManager::instance();
    m_myself = IMUser::instance();


    m_userInfoTipWindow = new UserInfoTipWindow(this);
    //connect(m_userInfoTipWindow, SIGNAL(showUserInfoRequested(IMUserBase*)), this, SLOT(showUserInfo(IMUserBase*)));



//    setStyleSheet("QTreeView::item:hover {background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #e7effd, stop: 1 #cbdaf1); border: 1px solid #bfcde4;}" );


}

ContactBox::~ContactBox(){

    m_userInfoTipWindow->hide();
    delete m_userInfoTipWindow;

}

void ContactBox::loadAllContacts(){

    QList<ContactGroupBase *> groups = m_myself->getContactGroups(false, false);

    QHash<QString/*Contact ID*/, Contact*> users = m_contactsManager->getAllUsers();


    foreach (ContactGroupBase *contactGroup, groups) {
        addOrRemoveContactGroupItem(contactGroup, true);

        QStringList members = contactGroup->members();
        foreach (QString contactID, members) {
            Contact *contact = users.take(contactID);
            Q_ASSERT(contact);
            if(!contact){
                continue;
            }

            addOrRemoveContactItem(contact, true);
        }

    }


}

void ContactBox::addOrRemoveContactItem(Contact *contact, bool add){

    QTreeWidgetItem *parentItem = contactGroupsHash.value(m_myself->getContactGroup(contact->getContactGroupID()));
    if(!parentItem){return;}

    QTreeWidgetItem *item = contactsHash.value(contact);


    if(add){
        if(item){return;}

        item = new QTreeWidgetItem(parentItem);
        ContactWidget *wgt = new ContactWidget(contact, this);
        setItemWidget(item, 0, wgt);

        contactsHash.insert(contact, item);
    }else{
        if(!item){return;}

        contactsHash.remove(contact);

        ContactWidget *wgt = qobject_cast<ContactWidget *>( itemWidget(item, 0) );
        if(wgt){
            delete wgt;
        }

        parentItem->removeChild(item);
        delete item;
        item = 0;

    }


}

void ContactBox::updateContactItemInfo(Contact *contact){

    QTreeWidgetItem *item = contactsHash.value(contact);
    if(!item){return;}

    ContactWidget *wgt = qobject_cast<ContactWidget *>( itemWidget(item, 0) );
    if(!wgt){return;}

    wgt->updateContactToUI();


}

void ContactBox::addOrRemoveContactGroupItem(ContactGroupBase *contactGroup, bool add){

    if(!contactGroup){return;}

    //    quint32 groupID = contactGroup->getGroupID();


    QTreeWidgetItem *item = contactGroupsHash.value(contactGroup);
    if(add){
        if(item){return;}

        item = new QTreeWidgetItem();
        item->setText(0, contactGroup->getGroupName());
        addTopLevelItem(item);

        contactGroupsHash.insert(contactGroup, item);

    }else{

        contactGroupsHash.remove(contactGroup);

        takeTopLevelItem(indexOfTopLevelItem((item)));

        delete item;
        item = 0;

    }


}

void ContactBox::updateContactGroupItemInfo(ContactGroupBase *contactGroup){

    QTreeWidgetItem *item = contactGroupsHash.value(contactGroup);
    if(!item){
        return;
    }

    //TODO
    item->setText(0, contactGroup->getGroupName());

}

void ContactBox::moveContact(Contact *contact, ContactGroupBase *oldContactGroup, ContactGroupBase *newContactGroup){

    QTreeWidgetItem *contactItem = contactsHash.value(contact);
    if(!contactItem){return;}

    QTreeWidgetItem *oldContactGroupItem = contactGroupsHash.value(oldContactGroup);
    QTreeWidgetItem *newContactGroupItem = contactGroupsHash.value(newContactGroup);
    if(!oldContactGroupItem || !newContactGroupItem){return;}
    if(oldContactGroupItem == newContactGroupItem){return;}

    oldContactGroupItem->removeChild(contactItem);
    newContactGroupItem->addChild(contactItem);

}

void ContactBox::setContactGroupItemExpanded(ContactGroupBase *contactGroup, bool expanded){

    QTreeWidgetItem *groupItem = contactGroupsHash.value(contactGroup);
    if(!groupItem){return;}

    setItemExpanded(groupItem, expanded);

}

void ContactBox::setContactGroupItemHidden(ContactGroupBase *contactGroup, bool hide){

    QTreeWidgetItem *groupItem = contactGroupsHash.value(contactGroup);
    if(!groupItem){return;}

    setItemHidden(groupItem, hide);

}

void ContactBox::setContactItemHidden(Contact *contact, bool hide){

    QTreeWidgetItem *item = contactsHash.value(contact);
    if(!item){return;}

    setItemHidden(item, hide);

}

bool ContactBox::event(QEvent *event){

    switch(event->type()){

    case QEvent::ContextMenu:
    {
        QContextMenuEvent *e = static_cast<QContextMenuEvent *> (event);
        if(!e){return false;}

        QTreeWidgetItem *item = itemAt(e->pos());
        if(!item){return false;}

        if(item->parent() == 0){
            handleContextMenuEventOnContactGroup(item, mapToGlobal(e->pos()));
        }else{
            handleContextMenuEventOnContact(item, mapToGlobal(e->pos()));
        }

        return true;
    }
        break;
    case QEvent::ToolTip:
    {
        QHelpEvent *helpEvent = static_cast<QHelpEvent *>(event);
        if(!helpEvent){return false;}

        QTreeWidgetItem *item = itemAt(helpEvent->pos());
        if(!item){return false;}
        if(item->parent() == 0){return false;}
        Contact *contact = contactsHash.key(item);
        if(!contact){return false;}

        ContactWidget *wgt = qobject_cast<ContactWidget *>( itemWidget(item, 0) );
        if(!wgt){return false;}
        if(!wgt->isMouseUnderFace()){return false;}

        QPoint globalvisualRectTopLeft = mapToGlobal(wgt->frameGeometry().topLeft());
        QPoint globalMousePos = helpEvent->globalPos();
        emit signalTooltipEventOnContact(contact, globalvisualRectTopLeft, globalMousePos);

        return true;
    }
        break;
    case QEvent::Resize:
    {
        QResizeEvent *e = static_cast<QResizeEvent *>(event);
        QTreeWidget::resizeEvent(e);
        updateGeometries();
    }
        break;

//    case QEvent::Enter:
//    {
//        QHoverEvent *e = static_cast<QHoverEvent *>(event);
//        QTreeWidgetItem *item = itemAt(e->pos());
//        if(!item){return false;}
//        if(item->parent() == 0){return false;}
//        ContactWidget *wgt = qobject_cast<ContactWidget *>( itemWidget(item, 0) );
//        if(!wgt){return false;}

//        item->setBackground(0, QBrush(Qt::gray));

//        qDebug()<<"----QEvent::HoverEnter";
//    }
//        break;

    default:
        break;

    }

//    qDebug()<<"event:"<<event->type();

    return QTreeWidget::event(event);

}



//void ContactBox::contextMenuEvent(QContextMenuEvent *e)
//{
//    QTreeWidgetItem *item = itemAt(e->pos());
//    if(!item){return;}

//    if(item->parent() == 0){
//        handleContextMenuEventOnContactGroup(item, mapToGlobal(e->pos()));
//    }else{
//        handleContextMenuEventOnContact(item, mapToGlobal(e->pos()));
//    }

//    e->accept();
//}

//void ContactBox::resizeEvent(QResizeEvent *e)
//{
//    QTreeWidget::resizeEvent(e);
//    if (const int numTopLevels = topLevelItemCount()) {
//        for (int i = 0; i < numTopLevels; i++){
//        }
//    }

//}




void ContactBox::handleMousePress(QTreeWidgetItem* item){

    if (item == 0)
        return;

    if (QApplication::mouseButtons() != Qt::LeftButton)
        return;

    if (item->parent() == 0) {
        setItemExpanded(item, !isItemExpanded(item));
        return;
    }

}

void ContactBox::handleMouseDoubleClick(QTreeWidgetItem* item){

    if (item == 0 || item->parent() == 0){
        return;
    }

    Contact *contact = contactsHash.key(item);
    if(!contact){
        return;
    }

    emit signalContactItemActivated(contact);

}

void ContactBox::handleCurrentItemChanged(QTreeWidgetItem * current, QTreeWidgetItem * previous){


    if(previous && (previous->parent() != 0) ){
        ContactWidget *wgt = qobject_cast<ContactWidget *>( itemWidget(previous, 0) );
        if(!wgt){return;}
        wgt->setExpanded(false);

        previous->setSizeHint(0, wgt->sizeHint());

    }

    if(current &&(current->parent() != 0) ){
        ContactWidget *wgt = qobject_cast<ContactWidget *>( itemWidget(current, 0) );
        if(!wgt){return;}
        wgt->setExpanded(true);

        current->setSizeHint(0, wgt->sizeHint());

    }


    updateGeometries();

}

void ContactBox::handleContextMenuEventOnContactGroup(QTreeWidgetItem* item, const QPoint &global_mouse_pos){
    qDebug()<<"--ContactBox::handleContextMenuEventOnContactGroup(...)";


    ContactGroupBase *group = contactGroupsHash.key(item);
    if(!group){return;}

    emit signalContextMenuEventOnContactGroup(group, global_mouse_pos);

}

void ContactBox::handleContextMenuEventOnContact(QTreeWidgetItem* item, const QPoint &global_mouse_pos){
    qDebug()<<"--ContactBox::handleContextMenuEventOnContact(...)";


    Contact *contact = contactsHash.key(item);
    if(!contact){
        return;
    }

    emit signalContextMenuEventOnContact(contact, global_mouse_pos);

}

//void ContactBox::handleTooltipEventOnContact(QTreeWidgetItem* item, const QPoint &global_item_topLeft_pos, const QPoint &global_mouse_pos){

//    qDebug()<<"--MainWindow::handleTooltipEventOnItem()";


//    Contact *contact = contactsHash.key(item);
//    if(!contact){
//        return;
//    }

//    emit signalTooltipEventOnContact(contact, global_item_topLeft_pos, global_mouse_pos);

//}





} //namespace HEHUI
