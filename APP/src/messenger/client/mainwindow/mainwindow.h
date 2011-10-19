#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QWidget>
#include <QTranslator>



#include "../ui/ui_mainwindow.h"
#include "contactsmanager/contactsmanager.h"
#include "../chatwindow/chatwindowmanager.h"
#include "../networkmanager/clientnetworkmanager.h"
#include "../packetmanager/clientpacketsparser.h"
#include "../search/search.h"

#include "../../sharedim/constants_global_shared.h"

#include "informationtips/userinfotipwindow.h"
#include "contactinfowidget/contactinfowidget.h"

#include "HHSharedCore/hdatabaseutility.h"
#include "HHSharedGUI/hmainwindowbase.h"
#include "HHSharedGUI/hsystemtrayiconbase.h"
#include "HHSharedGUI/hexpandlistviewmanager.h"





namespace HEHUI {


class MainWindow: public MainWindowBase {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0, HEHUI::WindowPosition positon = HEHUI::Center);
    ~MainWindow();


    QSystemTrayIcon *SystemTrayIcon();

    void retranslateUi();

    void setTrayIconVisible(bool visible);


protected:
    void closeEvent(QCloseEvent * event);
    void changeEvent(QEvent * event);


signals:
    void signalOnlineStateChanged(quint8 onlineStateCode);


public slots:
    void slotIconActivated(QSystemTrayIcon::ActivationReason reason);

    void slotSystemConfig();
    void slotBugReport();
    void slotHelp();
    void slotAbout();

    void slotQuit();

    void slotUpdateContactsInfo();
    void slotUpdateBlacklistInfo();

    void updateAllInterestGroupsInfoToUI();
    void updateInterestGroupInfoToUI(InterestGroup *interestGroup);


private slots:
    void slotInitPlugin(AbstractPluginInterface *plugin);
    void slotUserVerified();
    void slotLockUI();

    void slotContextMenuEventOnCategoryOccurs(const QString &group_name, const QPoint &global_mouse_pos, QMenu *contextMenu);
    void slotContextMenuEventOnObjectItemOccurs(const QString &contactID, const QPoint &global_mouse_pos, QMenu *contextMenu);
    void slotTooltipEventOnObjectItemOccurs(const QString &contactID, const QPoint &global_item_topLeft_pos, const QPoint &global_mouse_pos);

    void slotMoveContactToGroup();
    void slotMoveContactToBlacklist();
    void slotDeleteContact(const QString &contactID, const QString &existingGroupName);

    void slotProcessUpdatePasswordResult(quint8 errorTypeCode, const QString &message);

    void slotProcessLoginResult(quint8 errorTypeCode);
    void slotProcessContactStateChanged(const QString &contactID, quint8 onlineStateCode, const QString &contactHostAddress, quint16 contactHostPort);
    void slotProcessContactsOnlineInfo(const QString &contactsOnlineInfoString);
    void slotProcessUserInfo(const QString &userID, const QString &userInfo);
    void slotProcessContactGroupsInfo(const QString &contactGroupsInfo, quint32 personalContactGroupsInfoVersionOnServer);

    //void slotProcessSearchContactsResult(const QString &users);
    void slotProcessAddContactResult(const QString &userID, const QString &userNickName, const QString &userFace, quint8 errorTypeCode, const QString &reasonMessage);
    void getNewContactSettings(const QString &contactID);
    void slotProcessBlacklistInfo(const QString &blacklistOnServer, quint32 blacklistInfoVersionOnServer);

    void slotSearch();
    void slotProcessContactRequestFromUser(const QString &userID, const QString &userNickName, const QString &userFace, const QString &verificationMessage);
    void showContactRequestFromUser(const QString &userID, const QString &userNickName, const QString &userFace, const QString &verificationMessage);

    void slotProcessChatMessageReceivedFromContact(const QString &contactID, const QString &message, const QString &time = "");
    void slotProcessChatMessageCachedOnServer(const QStringList &messages);
    void slotProcessReceivedChatImage(const QString &fileName);

    void slotProcessInterestGroupChatMessagesReceivedFromContact(quint32 interestGroupID, const QString &contactID, const QString &message, const QString &time = "");
    void slotProcessInterestGroupChatMessagesCachedOnServer(const QStringList &messages);

    void slotSendChatMessageToContact(Contact *contact, const QString &message, const QStringList &imageList);
    void slotSendChatMessageToInterestGroup(InterestGroup *interestGroup, const QString &message, const QStringList &imageList);


    void slotProcessInterestGroupsList(const QString &interestGroupsListFromServer, quint32 interestGroupsInfoVersionOnServer);
    void slotProcessInterestGroupInfo(const QString &interestGroupInfoFromServer, quint32 groupID);
    void slotProcessInterestGroupMembersInfo(const QString &interestGroupMembersInfoFromServer, quint32 interestGroupMembersInfoVersionOnServer, quint32 groupID);

    void interestGroupItemActivated(QListWidgetItem * item );

    void on_toolButtonUserFace_clicked(){showUserInfo(imUser);}
    void showUserInfo(IMUserBase *user);


    void requestLogin(const QHostAddress &serverHostAddress, quint16 serverHostPort);

    void peerConnected(const QHostAddress &peerAddress, quint16 peerPort);
    void signalConnectToPeerTimeout(const QHostAddress &peerAddress, quint16 peerPort);
    void peerDisconnected(const QHostAddress &peerAddress, quint16 peerPort, bool normalClose);



private:

    void setupSystemTray();

    void initUI();

    void checkNetwork();
    void startNetwork();
    void stopNetwork();

    //退出前的清理工作
    void aboutToQuit();

    void savePreferedStyle(const QString &preferedStyle, bool useStylePalette);
    void savePreferedLanguage(const QString &preferedLanguage);


private:
    Ui::MainWindowUIClass ui;


    SystemTrayIconBase *systemTray;
    QMenu *trayMenu;

    QMenu *pluginsMenu;

    IMUser *imUser;
    IM::OnlineState stateBeforeLocking;

    ExpandListViewManager *expandListViewManager;
    ExpandListView *friendsListView;
//    ExpandListView *teamWidgetBox;
//    ExpandListView *blacklistWidgetBox;

    ContactsManager *contactsManager;

    ChatWindowManager *chatWindowManager;

    PacketHandlerBase *m_packetHandler;
    ClientNetworkManager *networkManager;
    ClientPacketsParser *clientPacketsParser;
    bool networkStarted;

    Search *search;

    bool atuoShowSystemMessage;
    bool autoShowChatMessageFromContact;

    UserInfoTipWindow *m_userInfoTipWindow;
    //ContactInfoWidget *m_ContactInfoWidget;

    RUDPSocket *rudpSocket;

};

} //namespace HEHUI

#endif // MAINWINDOW_H
