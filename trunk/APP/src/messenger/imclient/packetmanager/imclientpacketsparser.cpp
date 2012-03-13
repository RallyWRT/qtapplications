/*
 ****************************************************************************
 * imclientpacketsparser.cpp
 *
 * Created On: 2010-7-13
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
 * Last Modified On: 2012-3-4
 * Last Modified By: 贺辉
 ***************************************************************************
 */



#include "imclientpacketsparser.h"




namespace HEHUI {


IMClientPacketsParser::IMClientPacketsParser(ClientResourcesManager *resourcesManager, QObject *parent)
    :QObject(parent)
{


    Q_ASSERT(resourcesManager);

    m_udpServer = resourcesManager->getUDPServer();
    Q_ASSERT_X(m_udpServer, "IMClientPacketsParser::IMClientPacketsParser(...)", "Invalid UDPServer!");
    connect(m_udpServer, SIGNAL(signalNewUDPPacketReceived(Packet*)), this, SLOT(parseIncomingPacketData(Packet*)), Qt::QueuedConnection);

    m_rtp = resourcesManager->getRTP();
    Q_ASSERT(m_rtp);

    m_udtProtocol = m_rtp->getUDTProtocol();
    Q_ASSERT(m_udtProtocol);
    m_udtProtocol->startWaitingForIOInOneThread(100);
    //m_udtProtocol->startWaitingForIOInSeparateThread(100, 1000);
    connect(m_udtProtocol, SIGNAL(packetReceived(Packet*)), this, SLOT(parseIncomingPacketData(Packet*)), Qt::QueuedConnection);

    m_tcpServer = m_rtp->getTCPServer();
    Q_ASSERT(m_tcpServer);
    connect(m_tcpServer, SIGNAL(packetReceived(Packet*)), this, SLOT(parseIncomingPacketData(Packet*)), Qt::QueuedConnection);



    //    heartbeatTimer = 0;
    //    processWaitingForReplyPacketsTimer = 0;

    //packetHandlerBase = new PacketHandlerBase(this);
    //m_packetHandlerBase = packetHandlerBase;


    //    m_networkManager = ClientNetworkManager::instance();
    //    connect(networkManager, SIGNAL(signalNewPacketReceived(Packet *)), this, SLOT(slotNewPacketReceived(Packet *)));
    //    processingPackets = false;

    //    localIPMCListeningAddress = NetworkManager::instance()->localIPMCListeningAddress();
    //    localIPMCListeningPort = NetworkManager::instance()->localIPMCListeningPort();

    


    serverLastOnlineTime = QDateTime();

    user = IMUser::instance();
    m_myUserID = user->getUserID();

    cryptography = new Cryptography();
    sessionEncryptionKey = QByteArray();
    

}

IMClientPacketsParser::~IMClientPacketsParser() {
    // TODO Auto-generated destructor stub

    qDebug()<<"IMClientPacketsParser::~IMClientPacketsParser() ";

    QMutexLocker locker(&mutex);

    //    if(heartbeatTimer){
    //        qDebug()<<"----------------------------33333333";
    //        heartbeatTimer->stop();
    //        qDebug()<<"----------------------------44444444";
    //    }
    //    delete heartbeatTimer;
    //    heartbeatTimer = 0;


    //    if(processWaitingForReplyPacketsTimer){
    //        processWaitingForReplyPacketsTimer->stop();
    //    }
    //    delete processWaitingForReplyPacketsTimer;
    //    processWaitingForReplyPacketsTimer = 0;


    if(cryptography){
        delete cryptography;
        cryptography = 0;
    }


    //networkManager = 0;

}




void IMClientPacketsParser::parseIncomingPacketData(Packet *packet){
    //    qDebug()<<"----IMClientPacketsParser::parseIncomingPacketData(Packet *packet)";

    //        if((packet->getTransmissionProtocol() == TP_UDP)
    //            && (networkManager->isLocalAddress(packet->getPeerHostAddress()))
    //            && (packet->getPeerHostPort() == localIPMCListeningPort)){
    //            qDebug()<<"~~Packet is been discarded!";
    //            return;
    //        }else if((packet->getTransmissionProtocol() == TP_TCP)
    //            && (packet->getPeerHostAddress() == networkManager->localTCPListeningAddress())
    //            && (packet->getPeerHostPort() == networkManager->localTCPListeningPort())){
    //            qDebug()<<"~~Packet is been discarded!";
    //            return;
    //        }

    //    qDebug()<<"~~networkManager->localAddress():"<<networkManager->localTCPListeningAddress().toString();
    //    qDebug()<<"~~packet->getPeerHostAddress():"<<packet->getPeerHostAddress();

    QByteArray packetData = packet->getPacketData();
    QDataStream in(&packetData, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_4_8);

    QString peerID = "";
    in >> peerID;

    QHostAddress peerAddress = packet->getPeerHostAddress();
    quint16 peerPort = packet->getPeerHostPort();

    quint8 packetType = packet->getPacketType();
    int socketID = packet->getSocketID();

    PacketHandlerBase::recylePacket(packet);

    switch(packetType){
    //    case quint8(HEHUI::HeartbeatPacket):
    //    {
    //        QString contactID;
    //        in >> contactID;
    //        emit signalHeartbeatPacketReceived(contactID);
    //        qDebug()<<"~~HeartbeatPacket--"<<" contactID:"<<contactID;
    //    }
    //    break;
    //    case quint8(HEHUI::ConfirmationOfReceiptPacket):
    //    {
    //        quint16 packetSerialNumber1 = 0, packetSerialNumber2 = 0;
    //        in >> packetSerialNumber1 >> packetSerialNumber2;
    //        m_packetHandlerBase->removeWaitingForReplyPacket(packetSerialNumber1, packetSerialNumber2);
    //        emit signalConfirmationOfReceiptPacketReceived(packetSerialNumber1, packetSerialNumber2);
    //        //qDebug()<<"~~ConfirmationOfReceiptPacket:"<<packetSerialNumber;
    //    }
    //    break;
    case quint8(IM::ServerDeclare):
    {

        quint16 port = 0;
        QString version;
        in >> port >> version;

        serverLastOnlineTime = QDateTime::currentDateTime();

        emit signalServerDeclarePacketReceived(peerAddress.toString(), port, peerID, version);

        qWarning()<<"~~ServerDeclare"<<" serverAddress:"<<peerAddress<<" servername:"<<peerID <<" serverRTPListeningPort:"<<port;
    }
        break;
    case quint8(IM::ServerOnline):
    {
        quint16 port;
        in >> port;

        serverLastOnlineTime = QDateTime::currentDateTime();

        emit signalServerOnlinePacketReceived(peerAddress.toString(), port, peerID);
        qDebug()<<"~~ServerOnline";
    }
        break;
    case quint8(IM::ServerOffline):
    {
        quint16 port;
        in >> port;

        //        stopHeartbeat();

        emit signalServerOfflinePacketReceived(peerAddress.toString(), port, peerID);
        qWarning()<<"~~ServerOffline";
    }
        break;
    case quint8(IM::ServerAnnouncement):
    {

        QString groupName, computerName, announcement;
        quint8 mustRead;
        in >> announcement >> mustRead;
        emit signalServerAnnouncementPacketReceived(announcement, (mustRead == quint8(0))?false:true);
        qDebug()<<"~~ServerAnnouncement"<<"groupName:"<<groupName<<"computerName:"<<computerName<<"announcement:"<<announcement<<"mustRead:"<<mustRead;
    }
        break;

    case quint8(IM::SERVER_RESPONSE_CLIENT_REGISTRATION):
    {
        
        qWarning()<<"--SERVER_RESPONSE_CLIENT_REQUEST_REGISTRATION";
        
        quint8 errorTypeCode = quint8(IM::ERROR_UnKnownError);
        QString message = "";
        in >> errorTypeCode >> message;
        emit signalRegistrationResultReceived(errorTypeCode, message);
        
    }
        break;

    case quint8(IM::SERVER_RESPONSE_CLIENT_REQUEST_UPDATE_PASSWORD):
    {
        
        qWarning()<<"--SERVER_RESPONSE_CLIENT_REQUEST_UPDATE_PASSWORD";
        

        quint8 errorTypeCode = quint8(IM::ERROR_UnKnownError);
        QString message = "";
        in >> errorTypeCode >> message;
        emit signalUpdatePasswordResultReceived(errorTypeCode, message);
        
    }
        break;

    case quint8(IM::SERVER_RESPONSE_CLIENT_REQUEST_LOGIN):
    {

        quint8 requestLoginResultCode = 0;
        in >> requestLoginResultCode;
        if(requestLoginResultCode){
            QString serverAddress = "";
            quint16 serverPort = 0;
            in >> serverAddress >> serverPort;
            emit signalLoginServerRedirected(serverAddress, serverPort, peerID);
        }else{
            quint8 errorTypeCode = quint8(IM::ERROR_UnKnownError);
            in >> errorTypeCode;
            emit signalLoginResultReceived(errorTypeCode);

        }

        qWarning()<<"--SERVER_RESPONSE_CLIENT_REQUEST_LOGIN";
    }
        break;

    case quint8(IM::SERVER_RESPONSE_CLIENT_LOGIN_REDIRECTION):
    {

        QString serverAddress = "";
        quint16 serverPort = 0;
        in >> serverAddress >> serverPort;

        emit signalLoginServerRedirected(serverAddress, serverPort, peerID);

    }
        break;

    case quint8(IM::SERVER_RESPONSE_CLIENT_LOGIN_RESULT):
    {

        quint8 loginResultCode = 10;
        in >> loginResultCode;
        quint8 errorTypeCode = quint8(IM::ERROR_UnKnownError);
        if(loginResultCode){
            QByteArray encryptedData;
            quint32 personalInfoVersionOnServer = 1, personalContactGroupsInfoVersionOnServer = 1, interestGroupsInfoVersionOnServer = 1, blacklistInfoVersionOnServer = 1;
            in >> encryptedData >> personalInfoVersionOnServer >> personalContactGroupsInfoVersionOnServer >> interestGroupsInfoVersionOnServer >> blacklistInfoVersionOnServer;

            QByteArray decryptedData;
            cryptography->teaCrypto(&decryptedData, encryptedData, sessionEncryptionKey, false);

            //TODO
            QDataStream stream(&decryptedData, QIODevice::ReadOnly);
            stream.setVersion(QDataStream::Qt_4_8);
            stream >> sessionEncryptionKey;

            user->setSessionEncryptionKey(sessionEncryptionKey);

            errorTypeCode = quint8(IM::ERROR_NoError);

            //TODO:
            user->loadMyInfoFromLocalDatabase();

            if(personalInfoVersionOnServer != user->getPersonalInfoVersion()){requestContactInfo(socketID, m_myUserID);}
            if(personalContactGroupsInfoVersionOnServer != user->getPersonalContactGroupsVersion()){updatePersonalContactGroupsInfo(socketID, false);}
            if(interestGroupsInfoVersionOnServer != user->getInterestGroupInfoVersion()){requestInterestGroupsList(socketID);}
            //requestInterestGroupsList();
            if(blacklistInfoVersionOnServer != user->getBlacklistInfoVersion()){requestBlacklistInfo(socketID);}

        }else{
            in >> errorTypeCode;
            //emit signalLoginResultReceived(IM::ErrorType(errorTypeCode));

        }

        emit signalLoginResultReceived(errorTypeCode);


        qWarning()<<"--SERVER_RESPONSE_CLIENT_LOGIN_RESULT";

    }
        break;

    case quint8(IM::ONLINE_STATE_CHANGED):
    {

        QByteArray encryptedData;
        in >> encryptedData;


        QByteArray decryptedData;
        cryptography->teaCrypto(&decryptedData, encryptedData, sessionEncryptionKey, false);
        //TODO
        QDataStream stream(&decryptedData, QIODevice::ReadOnly);
        stream.setVersion(QDataStream::Qt_4_8);
        QString contactID = "", contactHostAddress = "";
        quint16 contactHostPort = 0;
        quint8 stateCode = quint8(IM::ONLINESTATE_OFFLINE);
        stream >> contactID >> stateCode >> contactHostAddress >> contactHostPort;

        //emit signalContactStateChangedPacketReceived(contactID, IM::OnlineState(stateCode), contactHostAddress, contactHostPort);
        emit signalContactStateChangedPacketReceived(contactID, stateCode, contactHostAddress, contactHostPort);

        qWarning()<<"--ONLINE_STATE_CHANGED";
    }
        break;

    case quint8(IM::CONTACTS_ONLINE_INFO):
    {
        qWarning()<<"--CONTACTS_ONLINE_INFO";


        QByteArray encryptedData;
        in >> encryptedData;


        QByteArray decryptedData;
        cryptography->teaCrypto(&decryptedData, encryptedData, sessionEncryptionKey, false);
        //TODO
        QDataStream stream(&decryptedData, QIODevice::ReadOnly);
        stream.setVersion(QDataStream::Qt_4_8);
        QString contactsOnlineInfoString = "";
        stream >> contactsOnlineInfoString;



        emit signalContactsOnlineInfoPacketReceived(contactsOnlineInfoString);

    }
        break;


    case quint8(IM::SERVER_RESPONSE_USER_SUMMARY_INFO):
    {
        qWarning()<<"--SERVER_RESPONSE_USER_SUMMARY_INFO";
        

        //TODO:
        QString userID = "";
        QByteArray encryptedData;
        in >> userID >> encryptedData;


        QByteArray decryptedData;
        cryptography->teaCrypto(&decryptedData, encryptedData, sessionEncryptionKey, false);
        //TODO
        QDataStream stream(&decryptedData, QIODevice::ReadOnly);
        stream.setVersion(QDataStream::Qt_4_8);
        QString userInfo = "";
        stream >> userInfo;
        
        if(userID == user->getUserID()){
            user->setPersonalSummaryInfo(userInfo);
            user->saveMyInfoToLocalDatabase();
        }else{
            user->setContactSummaryInfo(userID, userInfo);
            user->saveContactInfoToLocalDatabase(userID);
        }

        emit signalUserInfoPacketReceived(userID, userInfo);

    }
        break;

    case quint8(IM::CONTACT_GROUPS_INFO):
    {

        QByteArray encryptedData;
        in >> encryptedData;

        QByteArray decryptedData;
        cryptography->teaCrypto(&decryptedData, encryptedData, sessionEncryptionKey, false);
        //TODO
        QDataStream stream(&decryptedData, QIODevice::ReadOnly);
        stream.setVersion(QDataStream::Qt_4_8);
        QString groupsInfo = "";
        quint32 personalContactGroupsInfoVersionOnServer = 1;
        stream >> groupsInfo >> personalContactGroupsInfoVersionOnServer;
        
        //TODO: Save the data

        emit signalContactGroupsInfoPacketReceived(groupsInfo, personalContactGroupsInfoVersionOnServer);
        
        qWarning()<<"--CONTACT_GROUPS_INFO";

    }
        break;

    case quint8(IM::SERVER_RESPONSE_SEARCH_CONTACTS):
    {
        //TODO:

        QByteArray encryptedData;
        in >> encryptedData;

        QByteArray decryptedData;
        cryptography->teaCrypto(&decryptedData, encryptedData, sessionEncryptionKey, false);
        //TODO
        QDataStream stream(&decryptedData, QIODevice::ReadOnly);
        stream.setVersion(QDataStream::Qt_4_8);
        QString result = "";
        stream >> result;

        emit signalSearchContactsResultPacketReceived(result.split(QString(PACKET_DATA_SEPARTOR)));

        qWarning()<<"--SERVER_RESPONSE_SEARCH_CONTACTS";
    }
        break;

    case quint8(IM::CLIENT_REQUEST_ADD_CONTACT):
    {

        QByteArray encryptedData;
        in >> encryptedData;

        QByteArray decryptedData;
        cryptography->teaCrypto(&decryptedData, encryptedData, sessionEncryptionKey, false);
        //TODO
        QDataStream stream(&decryptedData, QIODevice::ReadOnly);
        stream.setVersion(QDataStream::Qt_4_8);
        QString userID = "", userNickName = "", userFace = "", verificationMessage = "";
        stream >> userID >> userNickName >> userFace >> verificationMessage;

        //emit signalAddContactResultPacketReceived(contactID, IM::ErrorType(errorTypeCode));
        emit signalAddContactRequestFromUserPacketReceived(userID, userNickName, userFace, verificationMessage);
        
    }
        break;

    case quint8(IM::CLIENT_RESPONSE_ADD_CONTACT_REQUEST):
    {

        QByteArray encryptedData;
        in >> encryptedData;

        QByteArray decryptedData;
        cryptography->teaCrypto(&decryptedData, encryptedData, sessionEncryptionKey, false);
        //TODO
        QDataStream stream(&decryptedData, QIODevice::ReadOnly);
        stream.setVersion(QDataStream::Qt_4_8);
        QString userID = "", userNickName = "", userFace = "", reasonMessage = "";
        quint8 errorTypeCode = quint8(IM::ERROR_UnKnownError);
        
        stream >> userID >> userNickName >> userFace >> errorTypeCode >> reasonMessage;

        //emit signalAddContactResultPacketReceived(contactID, IM::ErrorType(errorTypeCode));
        emit signalAddContactResultPacketReceived(userID, userNickName, userFace, errorTypeCode, reasonMessage);
        
    }
        break;

    case quint8(IM::SERVER_RESPONSE_INTEREST_GROUPS_LIST):
    {
        qDebug()<<"--SERVER_RESPONSE_INTEREST_GROUPS_LIST";


        QByteArray encryptedData;
        in >> encryptedData;

        QByteArray decryptedData;
        cryptography->teaCrypto(&decryptedData, encryptedData, sessionEncryptionKey, false);
        //TODO
        QDataStream stream(&decryptedData, QIODevice::ReadOnly);
        stream.setVersion(QDataStream::Qt_4_8);
        QString interestGroupsListOnServerForUser = "";
        quint32 interestGroupsInfoVersionOnServer = 1;

        stream >> interestGroupsListOnServerForUser >> interestGroupsInfoVersionOnServer;

        emit signalInterestGroupsListPacketReceived(interestGroupsListOnServerForUser, interestGroupsInfoVersionOnServer);

    }
        break;

    case quint8(IM::SERVER_RESPONSE_INTEREST_GROUP_INFO):
    {

        QByteArray encryptedData;
        in >> encryptedData;

        QByteArray decryptedData;
        cryptography->teaCrypto(&decryptedData, encryptedData, sessionEncryptionKey, false);
        //TODO
        QDataStream stream(&decryptedData, QIODevice::ReadOnly);
        stream.setVersion(QDataStream::Qt_4_8);
        quint32 groupID = 0;
        QString interestGroupInfoStringFromServer = "";

        stream >> interestGroupInfoStringFromServer >> groupID;

        emit signalInterestGroupInfoPacketReceived(interestGroupInfoStringFromServer, groupID);

    }
        break;

    case quint8(IM::SERVER_RESPONSE_INTEREST_GROUP_MEMBERS_INFO):
    {
        qWarning()<<"----SERVER_RESPONSE_INTEREST_GROUP_MEMBERS_INFO";


        QByteArray encryptedData;
        in >> encryptedData;

        QByteArray decryptedData;
        cryptography->teaCrypto(&decryptedData, encryptedData, sessionEncryptionKey, false);
        //TODO
        QDataStream stream(&decryptedData, QIODevice::ReadOnly);
        stream.setVersion(QDataStream::Qt_4_8);
        QString interestGroupMembersInfoStringFromServer = "";
        quint32 interestGroupMembersInfoVersionOnServer = 0, groupID = 0;

        stream >> interestGroupMembersInfoStringFromServer >> interestGroupMembersInfoVersionOnServer >> groupID;

        emit signalInterestGroupMembersInfoPacketReceived(interestGroupMembersInfoStringFromServer, interestGroupMembersInfoVersionOnServer, groupID);

    }
        break;

    case quint8(IM::BLACKLIST_INFO):
    {

        QByteArray encryptedData;
        in >> encryptedData;

        QByteArray decryptedData;
        cryptography->teaCrypto(&decryptedData, encryptedData, sessionEncryptionKey, false);
        //TODO
        QDataStream stream(&decryptedData, QIODevice::ReadOnly);
        stream.setVersion(QDataStream::Qt_4_8);
        QString blacklistOnServer = "";
        quint32 blacklistInfoVersionOnServer = 1;

        stream >> blacklistOnServer >> blacklistInfoVersionOnServer;

        emit signalBlacklistInfoPacketReceived(blacklistOnServer, blacklistInfoVersionOnServer);

    }
        break;


    case quint8(IM::SESSION_ENCRYPTION_KEY_WITH_CONTACT):
    {

        QByteArray encryptedData;
        in >> encryptedData;

        QByteArray decryptedData;
        cryptography->teaCrypto(&decryptedData, encryptedData, sessionEncryptionKey, false);
        //TODO
        QDataStream stream(&decryptedData, QIODevice::ReadOnly);
        stream.setVersion(QDataStream::Qt_4_8);
        QString contactID = "";
        QByteArray key;
        stream >> contactID >> key;

        sessionEncryptionKeyWithContactHash[contactID] = key;

    }
        break;

    case quint8(IM::CHAT_MESSAGE_FROM_CONTACT):
    {

        //TODO:

        QByteArray encryptedData;
        QString contactID = peerID;
        in >> encryptedData;

        QByteArray decryptedData;
        cryptography->teaCrypto(&decryptedData, encryptedData, sessionEncryptionKeyWithContactHash.value(contactID), false);
        //TODO
        QDataStream stream(&decryptedData, QIODevice::ReadOnly);
        stream.setVersion(QDataStream::Qt_4_8);

        QString message = "";
        stream  >> message;

        emit signalChatMessageReceivedFromContact(contactID, message, "");

    }
        break;

    case quint8(IM::CHAT_MESSAGES_CACHED_ON_SERVER):
    {
        //TODO:

        QByteArray encryptedData;
        in >> encryptedData;

        QByteArray decryptedData;
        cryptography->teaCrypto(&decryptedData, encryptedData, sessionEncryptionKey, false);
        //TODO
        QDataStream stream(&decryptedData, QIODevice::ReadOnly);
        stream.setVersion(QDataStream::Qt_4_8);
        QString messagesString = "";
        stream >> messagesString;

        emit signalChatMessageCachedOnServerReceived(messagesString.split(QString(UNIT_SEPARTOR)));

    }
        break;

    case quint8(IM::CHAT_IMAGE):
    {
        //TODO:
    }
        break;

    case quint8(IM::GROUP_CHAT_MESSAGES_CACHED_ON_SERVER):
    {
        //TODO:

        QByteArray encryptedData;
        in >> encryptedData;

        QByteArray decryptedData;
        cryptography->teaCrypto(&decryptedData, encryptedData, sessionEncryptionKey, false);
        //TODO
        QDataStream stream(&decryptedData, QIODevice::ReadOnly);
        stream.setVersion(QDataStream::Qt_4_8);
        QString messagesString = "";
        stream >> messagesString;

        emit signalInterestGroupChatMessagesCachedOnServerReceived(messagesString.split(QString(UNIT_SEPARTOR)));

    }
        break;

    case quint8(IM::GROUP_CHAT_MESSAGE):
    {

        QByteArray encryptedData;
        QString contactID = peerID;
        in >> encryptedData;

        QByteArray decryptedData;
        cryptography->teaCrypto(&decryptedData, encryptedData, sessionEncryptionKeyWithContactHash.value(contactID), false);
        //TODO
        QDataStream stream(&decryptedData, QIODevice::ReadOnly);
        stream.setVersion(QDataStream::Qt_4_8);
        quint32 interestGroupID = 0;
        QString message = "";
        stream  >> interestGroupID >> message;

        emit signalInterestGroupChatMessageReceivedFromContact(interestGroupID, contactID, message, "");

    }
        break;


        //File TX
    case quint8(IM::RequestUploadFile):
    {
        QByteArray encryptedData;
        QString contactID = peerID;
        in >> encryptedData;

        QByteArray decryptedData;
        cryptography->teaCrypto(&decryptedData, encryptedData, sessionEncryptionKeyWithContactHash.value(contactID), false);
        //TODO
        QDataStream stream(&decryptedData, QIODevice::ReadOnly);
        stream.setVersion(QDataStream::Qt_4_8);
        QByteArray fileMD5Sum;
        QString fileName = "";
        quint64 size = 0;
        QString localFileSaveDir = "./";
        stream >> fileMD5Sum >> fileName >> size >> localFileSaveDir ;

        emit signalContactRequestUploadFile(socketID, contactID, fileMD5Sum, fileName, size, localFileSaveDir);

        qDebug()<<"~~RequestUploadFile";
    }
        break;
    case quint8(IM::RequestDownloadFile):
    {
        QByteArray encryptedData;
        QString contactID = peerID;
        in >> encryptedData;

        QByteArray decryptedData;
        cryptography->teaCrypto(&decryptedData, encryptedData, sessionEncryptionKeyWithContactHash.value(contactID), false);
        //TODO
        QDataStream stream(&decryptedData, QIODevice::ReadOnly);
        stream.setVersion(QDataStream::Qt_4_8);
        QString localBaseDir, fileName, remoteFileSaveDir;
        stream >> localBaseDir >> fileName >> remoteFileSaveDir;

        emit signalContactRequestDownloadFile(socketID, contactID, localBaseDir, fileName, remoteFileSaveDir);

        qDebug()<<"~~RequestDownloadFile";
    }
        break;
    case quint8(IM::RequestFileData):
    {
        QByteArray encryptedData;
        QString contactID = peerID;
        in >> encryptedData;

        QByteArray decryptedData;
        cryptography->teaCrypto(&decryptedData, encryptedData, sessionEncryptionKeyWithContactHash.value(contactID), false);
        //TODO
        QDataStream stream(&decryptedData, QIODevice::ReadOnly);
        stream.setVersion(QDataStream::Qt_4_8);
        QByteArray fileMD5;
        int startPieceIndex = 0, endPieceIndex = 0;
        in >> fileMD5 >> startPieceIndex >> endPieceIndex;

        emit signalFileDataRequested(socketID, contactID, fileMD5, startPieceIndex, endPieceIndex);

        qDebug()<<"~~RequestFileData";
    }
        break;
    case quint8(IM::FileData):
    {
        QByteArray encryptedData;
        QString contactID = peerID;
        in >> encryptedData;

        QByteArray decryptedData;
        cryptography->teaCrypto(&decryptedData, encryptedData, sessionEncryptionKeyWithContactHash.value(contactID), false);
        //TODO
        QDataStream stream(&decryptedData, QIODevice::ReadOnly);
        stream.setVersion(QDataStream::Qt_4_8);
        QByteArray fileMD5, data, sha1;;
        int pieceIndex = 0;
        stream >> fileMD5 >> pieceIndex >> data >>sha1;

        //        if(data.size() != size || sha1 != QCryptographicHash::hash(data, QCryptographicHash::Sha1)){
        //            qCritical()<<"ERROR! Data Verification Failed!";
        //            requestFileData(socketID, offset, size);
        //            return;
        //        }

        emit signalFileDataReceived(socketID, contactID, fileMD5, pieceIndex, data, sha1);

        //qDebug()<<"~~FileData";
    }
        break;
    case quint8(IM::FileTXStatusChanged):
    {
        QByteArray encryptedData;
        QString contactID = peerID;
        in >> encryptedData;

        QByteArray decryptedData;
        cryptography->teaCrypto(&decryptedData, encryptedData, sessionEncryptionKeyWithContactHash.value(contactID), false);
        //TODO
        QDataStream stream(&decryptedData, QIODevice::ReadOnly);
        stream.setVersion(QDataStream::Qt_4_8);
        QByteArray fileMD5;
        quint8 status;
        stream >> fileMD5 >> status;

        emit signalFileTXStatusChanged(socketID, contactID, fileMD5, status);

        qDebug()<<"~~FileTXStatusChanged";
    }
        break;
    case quint8(IM::FileTXError):
    {
        QByteArray encryptedData;
        QString contactID = peerID;
        in >> encryptedData;

        QByteArray decryptedData;
        cryptography->teaCrypto(&decryptedData, encryptedData, sessionEncryptionKeyWithContactHash.value(contactID), false);
        //TODO
        QDataStream stream(&decryptedData, QIODevice::ReadOnly);
        stream.setVersion(QDataStream::Qt_4_8);
        QByteArray fileMD5;
        quint8 errorCode;
        QString message;
        stream >> fileMD5 >> errorCode >> message;

        emit signalFileTXError(socketID, contactID, fileMD5, errorCode, message);

        qDebug()<<"~~FileTXStatusChanged";
    }
        break;




    default:
        qWarning()<<"Unknown Packet Type:"<<packetType
                 <<" From:"<<peerAddress.toString()
                <<" Port:"<<peerPort;
        break;

    }



}


//void IMClientPacketsParser::startHeartbeat(int interval){
//    if(NULL == heartbeatTimer){
//        heartbeatTimer = new QTimer();
//        heartbeatTimer->setSingleShot(false);
//        heartbeatTimer->setInterval(interval);
//        connect(heartbeatTimer, SIGNAL(timeout()), this, SLOT(sendHeartbeatPacket()));

//    }else{
//        heartbeatTimer->stop();
//        heartbeatTimer->setInterval(interval);
//    }

//    heartbeatTimer->start();


//}

//void IMClientPacketsParser::stopHeartbeat(){
//    if(heartbeatTimer){
//        heartbeatTimer->stop();
//    }

//}

//int IMClientPacketsParser::crypto(QByteArray *destination, const QByteArray &source, bool encrypt){

//    return cryptography->teaCrypto(destination, source, sessionEncryptionKey, encrypt);

//}


quint16 IMClientPacketsParser::getLastReceivedPacketSN(const QString &peerID){
    quint16 lastpacketSN = 0;

    QList< QPair<quint16 /*Packet Serial Number*/, QDateTime/*Received Time*/> > list = m_receivedPacketsHash.values(peerID);
    if(list.isEmpty()){
        return lastpacketSN;
    }

    QDateTime lastpacketTime(QDate(1970, 1, 1));
    for(int i=0; i<list.size(); i++){
        QPair<quint16, QDateTime> pair = list.at(i);
        QDateTime time = pair.second;
        if(time.addSecs(UDP_PACKET_WAITING_FOR_REPLY_TIMEOUT) < QDateTime::currentDateTime()){
            m_receivedPacketsHash.remove(peerID, pair);
        }else{
            if(time > lastpacketTime){
                lastpacketTime = time;
                lastpacketSN = pair.first;
            }
        }
    }
    //    foreach ( QPair<quint16, QDateTime> pair, list) {
    //        QDateTime time = pair.second;
    //        if(time.addSecs(UDP_PACKET_WAITING_FOR_REPLY_TIMEOUT) < QDateTime::currentDateTime()){
    //            m_receivedPacketsHash.remove(peerID, pair);
    //            list.removeOne(pair);
    //        }else{
    //            if(time > lastpacketTime){
    //                lastpacketTime = time;
    //                lastpacketSN = pair.first;
    //            }
    //        }
    //    }

    //TODO:TX Rate

    return lastpacketSN;


}





















} //namespace HEHUI