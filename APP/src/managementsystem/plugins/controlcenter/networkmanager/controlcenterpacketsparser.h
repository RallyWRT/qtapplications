/*
 ****************************************************************************
 * controlcenterpacketsparser.h
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
 * Last Modified On: 2010-7-13
 * Last Modified By: 贺辉
 ***************************************************************************
 */

#ifndef CONTROLCENTERPACKETSPARSER_H_
#define CONTROLCENTERPACKETSPARSER_H_



#include <QHostAddress>
#include <QDebug>

#include "../../sharedms/global_shared.h"
#include "resourcesmanagerinstance.h"


#include "HHSharedCore/hcryptography.h"
#include "HHSharedNetwork/hpacketparserbase.h"



namespace HEHUI {


class ControlCenterPacketsParser : public QObject{
    Q_OBJECT
public:
    ControlCenterPacketsParser(UDPServer *udpServer, UDTProtocol *udtProtocol, QObject *parent = 0);
    virtual ~ControlCenterPacketsParser();

public slots:

    void parseIncomingPacketData(Packet *packet);



    bool sendClientLookForServerPacket(const QString &targetAddress = QString(IP_MULTICAST_GROUP_ADDRESS)){
        qDebug()<<"----sendClientLookForServerPacket(...)";

        QHostAddress address = QHostAddress(targetAddress);
        if(address.isNull()){
            address = QHostAddress(QString(IP_MULTICAST_GROUP_ADDRESS));
        }

        Packet *packet = PacketHandlerBase::getPacket();

        packet->setPacketType(quint8(MS::ClientLookForServer));
        packet->setTransmissionProtocol(TP_UDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);
        out << m_localComputerName << m_udpServer->localPort();
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        return m_udpServer->sendUDPDatagram(address, IP_MULTICAST_GROUP_PORT, ba);

    }

    bool sendAdminOnlineStatusChangedPacket(int socketID, const QString &clientName, const QString &adminName, bool online){
        qDebug()<<"----sendAdminOnlineStatusChangedPacket(...)";

        Packet *packet = PacketHandlerBase::getPacket(socketID);

        packet->setPacketType(online?quint8(MS::AdminOnline):quint8(MS::AdminOffline));
        packet->setTransmissionProtocol(TP_UDT);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_localID << clientName << adminName;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        return m_udtProtocol->sendUDTMessageData(socketID, &ba);

    }




    bool sendRequestClientDetailedInfoPacket(int socketID, const QString &computerName, bool rescan){

        Packet *packet = PacketHandlerBase::getPacket(socketID);
        packet->setPacketType(quint8(MS::ClientDetailedInfoRequested));
        packet->setTransmissionProtocol(TP_RUDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_localID << localRUDPListeningPort << computerName << rescan;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        return m_udtProtocol->sendUDTMessageData(socketID, &ba);

    }

    bool sendAdminRequestRemoteConsolePacket(int socketID, const QString &computerName, const QString &applicationPath, const QString &adminID, bool startProcess = true){
        qDebug()<<"----sendServerRequestRemoteConsolePacket(...)";

        Packet *packet = PacketHandlerBase::getPacket(socketID);
        packet->setPacketType(quint8(MS::AdminRequestRemoteConsole));
        packet->setTransmissionProtocol(TP_RUDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_localID << computerName << applicationPath << adminID << startProcess;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        return m_udtProtocol->sendUDTMessageData(socketID, &ba);
    }

    bool sendRemoteConsoleCMDFromAdminPacket(int socketID, const QString &computerName, const QString &command){
        qDebug()<<"----sendRemoteConsoleCMDFromServerPacket(...)";

        Packet *packet = PacketHandlerBase::getPacket(socketID);
        packet->setPacketType(quint8(MS::RemoteConsoleCMDFromAdmin));
        packet->setTransmissionProtocol(TP_RUDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_localID << computerName << command;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        return m_udtProtocol->sendUDTMessageData(socketID, &ba);
    }

    bool sendSetupUSBSDPacket(int socketID, const QString &computerName, const QString &userName, bool enable, bool temporarilyAllowed, const QString &adminName){

        Packet *packet = PacketHandlerBase::getPacket(socketID);
        packet->setPacketType(quint8(MS::SetupUSBSD));
        packet->setTransmissionProtocol(TP_RUDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_localID << computerName << userName << enable << temporarilyAllowed << adminName;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        return m_udtProtocol->sendUDTMessageData(socketID, &ba);
    }

    bool sendSetupProgramesPacket(int socketID, const QString &computerName, const QString &userName, bool enable, bool temporarilyAllowed, const QString &adminName){

        Packet *packet = PacketHandlerBase::getPacket(socketID);
        packet->setPacketType(quint8(MS::SetupProgrames));
        packet->setTransmissionProtocol(TP_RUDP);
        //packet->setRemainingRetransmissionTimes(int(PACKET_RETRANSMISSION_TIMES));
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_localID << computerName << userName << enable << temporarilyAllowed << adminName;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        return m_udtProtocol->sendUDTMessageData(socketID, &ba);
    }

    bool sendShowAdminPacket(int socketID, const QString &computerName, const QString &userName, bool show){

        Packet *packet = PacketHandlerBase::getPacket(socketID);
        packet->setPacketType(quint8(MS::ShowAdmin));
        packet->setTransmissionProtocol(TP_RUDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_localID << computerName << userName << show;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        return m_udtProtocol->sendUDTMessageData(socketID, &ba);
    }

    bool sendModifyAdminGroupUserPacket(int socketID, const QString &computerName, const QString &userName, bool addToAdminGroup, const QString &adminName){

        Packet *packet = PacketHandlerBase::getPacket(socketID);
        packet->setPacketType(quint8(MS::ModifyAdminGroupUser));
        packet->setTransmissionProtocol(TP_RUDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_localID << computerName << userName << addToAdminGroup  << adminName;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        return m_udtProtocol->sendUDTMessageData(socketID, &ba);
    }

    bool sendAdminRequestConnectionToClientPacket(int socketID, const QString &computerName, const QString &users){

        Packet *packet = PacketHandlerBase::getPacket(socketID);
        packet->setPacketType(quint8(MS::AdminRequestConnectionToClient));
        packet->setTransmissionProtocol(TP_RUDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_localID << computerName << users ;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        return m_udtProtocol->sendUDTMessageData(socketID, &ba);
    }
    
    bool sendAdminSearchClientPacket(const QHostAddress &targetAddress, const QString &computerName, const QString &userName, const QString &workgroup, const QString &macAddress, const QString &ipAddress, const QString &osVersion, const QString &adminName){

        Packet *packet = PacketHandlerBase::getPacket();

        packet->setPacketType(quint8(MS::AdminSearchClient));
        packet->setTransmissionProtocol(TP_UDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_localID << localRUDPListeningPort << computerName << userName << workgroup << macAddress << ipAddress << osVersion << adminName;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        return m_udpServer->sendUDPDatagram(targetAddress, quint16(IP_MULTICAST_GROUP_PORT), ba);
    }
    
    bool sendRemoteAssistancePacket(int socketID, const QString &computerName, const QString &adminName){

        Packet *packet = PacketHandlerBase::getPacket(socketID);

        packet->setPacketType(quint8(MS::AdminRequestRemoteAssistance));
        packet->setTransmissionProtocol(TP_RUDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_localID << computerName << adminName;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        return m_udtProtocol->sendUDTMessageData(socketID, &ba);
    }

    bool sendUpdateMSUserPasswordPacket(const QString &peerAddress, quint16 peerPort, const QString &workgroupName, const QString &adminName){

        Packet *packet = PacketHandlerBase::getPacket();
        QHostAddress targetAddress = QHostAddress(peerAddress);
        if(targetAddress.isNull()){
            targetAddress = ipmcGroupAddress;
        }

        packet->setPacketType(quint8(MS::UpdateMSWUserPassword));
        packet->setTransmissionProtocol(TP_UDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_localID << workgroupName << adminName;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        return m_udpServer->sendUDPDatagram(targetAddress, peerPort, ba);

    }
    
    bool sendUpdateMSUserPasswordPacket(int socketID, const QString &workgroupName, const QString &adminName){

        Packet *packet = PacketHandlerBase::getPacket(socketID);
        packet->setPacketType(quint8(MS::UpdateMSWUserPassword));
        packet->setTransmissionProtocol(TP_RUDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_localID << workgroupName << adminName;
        packet->setPacketData(ba);


        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        return m_udtProtocol->sendUDTMessageData(socketID, &ba);

    }


    bool sendInformUpdatePasswordPacket(const QString &peerAddress, quint16 peerPort, const QString &workgroupName, const QString &adminName){

        Packet *packet = PacketHandlerBase::getPacket();
        QHostAddress targetAddress = QHostAddress(peerAddress);
        if(targetAddress.isNull()){
            targetAddress = ipmcGroupAddress;
        }

        packet->setPacketType(quint8(MS::InformUserNewPassword));
        packet->setTransmissionProtocol(TP_UDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_localID << workgroupName <<  adminName  ;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        return m_udpServer->sendUDPDatagram(targetAddress, peerPort, ba);

    }
    
    bool sendInformUpdatePasswordPacket(int socketID, const QString &workgroupName, const QString &adminName){

        Packet *packet = PacketHandlerBase::getPacket(socketID);

        packet->setPacketType(quint8(MS::InformUserNewPassword));
        packet->setTransmissionProtocol(TP_RUDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_localID << workgroupName <<  adminName  ;
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        return m_udtProtocol->sendUDTMessageData(socketID, &ba);

    }


    bool sendAnnouncementPacket(const QString &peerAddress, quint16 peerPort, const QString &groupName, const QString &computerName, quint32 announcementID, const QString &announcement, const QString &adminName, bool mustRead = true){

        Packet *packet = PacketHandlerBase::getPacket();
        QHostAddress targetAddress = QHostAddress(peerAddress);
        if(targetAddress.isNull()){
            targetAddress = ipmcGroupAddress;
        }
        
        packet->setPacketType(quint8(MS::ServerAnnouncement));
        packet->setTransmissionProtocol(TP_UDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_localID << groupName << computerName << announcementID << announcement << adminName << (mustRead?quint8(1):quint8(0));
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        return m_udpServer->sendUDPDatagram(targetAddress, peerPort, ba);

    }

    bool sendAnnouncementPacket(int socketID, const QString &groupName, const QString &computerName, quint32 announcementID, const QString &announcement, const QString &adminName, bool mustRead = true){

        Packet *packet = PacketHandlerBase::getPacket(socketID);

        packet->setPacketType(quint8(MS::ServerAnnouncement));
        packet->setTransmissionProtocol(TP_RUDP);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << m_localID << groupName << computerName << announcementID << announcement << adminName << (mustRead?quint8(1):quint8(0));
        packet->setPacketData(ba);

        ba.clear();
        out.device()->seek(0);
        QVariant v;
        v.setValue(*packet);
        out << v;

        return m_udtProtocol->sendUDTMessageData(socketID, &ba);

    }





signals:
    void  signalHeartbeatPacketReceived(const QString &computerName);
    void  signalConfirmationOfReceiptPacketReceived(quint16 packetSerialNumber1, quint16 packetSerialNumber2);

    //    void signalClientLookForServerPacketReceived(const QHostAddress clientAddress, quint16 clientPort, const QString &clientName);
    void signalServerDeclarePacketReceived(const QString &serverAddress, quint16 serverUDTListeningPort, const QString &serverName, const QString &version, int serverInstanceID);

    //    void signalClientOnlinePacketReceived(const QHostAddress clientAddress, quint16 clientPort, const QString &clientName);
    //    void signalClientOfflinePacketReceived(const QHostAddress clientAddress, quint16 clientPort, const QString &clientName);

    void signalServerOnlinePacketReceived(const QHostAddress serverAddress, quint16 serverPort, const QString &serverName);
    void signalServerOfflinePacketReceived(const QHostAddress serverAddress, quint16 serverPort, const QString &serverName);

    //    void signalAdminLoggedOnToServerRequestPacketReceived(const QHostAddress adminAddress, quint16 adminPort, const QString &adminID);
    //    void signalServerRequestRemoteConsolePacketReceived(const QString &adminID);
    void signalClientResponseRemoteConsoleStatusPacketReceived(const QString &computerName, bool accept, const QString &extraMessage);
    //    void signalRemoteConsoleCMDFromServerPacketReceived(const QString &command);
    void signalRemoteConsoleCMDResultFromClientPacketReceived(const QString &computerName, const QString &result);



    void signalClientResponseClientDetailedInfoPacketReceived(const QString &computerName, const QString &clientInfo);

    void signalClientRequestSoftwareVersionPacketReceived(const QString &softwareName);
    void signalServerResponseSoftwareVersionPacketReceived(const QString &softwareName, const QString &version);

    //    void  signalServerAnnouncementPacketReceived(const QString &groupName, const QString &computerName, const QString &announcement, bool mustRead = true);


    void signalClientResponseAdminConnectionResultPacketReceived(const QString &computerName, bool result, const QString &message);
    void signalClientMessagePacketReceived(const QString &computerName, const QString &message);

    void signalUserResponseRemoteAssistancePacketReceived(const QString &userName, const QString &computerName, bool accept);
    void signalNewPasswordRetrevedByUserPacketReceived(const QString &userName, const QString &computerName);
    
    void signalClientResponseClientSummaryInfoPacketReceived(const QString &computerName, const QString &workgroupName, const QString &networkInfo, const QString &usersInfo, const QString &osInfo, bool usbsdEnabled, bool programesEnabled, const QString &admins, const QString &clientVersion);



public slots:
    //HeartbeatPacket: PacketType+ComputerName+IP
//    void startHeartbeat(int interval = HEARTBEAT_TIMER_INTERVAL);
//    void heartbeat();
//    void confirmPacketReceipt(quint16 packetSerialNumber);

private:
    quint16 getLastReceivedPacketSN(const QString &peerID);

private:
    QHostAddress serverAddress;
    quint16 serverUDTListeningPort;
    QString serverName;

    QHostAddress ipmcGroupAddress;
    quint16 ipmcListeningPort;

    quint16 localRUDPListeningPort;

    QTimer *heartbeatTimer;

    PacketHandlerBase *m_packetHandlerBase;
    //NetworkManagerInstance *networkManager;

    QMutex mutex;

    QString m_localComputerName;
    QString m_localID;

    QMultiHash<QString /*Peer ID*/, QPair<quint16 /*Packet Serial Number*/, QDateTime/*Received Time*/> > m_receivedPacketsHash;

    UDPServer *m_udpServer;
    UDTProtocol *m_udtProtocol;



};

}

#endif /* CONTROLCENTERPACKETSPARSER_H_ */
