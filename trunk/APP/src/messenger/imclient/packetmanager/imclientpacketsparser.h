/*
 ****************************************************************************
 * imclientpacketsparser.h
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

#ifndef IMCLIENTPACKETSPARSER_H_
#define IMCLIENTPACKETSPARSER_H_



#include <QHostAddress>
#include <QHostInfo>
#include <QDebug>
#include <QFile>


#include "../sharedim/constants_global_shared.h"
#include "../networkmanager/clientresourcesmanager.h"
#include "imuser.h"


#include "HHSharedCore/hcryptography.h"
#include "HHSharedNetwork/hnetworkutilities.h"



namespace HEHUI {


class IMClientPacketsParser : public QObject{
    Q_OBJECT
public:
    IMClientPacketsParser(ClientResourcesManager *resourcesManager, QObject *parent = 0);
    virtual ~IMClientPacketsParser();



    //    void startHeartbeat(int interval = HEARTBEAT_TIMER_INTERVAL);
    //    void stopHeartbeat();

public slots:
    void parseIncomingPacketData(Packet *packet);



    //    void sendHeartbeatPacket(){
    //        qDebug()<<"----sendHeartbeatPacket()";

    //        if(m_serverAddress.isNull()){
    //            m_serverAddress = QHostAddress::Broadcast;
    //        }

    //        Packet *packet = PacketHandlerBase::getPacket(serverSocketID);

    //        packet->setPacketType(quint8(HEHUI::HeartbeatPacket));
    //        packet->setTransmissionProtocol(TP_RUDP);
    //        QByteArray ba;
    //        QDataStream out(&ba, QIODevice::WriteOnly);
    //        out.setVersion(QDataStream::Qt_4_8);
    //        out << m_myUserID;
    //        packet->setPacketData(ba);
    //        PacketHandlerBase::appendOutgoingPacket(packet);


    //    }


    //    void sendConfirmationOfReceiptPacket(const QHostAddress peerAddress, quint16 peerPort, quint16 packetSerialNumber, const QString &peerID){
    //        qDebug()<<"----sendConfirmationOfReceiptPacket(...)";

    //        Packet *packet = PacketHandlerBase::getPacket(peerAddress, peerPort, localRUDPListeningAddress, localRUDPListeningPort);

    //        packet->setPacketType(quint8(HEHUI::ConfirmationOfReceiptPacket));
    //        QByteArray ba;
    //        QDataStream out(&ba, QIODevice::WriteOnly);
    //        out.setVersion(QDataStream::Qt_4_8);
    //        out << m_myUserID << packetSerialNumber << getLastReceivedPacketSN(peerID);
    //        packet->setPacketData(ba);
    //        PacketHandlerBase::appendOutgoingPacket(packet);

    //    }

    bool sendClientLookForServerPacket(const QHostAddress &targetAddress, quint16 targetPort){
        //qDebug()<<"----sendClientLookForServerPacket(...)";

        Packet *packet = PacketHandlerBase::getPacket();
        
        packet->setPacketType(quint8(IM::ClientLookForServer));
        packet->setTransmissionProtocol(TP_UDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES*3));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_myUserID << QString(APP_VERSION);
        packet->setPacketData(ba);


        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;
        return m_udpServer->sendDatagram(ba, targetAddress, targetPort);

    }

    //    bool requestRegistration(int serverSocketID){

    //        Packet *packet = PacketHandlerBase::getPacket(serverSocketID);
    //        packet->setPacketType(quint8(IM::CLIENT_REQUEST_REGISTRATION));
    //        packet->setTransmissionProtocol(TP_RUDP);
    //        QByteArray ba;
    //        QDataStream out(&ba, QIODevice::WriteOnly);
    //        out.setVersion(QDataStream::Qt_4_8);
    //        out << m_myUserID;
    //        packet->setPacketData(ba);

    //        ba.clear();
    //        out.device()->seek(0);
    //        QVariant v;
    //        v.setValue(*packet);
    //        out << v;
    //        return m_rtp->sendReliableData(serverSocketID, &ba);

    //    }


    bool requestRegistrationServerInfo(int serverSocketID, const QString &clientVersion = APP_VERSION){
        qWarning()<<"--requestRegistrationServerInfo(...)";

        Packet *packet = PacketHandlerBase::getPacket(serverSocketID);
        packet->setPacketType(quint8(IM::CLIENT_REQUEST_REGISTRATION_SERVER_INFO));
        packet->setTransmissionProtocol(TP_RUDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_myUserID << clientVersion;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;
        return m_rtp->sendReliableData(serverSocketID, &ba);

    }

    bool registration(int serverSocketID, const QString &userID, const QString &password){
        qWarning()<<"--registration(...)";

        Packet *packet = PacketHandlerBase::getPacket(serverSocketID);
        packet->setPacketType(quint8(IM::CLIENT_REGISTRATION));
        packet->setTransmissionProtocol(TP_RUDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_myUserID << userID << password;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;
        return m_rtp->sendReliableData(serverSocketID, &ba);

    }

    bool updatePassword(int serverSocketID, const QString &newPassword){
        qDebug()<<"--updatePassword(...)";
        
        QByteArray encryptedNewPassword;
        cryptography->teaCrypto(&encryptedNewPassword, newPassword.toUtf8(), sessionEncryptionKey, true);

        Packet *packet = PacketHandlerBase::getPacket(serverSocketID);
        packet->setPacketType(quint8(IM::CLIENT_REQUEST_UPDATE_PASSWORD));
        packet->setTransmissionProtocol(TP_RUDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_myUserID << encryptedNewPassword;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;
        return m_rtp->sendReliableData(serverSocketID, &ba);

    }

    bool requestLogin(int serverSocketID, const QString &clientVersion = APP_VERSION){
        qDebug()<<"--requestLogin(...)";
        
        m_myUserID = user->getUserID();
        sessionEncryptionKey = cryptography->SHA1(user->getPassword().toUtf8());
        Q_ASSERT_X(!m_myUserID.isEmpty(), "requestLogin", "Invalid User ID!");
        if(m_myUserID.isEmpty()){
            qCritical()<<"Invalid User ID!";
            return false;
        }
        qWarning()<<"m_myUserID:"<<m_myUserID;

        Packet *packet = PacketHandlerBase::getPacket(serverSocketID);

        packet->setPacketType(quint8(IM::CLIENT_REQUEST_LOGIN));
        packet->setTransmissionProtocol(TP_RUDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_myUserID << clientVersion;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;
        return m_rtp->sendReliableData(serverSocketID, &ba);

    }

    bool changeMyOnlineState(int serverSocketID, quint8 onlineStateCode){
        qDebug()<<"--changeMyOnlineState(...)";

        Packet *packet = PacketHandlerBase::getPacket(serverSocketID);
        packet->setPacketType(quint8(IM::ONLINE_STATE_CHANGED));
        packet->setTransmissionProtocol(TP_RUDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        
        out << onlineStateCode;
        QByteArray encryptedData;
        cryptography->teaCrypto(&encryptedData, ba, sessionEncryptionKey, true);
        ba.clear();
        out.device()->seek(0);
        out << m_myUserID << encryptedData;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;
        return m_rtp->sendReliableData(serverSocketID, &ba);

    }

    bool logout(int serverSocketID){
        qDebug()<<"--logout(...)";
        return changeMyOnlineState(serverSocketID, IM::ONLINESTATE_OFFLINE);

    }

    bool requestContactInfo(int serverSocketID, const QString &contactID, bool summaryInfo = true){
        qDebug()<<"--requestContactInfo(...)";
        
        QByteArray encryptedID;
        cryptography->teaCrypto(&encryptedID, contactID.toUtf8(), sessionEncryptionKey, true);

        Packet *packet = PacketHandlerBase::getPacket(serverSocketID);
        packet->setPacketType(quint8(IM::CLIENT_REQUEST_USER_INFO));
        packet->setTransmissionProtocol(TP_RUDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        
        out << contactID << quint8(summaryInfo?1:0);
        QByteArray encryptedData;
        cryptography->teaCrypto(&encryptedData, ba, sessionEncryptionKey, true);
        ba.clear();
        out.device()->seek(0);
        out << m_myUserID << encryptedData;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;
        return m_rtp->sendReliableData(serverSocketID, &ba);

    }

    bool requestPersonalInfo(int serverSocketID){
        qDebug()<<"--requestPersonalInfo(...)";
        
        return requestContactInfo(serverSocketID, m_myUserID);

    }

    bool searchContact(int serverSocketID, const QString &propertiesString, bool matchExactly, bool searchOnlineUsersOnly, bool searchWebcamUsersOnly){
        qDebug()<<"--searchContact(...)";

        Packet *packet = PacketHandlerBase::getPacket(serverSocketID);
        packet->setPacketType(quint8(IM::CLIENT_REQUEST_SEARCH_CONTACTS));
        packet->setTransmissionProtocol(TP_RUDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);

        out << propertiesString << matchExactly << searchOnlineUsersOnly << searchWebcamUsersOnly;
        QByteArray encryptedData;
        cryptography->teaCrypto(&encryptedData, ba, sessionEncryptionKey, true);
        ba.clear();
        out.device()->seek(0);
        out << m_myUserID << encryptedData;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;
        return m_rtp->sendReliableData(serverSocketID, &ba);
    }

    bool addContact(int serverSocketID, const QString &contactID, const QString &verificationMessage = "", quint32 groupID = ContactGroupBase::Group_Friends_ID){
        qWarning()<<"--addContact(...)";
        
        Packet *packet = PacketHandlerBase::getPacket(serverSocketID);
        packet->setPacketType(quint8(IM::CLIENT_REQUEST_ADD_CONTACT));
        packet->setTransmissionProtocol(TP_RUDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);

        out << contactID << verificationMessage << groupID;
        QByteArray encryptedData;
        cryptography->teaCrypto(&encryptedData, ba, sessionEncryptionKey, true);
        ba.clear();
        out.device()->seek(0);
        out << m_myUserID << encryptedData;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;
        return m_rtp->sendReliableData(serverSocketID, &ba);

    }
    
    bool moveContactToGroup(int serverSocketID, const QString &contactID, quint32 oldGroupID, quint32 newGroupID){
        qDebug()<<"--moveContactToGroup(...)";

        Packet *packet = PacketHandlerBase::getPacket(serverSocketID);
        packet->setPacketType(quint8(IM::CLIENT_REQUEST_MOVE_CONTACT));
        packet->setTransmissionProtocol(TP_RUDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);

        out << contactID << oldGroupID << newGroupID;
        QByteArray encryptedData;
        cryptography->teaCrypto(&encryptedData, ba, sessionEncryptionKey, true);
        ba.clear();
        out.device()->seek(0);
        out << m_myUserID << encryptedData;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;
        return m_rtp->sendReliableData(serverSocketID, &ba);

    }

    bool responseAddContactRequestFromUser(int serverSocketID, const QString &userID, bool acceptRequest, const QString &extraMessage = ""){
        qDebug()<<"--responseAddContactRequestFromUser(...)";
        
        Packet *packet = PacketHandlerBase::getPacket(serverSocketID);
        packet->setPacketType(quint8(IM::CLIENT_RESPONSE_ADD_CONTACT_REQUEST));
        packet->setTransmissionProtocol(TP_RUDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);

        out << userID << acceptRequest << extraMessage;
        QByteArray encryptedData;
        cryptography->teaCrypto(&encryptedData, ba, sessionEncryptionKey, true);
        ba.clear();
        out.device()->seek(0);
        out << m_myUserID << encryptedData;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;
        return m_rtp->sendReliableData(serverSocketID, &ba);

    }

    
    
    bool deleteContact(int serverSocketID, const QString &contactID, bool deleteMeFromOpposition = false, bool addToBlacklist = false){
        qDebug()<<"--deleteContact(...)";
        
        Packet *packet = PacketHandlerBase::getPacket(serverSocketID);
        packet->setPacketType(quint8(IM::CLIENT_REQUEST_DELETE_CONTACT));
        packet->setTransmissionProtocol(TP_RUDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);

        out << contactID << quint8(deleteMeFromOpposition?1:0) << quint8(addToBlacklist?1:0);
        QByteArray encryptedData;
        cryptography->teaCrypto(&encryptedData, ba, sessionEncryptionKey, true);
        ba.clear();
        out.device()->seek(0);
        out << m_myUserID << encryptedData;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;
        return m_rtp->sendReliableData(serverSocketID, &ba);

    }

//    bool addOrDeleteBlacklistedContact(int serverSocketID, const QString &contactID,  bool addToBlacklist = true){
//        qDebug()<<"--addOrDeleteBlacklistedContact(...)";

//        Packet *packet = PacketHandlerBase::getPacket(serverSocketID);
//        packet->setPacketType(quint8(IM::CLIENT_REQUEST_ADD_OR_DELETE_BLACKLISTED_CONTACT));
//        packet->setTransmissionProtocol(TP_RUDP);
//        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
//        QByteArray ba;
//        QDataStream out(&ba, QIODevice::WriteOnly);
//        out.setVersion(QDataStream::Qt_4_8);

//        out << contactID << addToBlacklist;
//        QByteArray encryptedData;
//        cryptography->teaCrypto(&encryptedData, ba, sessionEncryptionKey, true);
//        ba.clear();
//        out.device()->seek(0);
//        out << m_myUserID << encryptedData;
//        packet->setPacketData(ba);

//        ba.clear();
//        out.device()->seek(0);
//        QVariant v;
//        v.setValue(*packet);
//        out << v;
//        return m_rtp->sendReliableData(serverSocketID, &ba);

//    }

    bool requestInterestGroupsList(int serverSocketID){
        qDebug()<<"------------------requestInterestGroupsList(...)";

        Packet *packet = PacketHandlerBase::getPacket(serverSocketID);
        packet->setPacketType(quint8(IM::CLIENT_REQUEST_INTEREST_GROUPS_LIST));
        packet->setTransmissionProtocol(TP_RUDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_myUserID;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;
        return m_rtp->sendReliableData(serverSocketID, &ba);
    }

    bool requestInterestGroupInfo(int serverSocketID, quint32 groupID){

        Packet *packet = PacketHandlerBase::getPacket(serverSocketID);
        packet->setPacketType(quint8(IM::CLIENT_REQUEST_INTEREST_GROUP_INFO));
        packet->setTransmissionProtocol(TP_RUDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);

        //out << m_myUserID << groupID;
        out << groupID;
        QByteArray encryptedData;
        cryptography->teaCrypto(&encryptedData, ba, sessionEncryptionKey, true);
        ba.clear();
        out.device()->seek(0);
        out << m_myUserID << encryptedData;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;
        return m_rtp->sendReliableData(serverSocketID, &ba);

    }

    bool requestInterestGroupMembersInfo(int serverSocketID, quint32 groupID){
        qDebug()<<"----requestInterestGroupMembersInfo(...)";

        Packet *packet = PacketHandlerBase::getPacket(serverSocketID);
        packet->setPacketType(quint8(IM::CLIENT_REQUEST_INTEREST_GROUP_MEMBERS_INFO));
        packet->setTransmissionProtocol(TP_RUDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);

        //out << m_myUserID << groupID;
        out << groupID;
        QByteArray encryptedData;
        cryptography->teaCrypto(&encryptedData, ba, sessionEncryptionKey, true);
        ba.clear();
        out.device()->seek(0);
        out << m_myUserID << encryptedData;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;
        return m_rtp->sendReliableData(serverSocketID, &ba);

    }


    bool requestPersonalMessage(int serverSocketID, const QString &userID){
        qDebug()<<"--requestPersonalMessage(...)";

        Packet *packet = PacketHandlerBase::getPacket(serverSocketID);
        packet->setPacketType(quint8(IM::CLIENT_REQUEST_PERSONAL_MESSAGE_INFO));
        packet->setTransmissionProtocol(TP_RUDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);

        out << userID;
        QByteArray encryptedData;
        cryptography->teaCrypto(&encryptedData, ba, sessionEncryptionKey, true);
        ba.clear();
        out.device()->seek(0);
        out << m_myUserID << encryptedData;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;
        return m_rtp->sendReliableData(serverSocketID, &ba);
    }

    bool updateContactRemarkInfo(int serverSocketID, quint32 contactID, const QString &remarkName, const QString &extraRemarkInfo){
        qDebug()<<"--updateContactRemarkInfo(...)";
        
        Packet *packet = PacketHandlerBase::getPacket(serverSocketID);
        packet->setPacketType(quint8(IM::CLIENT_REQUEST_MODIFY_CONTACT_REMARK));
        packet->setTransmissionProtocol(TP_RUDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);

        out << contactID << remarkName << extraRemarkInfo;
        QByteArray encryptedData;
        cryptography->teaCrypto(&encryptedData, ba, sessionEncryptionKey, true);
        ba.clear();
        out.device()->seek(0);
        out << m_myUserID << encryptedData;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;
        return m_rtp->sendReliableData(serverSocketID, &ba);

    }

    bool requestPersonalContactGroupsInfo(int serverSocketID){
        //qWarning()<<"--requestPersonalContactGroupsInfo(...)";

        Packet *packet = PacketHandlerBase::getPacket(serverSocketID);
        packet->setPacketType(quint8(IM::CONTACT_GROUPS_INFO));
        packet->setTransmissionProtocol(TP_RUDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);

        out << m_myUserID;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;
        return m_rtp->sendReliableData(serverSocketID, &ba);

    }

    bool renameContactGroup(int serverSocketID, quint32 groupID, const QString &newGroupName){
        qDebug()<<"--renameContactGroup(...)";

        Packet *packet = PacketHandlerBase::getPacket(serverSocketID);
        packet->setPacketType(quint8(IM::RENAME_CONTACT_GROUP));
        packet->setTransmissionProtocol(TP_RUDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);

        out << groupID << newGroupName;
        QByteArray encryptedData;
        cryptography->teaCrypto(&encryptedData, ba, sessionEncryptionKey, true);
        ba.clear();
        out.device()->seek(0);
        out << m_myUserID << encryptedData;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;
        return m_rtp->sendReliableData(serverSocketID, &ba);

    }

    bool createOrDeleteContactGroup(int serverSocketID, quint32 groupID, const QString &groupName, bool create = true){
        qDebug()<<"--createOrDeleteContactGroup(...)";

        Packet *packet = PacketHandlerBase::getPacket(serverSocketID);
        packet->setPacketType(quint8(IM::CREATE_OR_DELETE_CONTACT_GROUP));
        packet->setTransmissionProtocol(TP_RUDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);

        out << groupID << groupName << quint8(create?1:0);
        QByteArray encryptedData;
        cryptography->teaCrypto(&encryptedData, ba, sessionEncryptionKey, true);
        ba.clear();
        out.device()->seek(0);
        out << m_myUserID << encryptedData;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;
        return m_rtp->sendReliableData(serverSocketID, &ba);

    }

    bool requestSessionEncryptionKeyWithContact(int serverSocketID, quint32 contactID){
        qDebug()<<"--requestSessionEncryptionKeyWithContact(...)";
        
        Packet *packet = PacketHandlerBase::getPacket(serverSocketID);
        packet->setPacketType(quint8(IM::SESSION_ENCRYPTION_KEY_WITH_CONTACT));
        packet->setTransmissionProtocol(TP_RUDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);

        out << contactID;
        QByteArray encryptedData;
        cryptography->teaCrypto(&encryptedData, ba, sessionEncryptionKey, true);
        ba.clear();
        out.device()->seek(0);
        out << m_myUserID << encryptedData;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;
        return m_rtp->sendReliableData(serverSocketID, &ba);

    }

    bool sendChatMessageToContact(int peerSocketID, const QString &contactID, const QString &message, const QString &contactHostAddress, quint16 contactHostPort){
        qDebug()<<"--sendChatMessageToContact(...)";
        
        //        QHostAddress targetHostAddress = QHostAddress(contactHostAddress);
        //        quint16 targetHostPort = contactHostPort;
        //TODO:判断在线状态及加密KEY

        Packet *packet = PacketHandlerBase::getPacket(peerSocketID);
        packet->setPacketType(quint8(IM::CHAT_MESSAGE_FROM_CONTACT));
        packet->setTransmissionProtocol(TP_RUDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);

        out << message;
        QByteArray encryptedData;
        cryptography->teaCrypto(&encryptedData, ba, sessionEncryptionKeyWithContactHash.value(contactID), true);
        ba.clear();
        out.device()->seek(0);
        out << m_myUserID << encryptedData;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;
        return m_rtp->sendReliableData(peerSocketID, &ba);
    }

    bool sendChatMessageToServer(int serverSocketID, const QString &contactID, const QString &message){
        qDebug()<<"--sendChatMessageToServer(...)";

        Packet *packet = PacketHandlerBase::getPacket(serverSocketID);
        packet->setPacketType(quint8(IM::CHAT_MESSAGES_CACHED_ON_SERVER));
        packet->setTransmissionProtocol(TP_RUDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);

        out << contactID << message;
        QByteArray encryptedData;
        cryptography->teaCrypto(&encryptedData, ba, sessionEncryptionKey, true);
        ba.clear();
        out.device()->seek(0);
        out << m_myUserID << encryptedData;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;
        return m_rtp->sendReliableData(serverSocketID, &ba);
    }

    bool sendChatMessageToContact(int peerSocketID, const QString &contactID, const QString &message, const QStringList &imagePathList, const QString &contactHostAddress, quint16 contactHostPort){
        
        bool ok = sendChatMessageToContact(peerSocketID, contactID, message, contactHostAddress, contactHostPort);
        sendImageFileToContact(peerSocketID, contactID, imagePathList, contactHostAddress, contactHostPort);
        return ok;
    }


    bool sendInterestGroupChatMessageToContact(int peerSocketID, const QString &contactID, quint32 interestGroupID ,const QString &message, const QString &contactHostAddress, quint16 contactHostPort){
        qDebug()<<"--sendInterestGroupChatMessageToContact(...)";

        Packet *packet = PacketHandlerBase::getPacket(peerSocketID);
        packet->setPacketType(quint8(IM::GROUP_CHAT_MESSAGE));
        packet->setTransmissionProtocol(TP_RUDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);

        out << interestGroupID << message;
        QByteArray encryptedData;
        cryptography->teaCrypto(&encryptedData, ba, sessionEncryptionKeyWithContactHash.value(contactID), true);
        ba.clear();
        out.device()->seek(0);
        out << m_myUserID << encryptedData;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;
        return m_rtp->sendReliableData(peerSocketID, &ba);
    }

    bool sendInterestGroupChatMessageToServer(int serverSocketID, quint32 interestGroupID ,const QString &message){
        qDebug()<<"--sendInterestGroupChatMessageToServer(...)";

        Packet *packet = PacketHandlerBase::getPacket(serverSocketID);
        packet->setPacketType(quint8(IM::GROUP_CHAT_MESSAGE));
        packet->setTransmissionProtocol(TP_RUDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);

        out << interestGroupID << message;
        QByteArray encryptedData;
        cryptography->teaCrypto(&encryptedData, ba, sessionEncryptionKey, true);
        ba.clear();
        out.device()->seek(0);
        out << m_myUserID << encryptedData;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;
        return m_rtp->sendReliableData(serverSocketID, &ba);
    }


    bool sendImageFileToContact2(int peerSocketID, const QString &contactID, const QStringList &filePathList, const QString &contactHostAddress, quint16 contactHostPort){
        //TODO:
        return true;
    }
    bool sendImageToContact(int peerSocketID, const QString &contactID, const QString &imageName, const QByteArray &image){
        qDebug()<<"--sendImageToContact(...)";


        Packet *packet = PacketHandlerBase::getPacket(peerSocketID);
        packet->setPacketType(quint8(IM::CHAT_IMAGE));
        packet->setTransmissionProtocol(TP_RUDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);

        out << imageName << image;
        QByteArray encryptedData;
        cryptography->teaCrypto(&encryptedData, ba, sessionEncryptionKeyWithContactHash.value(contactID), true);
        ba.clear();
        out.device()->seek(0);
        out << m_myUserID << encryptedData;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;
        return m_rtp->sendReliableData(peerSocketID, &ba);
    }


    bool requestSendFileToContact(int peerSocketID, const QString &contactID, const QStringList &filePathList){
        //TODO:
        return true;
    }

    bool sendChatMessageToContacts(const QStringList &contacts, const QString &message, const QStringList &imagePathList){
        //TODO:
        return true;
    }

    //    quint16 sendChatMessageToInterestGroup(quint32 interestGroupID, const QString &message, const QStringList &imagePathList){
    //        //TODO:
    //        return 0;
    //    }
    

    ///////////////////////////////////////////////
    bool requestUploadFile(int socketID, const QString &contactID, const QByteArray &fileMD5Sum, const QString &fileName, quint64 size, const QString &remoteFileSaveDir = ""){
        Packet *packet = PacketHandlerBase::getPacket(socketID);

        packet->setPacketType(quint8(IM::RequestUploadFile));
        packet->setTransmissionProtocol(TP_UDT);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << fileMD5Sum << fileName << size << remoteFileSaveDir;

        QByteArray encryptedData;
        cryptography->teaCrypto(&encryptedData, ba, sessionEncryptionKeyWithContactHash.value(contactID), true);
        ba.clear();
        out.device()->seek(0);
        out << m_myUserID << encryptedData;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool requestDownloadFile(int socketID, const QString &contactID, const QString &remoteBaseDir, const QString &remoteFileName, const QString &localFileSaveDir){
        Packet *packet = PacketHandlerBase::getPacket(socketID);

        packet->setPacketType(quint8(IM::RequestDownloadFile));
        packet->setTransmissionProtocol(TP_UDT);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << remoteBaseDir << remoteFileName << localFileSaveDir;

        QByteArray encryptedData;
        cryptography->teaCrypto(&encryptedData, ba, sessionEncryptionKeyWithContactHash.value(contactID), true);
        ba.clear();
        out.device()->seek(0);
        out << m_myUserID << encryptedData;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool acceptFileDownloadRequest(int socketID, const QString &contactID, const QString &fileName, bool accepted, const QByteArray &fileMD5Sum, quint64 size, const QString &remoteFileSaveDir){
        Packet *packet = PacketHandlerBase::getPacket(socketID);

        packet->setPacketType(quint8(IM::ResponseFileDownloadRequest));
        packet->setTransmissionProtocol(TP_UDT);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << fileName << accepted << fileMD5Sum << size << remoteFileSaveDir;

        QByteArray encryptedData;
        cryptography->teaCrypto(&encryptedData, ba, sessionEncryptionKeyWithContactHash.value(contactID), true);
        ba.clear();
        out.device()->seek(0);
        out << m_myUserID << encryptedData;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        return m_rtp->sendReliableData(socketID, &ba);
    }
    bool denyFileDownloadRequest(int socketID, const QString &contactID, const QString &fileName, bool accepted, const QString &message){
        Packet *packet = PacketHandlerBase::getPacket(socketID);

        packet->setPacketType(quint8(IM::ResponseFileDownloadRequest));
        packet->setTransmissionProtocol(TP_UDT);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << fileName << accepted << message;

        QByteArray encryptedData;
        cryptography->teaCrypto(&encryptedData, ba, sessionEncryptionKeyWithContactHash.value(contactID), true);
        ba.clear();
        out.device()->seek(0);
        out << m_myUserID << encryptedData;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool responseFileUploadRequest(int socketID, const QString &contactID, const QByteArray &fileMD5Sum, bool accepted, const QString &message){
        Packet *packet = PacketHandlerBase::getPacket(socketID);

        packet->setPacketType(quint8(IM::ResponseFileUploadRequest));
        packet->setTransmissionProtocol(TP_UDT);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << fileMD5Sum << accepted << message;

        QByteArray encryptedData;
        cryptography->teaCrypto(&encryptedData, ba, sessionEncryptionKeyWithContactHash.value(contactID), true);
        ba.clear();
        out.device()->seek(0);
        out << m_myUserID << encryptedData;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool requestFileData(int socketID, const QString &contactID, const QByteArray &fileMD5, int startPieceIndex, int endPieceIndex){
        qDebug()<<"--requestFileData(...) "<<" startPieceIndex:"<<startPieceIndex<<" endPieceIndex:"<<endPieceIndex;

        Packet *packet = PacketHandlerBase::getPacket(socketID);

        packet->setPacketType(quint8(IM::RequestFileData));
        packet->setTransmissionProtocol(TP_UDT);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << fileMD5 << startPieceIndex << endPieceIndex;

        QByteArray encryptedData;
        cryptography->teaCrypto(&encryptedData, ba, sessionEncryptionKeyWithContactHash.value(contactID), true);
        ba.clear();
        out.device()->seek(0);
        out << m_myUserID << encryptedData;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool sendFileData(int socketID, const QString &contactID, const QByteArray &fileMD5, int pieceIndex, const QByteArray *data, const QByteArray *sha1){
        Packet *packet = PacketHandlerBase::getPacket(socketID);

        packet->setPacketType(quint8(IM::FileData));
        packet->setTransmissionProtocol(TP_UDT);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << fileMD5 << pieceIndex << *data << *sha1 ;

        QByteArray encryptedData;
        cryptography->teaCrypto(&encryptedData, ba, sessionEncryptionKeyWithContactHash.value(contactID), true);
        ba.clear();
        out.device()->seek(0);
        out << m_myUserID << encryptedData;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool fileTXStatusChanged(int socketID, const QString &contactID, const QByteArray &fileMD5, quint8 status){
        Packet *packet = PacketHandlerBase::getPacket(socketID);

        packet->setPacketType(quint8(IM::FileTXStatusChanged));
        packet->setTransmissionProtocol(TP_UDT);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << fileMD5 << status ;

        QByteArray encryptedData;
        cryptography->teaCrypto(&encryptedData, ba, sessionEncryptionKeyWithContactHash.value(contactID), true);
        ba.clear();
        out.device()->seek(0);
        out << m_myUserID << encryptedData;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        return m_rtp->sendReliableData(socketID, &ba);
    }

    bool fileTXError(int socketID, const QString &contactID, const QByteArray &fileMD5, quint8 errorCode, const QString &errorString){
        Packet *packet = PacketHandlerBase::getPacket(socketID);

        packet->setPacketType(quint8(IM::FileTXError));
        packet->setTransmissionProtocol(TP_UDT);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << fileMD5 << errorCode << errorString ;

        QByteArray encryptedData;
        cryptography->teaCrypto(&encryptedData, ba, sessionEncryptionKeyWithContactHash.value(contactID), true);
        ba.clear();
        out.device()->seek(0);
        out << m_myUserID << encryptedData;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        return m_rtp->sendReliableData(socketID, &ba);
    }



private slots:


    //    int crypto(QByteArray *destination, const QByteArray &source, bool encrypt);

    ///////////////////////////////////////////////
    //quint16 login(const QString &serverAddress, quint16 serverPort){
    bool login(int serverSocketID){
        //qWarning()<<"--login() "<<m_serverAddress.toString()<<":"<<m_serverUDPListeningPort;

        //TODO:Device Info
        QString deviceInfo = "PC";

        //TODO:密码保存方式
        sessionEncryptionKey = user->encryptedPassword();
        QByteArray encryptedPassword;
        cryptography->teaCrypto(&encryptedPassword, QByteArray::fromBase64(user->getPassword().toUtf8()), sessionEncryptionKey, true);
        
        Packet *packet = PacketHandlerBase::getPacket(serverSocketID);
        packet->setPacketType(quint8(IM::CLIENT_LOGIN_INFO));
        packet->setTransmissionProtocol(TP_RUDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << m_myUserID << encryptedPassword << quint8(user->getStateAfterLoggedin()) << deviceInfo ;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;
        return m_rtp->sendReliableData(serverSocketID, &ba);

    }



    bool requestPersonalContacsInfoVersion(int serverSocketID){
        //qWarning()<<"--requestPersonalContacsVersionInfo(...)";

        Packet *packet = PacketHandlerBase::getPacket(serverSocketID);
        packet->setPacketType(quint8(IM::CONTACTS_INFO_VERSION));
        packet->setTransmissionProtocol(TP_RUDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);

        out << m_myUserID;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;
        return m_rtp->sendReliableData(serverSocketID, &ba);

    }




private slots:



signals:
    void  signalHeartbeatPacketReceived(const QString &contactID);
    void  signalConfirmationOfReceiptPacketReceived(quint16 packetSerialNumber1, quint16 packetSerialNumber2);

    void signalRegistrationServerInfoReceived(quint8 errorTypeCode, bool canRegister, const QString &extraMessage, quint8 regMode, const QString &regServerAddress, bool requireActivation);
    void signalRegistrationResultReceived(quint8 errorTypeCode, quint32 sysID, const QString &message);
    void signalUpdatePasswordResultReceived(quint8 errorTypeCode, const QString &message);
    
    void signalLoginServerRedirected(const QString &serverAddress, quint16 serverPort, const QString &serverName);
    void signalLoginResultReceived(quint8 errorType);
    void signalClientLastLoginInfoPacketReceived(const QString &extIPAddress, const QString &loginTime, const QString &LogoutTime, const QString &deviceInfo);
    //void signalContactStateChangedPacketReceived(const QString &contactID, IM::OnlineState onlineState, const QString &contactHostAddress, quint16 contactHostPort);
    void signalContactStateChangedPacketReceived(const QString &contactID, quint8 onlineState, const QString &contactHostAddress, quint16 contactHostPort);
    void signalContactsOnlineInfoPacketReceived(const QString &contactsOnlineInfoString);
    void signalUserInfoPacketReceived(const QString &userID/*, const QString &userInfo*/);
    void signalContactGroupsInfoPacketReceived(const QString &contactGroupsInfo, quint32 personalContactGroupsInfoVersionOnServer);
    void signalContactsInfoVersionPacketReceived(const QString &contactsInfoVersionString, quint32 contactGroupsInfoVersionOnServer);
    void signalCreateOrDeleteContactGroupResultPacketReceived(quint32 groupID, const QString &groupName, bool createGroup, bool result);

    void signalSearchContactsResultPacketReceived(const QStringList &users);
    //void signalAddContactResultPacketReceived(const QString &contactID, IM::ErrorType errorType);
    void signalAddContactRequestFromUserPacketReceived(const QString &userID, const QString &userNickName, const QString &userFace, const QString &verificationMessage);
    void signalAddContactResultPacketReceived(const QString &userID, const QString &userNickName, const QString &userFace, quint8 errorTypeCode, const QString &reasonMessage);

    void signalDeleteContactResultPacketReceived(const QString &contactID, bool contactDeleted, bool addToBlacklist = false);


    void signalInterestGroupsListPacketReceived(const QString &systemGroupsListFromServer, quint32 systemInfoVersionOnServer);
    void signalInterestGroupInfoPacketReceived(const QString &interestGroupInfoFromServer, quint32 groupID);
    void signalInterestGroupMembersInfoPacketReceived(const QString &interestGroupMembersInfoFromServer, quint32 interestGroupMembersInfoVersionOnServer, quint32 groupID);
    
    
    void signalPersonalMessagePacketReceived(const QString &userID, const QString &personalMessage);

    void signalChatMessageReceivedFromContact(const QString &contactID, const QString &message, const QString &time);
    void signalChatMessageCachedOnServerReceived(const QStringList &messages);
    void signalImageDownloadResultReceived(const QString &contactID, const QString &imageName, const QByteArray &image);

    void signalInterestGroupChatMessageReceivedFromContact(quint32 interestGroupID, const QString &contactID, const QString &message, const QString &time);
    void signalInterestGroupChatMessagesCachedOnServerReceived(const QStringList &messages);






    void contactGroupInfoPacketReceived();

    void interestGroupsListPacketReceived();
    void interestGroupInfoPacketReceived();

    void systemGroupsListPacketReceived();
    void systemGroupInfoPacketReceived();

    void signalServerResponseAddContactPacketReceived();
    void signalServerResponseDeleteContactPacketReceived();
    void signalServerResponseMoveContactPacketReceived();
    void signalServerResponseModifyContactRemarkPacketReceived();

    void signalClientSoftwareUpdatePacketReceived();

    void signalServerDeclarePacketReceived(const QString &m_serverAddress, quint16 serverRUDPListeningPort, const QString &serverName, const QString &version);
    void signalServerOnlinePacketReceived(const QString m_serverAddress, quint16 serverRUDPListeningPort, const QString &serverName);
    void signalServerOfflinePacketReceived(const QString m_serverAddress, quint16 serverRUDPListeningPort, const QString &serverName);
    void signalServerAnnouncementPacketReceived(const QString &announcement, bool mustRead = true);


    ///////////////////////////
    //File TX
    void signalContactRequestUploadFile(int socketID, const QString &contactID, const QByteArray &fileMD5Sum, const QString &fileName, quint64 size, const QString &localFileSaveDir);
    void signalContactRequestDownloadFile(int socketID, const QString &contactID, const QString &localBaseDir, const QString &fileName, const QString &remoteFileSaveDir);
    void signalFileDataRequested(int socketID, const QString &contactID, const QByteArray &fileMD5, int startPieceIndex, int endPieceIndex);
    void signalFileDataReceived(int socketID, const QString &contactID, const QByteArray &fileMD5, int pieceIndex, const QByteArray &data, const QByteArray &sha1);
    void signalFileTXStatusChanged(int socketID, const QString &contactID, const QByteArray &fileMD5, quint8 status);
    void signalFileTXError(int socketID, const QString &contactID, const QByteArray &fileMD5, quint8 errorCode, const QString &errorString);



private:
    quint16 getLastReceivedPacketSN(const QString &peerID);

private:


    QTimer *heartbeatTimer;
    //        QTimer *processWaitingForReplyPacketsTimer;

    //ClientNetworkManager *m_networkManager;
    //PacketHandlerBase *m_packetHandlerBase;


    QDateTime serverLastOnlineTime;

    QMutex mutex;

    IMUser *user;
    QString m_myUserID;

    Cryptography *cryptography;
    QByteArray sessionEncryptionKey;

    QHash<QString/*Contact's ID*/, QByteArray/*Session Encryption Key*/> sessionEncryptionKeyWithContactHash;


    QMultiHash<QString /*Peer ID*/, QPair<quint16 /*Packet Serial Number*/, QDateTime/*Received Time*/> > m_receivedPacketsHash;




    UDPServer *m_udpServer;
    RTP *m_rtp;
    UDTProtocol *m_udtProtocol;
    TCPServer *m_tcpServer;



};

}

#endif /* IMCLIENTPACKETSPARSER_H_ */
