#include <QMdiSubWindow>
#include <QDebug>

#include "chatwindowmanager.h"
#include "../imageresource.h"


#include "../contactsmanager/contactsmanager.h"

namespace HEHUI {

ChatWindowManager::ChatWindowManager(QWidget *parent)
    : QMainWindow(parent)
{
	ui.setupUi(this);

    connect(ui.mdiArea, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(showContextMenu(const QPoint &)));

    m_chatWindowDisplayStyle = MDIChatWindow;
    ui.stackedWidget->setCurrentWidget(ui.pageMdiArea);

    initTabWidget();

}

ChatWindowManager::~ChatWindowManager()
{

}

void ChatWindowManager::closeEvent(QCloseEvent *e) {

    ui.mdiArea->closeAllSubWindows();
    showMinimized();
    hide();

    e->ignore();


/*
    if (Settings::instance()->getHideOnClose()) {
    	showMinimized();
        hide();
        e->ignore();
    }else{

    ui.mdiArea->closeAllSubWindows();
 	//Clear the pairList
    	pairList.clear();

	e->accept();
    }
*/


}

void ChatWindowManager::setChatWindowDisplayStyle(ChatWindowManager::ChatWindowDisplayStyle style){
    m_chatWindowDisplayStyle = style;
}

ChatWindowManager::ChatWindowDisplayStyle ChatWindowManager::getChatWindowDisplayStyle(){
    return m_chatWindowDisplayStyle;
}

void ChatWindowManager::switchChatWindowDisplayStyle(ChatWindowManager::ChatWindowDisplayStyle style){

    if(style == m_chatWindowDisplayStyle){return;}


    //TODO
    switch (m_chatWindowDisplayStyle) {
    case TabbedChatWindow:
    {
        int tabPages = ui.tabWidget->count();
        for(int i = tabPages; i >= 0; --i){
            ui.tabWidget->removeTab(i);
        }

        showMinimized();
        hide();
    }
        break;
    case MDIChatWindow:
    {
        foreach (QMdiSubWindow *window, ui.mdiArea->subWindowList()) {
            ui.mdiArea->removeSubWindow(window);
            window->widget()->setParent(0);
            window->deleteLater();
        }

        showMinimized();
        hide();
    }
        break;
    case SeparateChatWindow:
    {
        foreach (ContactChatWidget *ccw, m_contactChatWidgetHash.values()) {
            ccw->hide();
        }
        foreach (GroupChatWindow *gcw, m_groupChatWidgetHash.values()) {
            gcw->hide();
        }

    }
        break;
    default:
        break;
    }


    switch (style) {
    case TabbedChatWindow:
    {
        foreach (ContactChatWidget *ccw, m_contactChatWidgetHash.values()) {
            ui.tabWidget->addTab(ccw, ccw->displayName());
            ui.tabWidget->setCurrentWidget(ccw);
            ccw->show();
        }
        foreach (GroupChatWindow *gcw, m_groupChatWidgetHash.values()) {
            ui.tabWidget->addTab(gcw, gcw->interestGroup()->getGroupName());
            ui.tabWidget->setCurrentWidget(gcw);
            gcw->show();
        }

        ui.stackedWidget->setCurrentWidget(ui.pageTabWidget);
        showNormal();
        activateWindow();
        raise();
    }
        break;
    case MDIChatWindow:
    {
        foreach (ContactChatWidget *ccw, m_contactChatWidgetHash.values()) {
            QMdiSubWindow *subWindow = ui.mdiArea->addSubWindow(ccw);
            Contact *contact = ccw->contact();
            subWindow->setWindowIcon(ImageResource::createIconForContact(contact->getFace(), contact->getOnlineState()));
            //connect(subWindow, SIGNAL(destroyed()), this, SLOT(handleChatWindowClosed()));
            ccw->show();
            subWindow->show();
        }
        foreach (GroupChatWindow *gcw, m_groupChatWidgetHash.values()) {
            QMdiSubWindow *subWindow = ui.mdiArea->addSubWindow(gcw);
            subWindow->setWindowIcon(ImageResource::createIconForInterestGroup());
            //connect(subWindow, SIGNAL(destroyed()), this, SLOT(handleChatWindowClosed()));
            gcw->show();
        }

        ui.stackedWidget->setCurrentWidget(ui.pageMdiArea);
        showNormal();
        activateWindow();
        raise();
    }
        break;
    case SeparateChatWindow:
    {
        foreach (ContactChatWidget *ccw, m_contactChatWidgetHash.values()) {
            ccw->showNormal();
        }
        foreach (GroupChatWindow *gcw, m_groupChatWidgetHash.values()) {
            gcw->showNormal();
        }

        showMinimized();
        hide();
    }
        break;
    default:
        break;
    }

    m_chatWindowDisplayStyle = style;


}

void ChatWindowManager::slotNewChatWithContact(const QString &contactID){
    qDebug()<<"----ChatWindowManager::slotNewChatWithContact(const QString &id)~~~";

    Contact *contact = ContactsManager::instance()->getUser(contactID);
    if(!contact){
        qCritical()<<"XXXXChatWindowManager::slotNewMessageReceivedFromContact(...)~~Error:No such contact:"<<contactID;
        return;
    }

    ContactChatWidget *ccw = 0;

    switch (m_chatWindowDisplayStyle) {
    case TabbedChatWindow:
    {
        ccw = findContactChatTabWidget(contact);
        if(!ccw){
            ccw = createContactChatWindow(contact);
            ui.tabWidget->addTab(ccw, ccw->displayName());
        }
        ui.tabWidget->setCurrentWidget(ccw);
    }
        break;
    case MDIChatWindow:
    {
        QMdiSubWindow * subWindow = findChatWithContactMdiSubWindow(contact);
        if(!subWindow){
            ccw = createContactChatWindow(contact);
            QMdiSubWindow *subWindow = ui.mdiArea->addSubWindow(ccw);
            subWindow->setWindowIcon(ImageResource::createIconForContact(contact->getFace(), contact->getOnlineState()));
            //connect(subWindow, SIGNAL(destroyed()), this, SLOT(handleChatWindowClosed()));
            ccw->show();
        }else{
            ui.mdiArea->setActiveSubWindow(subWindow);
        }
    }
        break;
    case SeparateChatWindow:
    {
        ccw = m_contactChatWidgetHash.value(contactID);
        if(!ccw){
            ccw = createContactChatWindow(contact);
        }
        ccw->show();
        ccw->activateWindow();
        QApplication::alert(ccw);
    }
        return;
    default:
        break;
    }



    showNormal();
    activateWindow();
    raise();


}

void ChatWindowManager::slotNewMessageReceivedFromContact(const QString &contactID, const QString &message, const QString &time){
    //TODO:
    qWarning()<< "----ChatWindowManager::slotNewMessageReceivedFromContact(....)~~From:"<<contactID<<" msg:"<<message<<" Time:"<<time;


    Contact *contact = ContactsManager::instance()->getUser(contactID);
    if(!contact){
        qCritical()<<"Error:No such contact:"<<contactID;
        return;
    }

    ContactChatWidget *contactChatWindow = 0;

    switch (m_chatWindowDisplayStyle) {
    case TabbedChatWindow:
    {
        contactChatWindow = findContactChatTabWidget(contact);
        if(!contactChatWindow){
            contactChatWindow = createContactChatWindow(contact);
            ui.tabWidget->addTab(contactChatWindow, contactChatWindow->displayName());
        }
        ui.tabWidget->setCurrentWidget(contactChatWindow);

        update();
        showNormal();
        raise();
    }
        break;
    case MDIChatWindow:
    {
        QMdiSubWindow * subWindow = findChatWithContactMdiSubWindow(contact);
        if(!subWindow){
            contactChatWindow = createContactChatWindow(contact);
            QMdiSubWindow *subWindow = ui.mdiArea->addSubWindow(contactChatWindow);
            subWindow->setWindowIcon(ImageResource::createIconForContact(contact->getFace(), contact->getOnlineState()));
            //connect(subWindow, SIGNAL(destroyed()), this, SLOT(handleChatWindowClosed()));
            contactChatWindow->show();
        }else{
            ui.mdiArea->setActiveSubWindow(subWindow);
        }

        update();
        showNormal();
        raise();
    }
        break;
    case SeparateChatWindow:
    {
        contactChatWindow = m_contactChatWidgetHash.value(contactID);
        if(!contactChatWindow){
            contactChatWindow = createContactChatWindow(contact);
        }
        contactChatWindow->show();
        contactChatWindow->activateWindow();
        QApplication::alert(contactChatWindow);
    }
        break;
    default:
        break;
    }



    if(time.trimmed().isEmpty()){
        contactChatWindow->appendMessageReceivedFromContact(message);
    }else{
        contactChatWindow->appendMessageReceivedFromContact(message, 0, time);
    }


}

void ChatWindowManager::slotNewChatWithInterestGroup(quint32 interestGroupID){

    InterestGroup *group = ContactsManager::instance()->getInterestGroup(interestGroupID);
    if(!group){
        qCritical()<<"Error:No such interest group:"<<interestGroupID;
        return;
    }

    GroupChatWindow *groupChatWindow = 0;

    switch (m_chatWindowDisplayStyle) {
    case TabbedChatWindow:
    {
        groupChatWindow = findInterestGroupChatTabWidget(group);
        if(!groupChatWindow){
            groupChatWindow = createGroupChatWindow(group);
            ui.tabWidget->addTab(groupChatWindow, group->getGroupName());
        }
        ui.tabWidget->setCurrentWidget(groupChatWindow);
    }
        break;
    case MDIChatWindow:
    {
        QMdiSubWindow * subWindow = findChatWithInterestGroupMdiSubWindow(group);
        if(!subWindow){
            groupChatWindow = createGroupChatWindow(group);
            QMdiSubWindow *subWindow = ui.mdiArea->addSubWindow(groupChatWindow);
            subWindow->setWindowIcon(ImageResource::createIconForInterestGroup());
            //connect(subWindow, SIGNAL(destroyed()), this, SLOT(handleChatWindowClosed()));
            groupChatWindow->show();
        }else{
            ui.mdiArea->setActiveSubWindow(subWindow);
        }
    }
        break;
    case SeparateChatWindow:
    {
        groupChatWindow = m_groupChatWidgetHash.value(interestGroupID);
        if(!groupChatWindow){
            groupChatWindow = createGroupChatWindow(group);
        }
        groupChatWindow->show();
        groupChatWindow->activateWindow();
        QApplication::alert(groupChatWindow);
    }
        return;
    default:
        break;
    }


    update();
    showNormal();
    raise();

}

void ChatWindowManager::slotNewMessageReceivedFromInterestGroup(quint32 interestGroupID, const QString &contactID, const QString &message, const QString &time){

    InterestGroup *group = ContactsManager::instance()->getInterestGroup(interestGroupID);
    if(!group){
        qCritical()<<"Error:No such interest group:"<<interestGroupID;
        return;
    }
    if(!group->hasMember(contactID)){
        qCritical()<<QString("Error:No such member '%1' in group '%2'!").arg(contactID).arg(interestGroupID);
        return;
    }
    Contact *contact = ContactsManager::instance()->getUser(contactID);
    if(!contact){
        qCritical()<<"Error:No such contact:"<<contactID;
        return;
    }



    GroupChatWindow *groupChatWindow = 0;

    switch (m_chatWindowDisplayStyle) {
    case TabbedChatWindow:
    {
        groupChatWindow = findInterestGroupChatTabWidget(group);
        if(!groupChatWindow){
            groupChatWindow = createGroupChatWindow(group);
            ui.tabWidget->addTab(groupChatWindow, group->getGroupName());
        }
        ui.tabWidget->setCurrentWidget(groupChatWindow);

        update();
        showNormal();
        raise();
    }
        break;
    case MDIChatWindow:
    {
        QMdiSubWindow * subWindow = findChatWithInterestGroupMdiSubWindow(group);
        if(!subWindow){
            groupChatWindow = createGroupChatWindow(group);
            QMdiSubWindow *subWindow = ui.mdiArea->addSubWindow(groupChatWindow);
            subWindow->setWindowIcon(ImageResource::createIconForInterestGroup());
            //connect(subWindow, SIGNAL(destroyed()), this, SLOT(handleChatWindowClosed()));
            groupChatWindow->show();
        }else{
            ui.mdiArea->setActiveSubWindow(subWindow);
        }

        update();
        showNormal();
        raise();
    }
        break;
    case SeparateChatWindow:
    {
        groupChatWindow = m_groupChatWidgetHash.value(interestGroupID);
        if(!groupChatWindow){
            groupChatWindow = createGroupChatWindow(group);
        }
        groupChatWindow->show();
        groupChatWindow->activateWindow();
        QApplication::alert(groupChatWindow);
    }
        break;
    default:
        break;
    }


    if(time.trimmed().isEmpty()){
        groupChatWindow->appendMessageReceivedFromContact(message, contact);
    }else{
        groupChatWindow->appendMessageReceivedFromContact(message, contact, time);
    }


}

void ChatWindowManager::initTabWidget(){

    connect(ui.tabWidget, SIGNAL(currentChanged(int)), this, SLOT(slotTabPageChanged()));

    QTabBar *tabBar = ui.tabWidget->tabBar();
    QStyleOptionTab opt;
    if (tabBar) {
        opt.init(tabBar);
        opt.shape = tabBar->shape();
    }

    // workaround for sgi style
    //QPalette pal = palette();
    // pal.setColor(QPalette::Active, QPalette::Button, pal.color(QPalette::Active, QPalette::Window));
    // pal.setColor(QPalette::Disabled, QPalette::Button, pal.color(QPalette::Disabled, QPalette::Window));
    // pal.setColor(QPalette::Inactive, QPalette::Button, pal.color(QPalette::Inactive, QPalette::Window));

    QToolButton *newTabButton = new QToolButton(this);
    ui.tabWidget->setCornerWidget(newTabButton, Qt::TopLeftCorner);
    newTabButton->setCursor(Qt::ArrowCursor);
    newTabButton->setAutoRaise(true);
    newTabButton->setIcon(QIcon(":/resources/images/addtab.png"));
    QObject::connect(newTabButton, SIGNAL(clicked()), this, SLOT(slotNewTab()));
    newTabButton->setToolTip(tr("Add Page"));
//    newTabButton->setEnabled(false);

    QToolButton *closeTabButton = new QToolButton(this);
    //closeTabButton->setPalette(pal);
    ui.tabWidget->setCornerWidget(closeTabButton, Qt::TopRightCorner);
    closeTabButton->setCursor(Qt::ArrowCursor);
    closeTabButton->setAutoRaise(true);
    closeTabButton->setIcon(QIcon(":/resources/images/closetab.png"));
    QObject::connect(closeTabButton, SIGNAL(clicked()), this, SLOT(slotcloseTab()));
    closeTabButton->setToolTip(tr("Close Page"));
//    closeTabButton->setEnabled(false);


}

void ChatWindowManager::slotTabPageChanged(){

//    SystemManagementWidget *systemManagementWidget = qobject_cast<SystemManagementWidget *>(ui.tabWidget->currentWidget());
//    if(systemManagementWidget){
//        if(systemManagementWidget == localSystemManagementWidget){
//            ui.tabWidget->cornerWidget(Qt::TopRightCorner)->setEnabled(false);
//        }else{
//            ui.tabWidget->cornerWidget(Qt::TopRightCorner)->setEnabled(true);
//        }
//    }else{
//        ui.tabWidget->cornerWidget(Qt::TopRightCorner)->setEnabled(false);
//    }


}

void ChatWindowManager::slotNewTab(){

    switchChatWindowDisplayStyle(SeparateChatWindow);


    //    slotRemoteManagement();
    //    updateGeometry();

    //    QString localComputerName = QHostInfo::localHostName();
    //    SystemManagementWidget *systemManagementWidget = new SystemManagementWidget(m_adminName, controlCenterPacketsParser, localComputerName, "", "127.0.0.1", "");
    //    ui.tabWidget->addTab(systemManagementWidget, tr("Computer"));
    //    ui.tabWidget->setCurrentWidget(systemManagementWidget);

}

void ChatWindowManager::slotcloseTab(){


    ContactChatWidget *ccw = qobject_cast<ContactChatWidget *>(ui.tabWidget->currentWidget());
    if(ccw){
        ui.tabWidget->removeTab(ui.tabWidget->currentIndex());
        ccw->close();
        ccw->deleteLater();
        m_contactChatWidgetHash.remove(ccw->contact()->getUserID());
    }

    GroupChatWindow *gcw = qobject_cast<GroupChatWindow *>(ui.tabWidget->currentWidget());
    if(gcw){
        ui.tabWidget->removeTab(ui.tabWidget->currentIndex());
        gcw->close();
        gcw->deleteLater();
        m_groupChatWidgetHash.remove(gcw->interestGroup()->getGroupID());
    }

//    if(ui.tabWidget->count() == 0){
//        showMinimized();
//        hide();
//    }

    //如果只有一页，则关闭按钮不可用
    //ui.tabWidget->cornerWidget(Qt::TopRightCorner)->setEnabled(ui.tabWidget->count() > 1);


}


//void ChatWindowManager::handleChatWindowClosed(){

//    QMdiSubWindow *sw = qobject_cast<QMdiSubWindow *>(sender());

//    ContactChatWidget *ccw = qobject_cast<ContactChatWidget *>(sw->widget());
//    if(ccw){
//        ui.tabWidget->removeTab(ui.tabWidget->currentIndex());
//        ccw->close();
//        ccw->deleteLater();
//        m_contactChatWidgetHash.remove(ccw->contact()->getUserID());
//    }

//    GroupChatWindow *gcw = qobject_cast<GroupChatWindow *>(sw->widget());
//    if(gcw){
//        ui.tabWidget->removeTab(ui.tabWidget->currentIndex());
//        gcw->close();
//        gcw->deleteLater();
//        m_groupChatWidgetHash.remove(gcw->interestGroup()->getGroupID());
//    }

//    if(!ui.mdiArea->subWindowList().size()){
//        showMinimized();
//        hide();
//    }
    
//}

void ChatWindowManager::handleChatWindowClosed(){
    qDebug()<<"----ChatWindowManager::handleChatWindowClosed()";

    ContactChatWidget *ccw = qobject_cast<ContactChatWidget *>(sender());
    if(ccw){
        m_contactChatWidgetHash.remove(ccw->contact()->getUserID());
    }

    GroupChatWindow *gcw = qobject_cast<GroupChatWindow *>(sender());
    if(gcw){
        m_groupChatWidgetHash.remove(gcw->interestGroup()->getGroupID());
    }

    if(((m_chatWindowDisplayStyle == MDIChatWindow) && (!ui.mdiArea->subWindowList().size())) || ((m_chatWindowDisplayStyle == TabbedChatWindow) && (!ui.tabWidget->count())) ){
        showMinimized();
        hide();
    }

}


void ChatWindowManager::showContextMenu(const QPoint &pos){

    if(m_chatWindowDisplayStyle == TabbedChatWindow || m_chatWindowDisplayStyle == SeparateChatWindow) {
        return;
    }


    QMenu menu;
    if(ui.mdiArea->viewMode() == QMdiArea::SubWindowView){
        menu.addAction(tr("Cascade Windows"), ui.mdiArea, SLOT(cascadeSubWindows()));
        menu.addAction(tr("Tile Windows"), ui.mdiArea, SLOT(tileSubWindows()));
        menu.addSeparator();
//        menu.addAction(tr("Tabbed View"), this, SLOT(switchToTabbedView()));
                menu.addAction(tr("Tabbed View"), this, SLOT(slotNewTab()));

    }else{
//        menu.addAction(tr("SubWindow View"), this, SLOT(switchToSubWindowView()));
    }


    menu.exec(mapToGlobal(pos));

}

void ChatWindowManager::switchToSubWindowView(){
    ui.mdiArea->setViewMode(QMdiArea::SubWindowView);
}

void ChatWindowManager::switchToTabbedView(){
    ui.mdiArea->setViewMode(QMdiArea::TabbedView);
}

ContactChatWidget * ChatWindowManager::createContactChatWindow(Contact *contact){
        ContactChatWidget *contactChatWindow = new ContactChatWidget(contact);
        connect(contactChatWindow, SIGNAL(sendMsgButtonClicked(Contact *, const QString &, const QStringList &)), this, SIGNAL(signalSendChatMessageToCantact(Contact *, const QString &, const QStringList &)));
        connect(this, SIGNAL(signalChatImageReceived(const QString&)), contactChatWindow, SLOT(updateImage(const QString&)));

        connect(contactChatWindow, SIGNAL(destroyed()), this, SLOT(handleChatWindowClosed()));

        m_contactChatWidgetHash.insert(contact->getUserID(), contactChatWindow);

//        QMdiSubWindow *subWindow = ui.mdiArea->addSubWindow(contactChatWindow);
//        //subWindow->setWindowIcon(ImageResource::createMixedIcon((QString(RESOURCE_PATH) + "/" +contact->getFace()), contact->getOnlineState()));
//        subWindow->setWindowIcon(ImageResource::createIconForContact(contact->getFace(), contact->getOnlineState()));

//        connect(subWindow, SIGNAL(destroyed()), this, SLOT(chatWindowClosed()));

        return contactChatWindow;

}

QMdiSubWindow* ChatWindowManager::findChatWithContactMdiSubWindow(Contact *contact){

    foreach (QMdiSubWindow *window, ui.mdiArea->subWindowList()) {
        ContactChatWidget *messageWindow = qobject_cast<ContactChatWidget *>(window->widget());
        if(!messageWindow){continue;}
        if(messageWindow->contact() == contact){
            return window;
        }
    }

    return 0;

}

ContactChatWidget * ChatWindowManager::findContactChatTabWidget(Contact *contact){

    int tabPages = ui.tabWidget->count();
    for(int i = tabPages; i >= 0; --i){
        ContactChatWidget *ccw = qobject_cast<ContactChatWidget *>(ui.tabWidget->widget(i));
        if(!ccw){continue;}
        if(ccw->contact() == contact){
            return ccw;
        }
    }

    return 0;
}

GroupChatWindow* ChatWindowManager::createGroupChatWindow(InterestGroup *group){

    //InterestGroup *group = ContactsManager::instance()->getInterestGroup(interestGroupID);
    if(!group){
            qCritical()<<"Error:No such interest group!";
            return 0;
    }

    GroupChatWindow *groupChatWindow = new GroupChatWindow(group);
    connect(groupChatWindow, SIGNAL(sendMsgButtonClicked(InterestGroup *, const QString &, const QStringList &)), this, SIGNAL(signalSendChatMessageToInterestGroup(InterestGroup *, const QString &, const QStringList &)));
    connect(this, SIGNAL(signalChatImageReceived(const QString&)), groupChatWindow, SLOT(updateImage(const QString&)));

    connect(groupChatWindow, SIGNAL(destroyed()), this, SLOT(handleChatWindowClosed()));

    m_groupChatWidgetHash.insert(group->getGroupID(), groupChatWindow);



//    QMdiSubWindow *subWindow = ui.mdiArea->addSubWindow(groupChatWindow);
//    subWindow->setWindowIcon(ImageResource::createIconForInterestGroup());
//    connect(subWindow, SIGNAL(destroyed()), this, SLOT(chatWindowClosed()));

    return groupChatWindow;
}

QMdiSubWindow* ChatWindowManager::findChatWithInterestGroupMdiSubWindow(InterestGroup *group){

    foreach (QMdiSubWindow *window, ui.mdiArea->subWindowList()) {
            GroupChatWindow *groupChatWindow = qobject_cast<GroupChatWindow *>(window->widget());
            if(!groupChatWindow){continue;}
            if(groupChatWindow->interestGroup() == group){
                    return window;
            }
    }

    return 0;

}

GroupChatWindow * ChatWindowManager::findInterestGroupChatTabWidget(InterestGroup *group){

    int tabPages = ui.tabWidget->count();
    for(int i = tabPages; i >= 0; --i){
        GroupChatWindow *gcw = qobject_cast<GroupChatWindow *>(ui.tabWidget->widget(i));
        if(!gcw){continue;}
        if(gcw->interestGroup() == group){
            return gcw;
        }
    }

    return 0;

}



//bool ChatWindowManager::isChatWindowOpened(Contact *contact){
//	return false;
//}

























} //namespace HEHUI
