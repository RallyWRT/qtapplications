#ifndef UDTPROTOCOL_H
#define UDTPROTOCOL_H

#include "HHSharedNetwork/hpackethandlerbase.h"
#include "HHSharedUDT/hudtprotocolbase.h"


namespace HEHUI {


class UDTProtocol : public UDTProtocolBase
{
    Q_OBJECT
public:
    explicit UDTProtocol(bool stream = true, const SocketOptions *options = 0, QObject *parent = 0);
    
signals:
    void packetReceived(Packet *packet);
    
public slots:

private slots:
    void streamDataReceived(UDTSOCKET socket, QByteArray *data) ;
    void messageDataReceived(UDTSOCKET socket, QByteArray *data) ;

    void convertDataToPacket(UDTSOCKET socket, QByteArray *data);


private:

    QString m_errorMessage;



    
};

} //namespace HEHUI

#endif // UDTPROTOCOL_H