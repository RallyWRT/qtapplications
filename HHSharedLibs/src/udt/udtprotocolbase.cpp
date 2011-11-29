﻿/*
 * abstractudtsocket.cpp
 *
 *  Created on: 2010-12-15
 *      Author: hehui
 */


#include "udtprotocolbase.h"

#include <QCoreApplication>
#include <QtConcurrentRun>
#include <QDebug>

//#ifndef Q_OS_WIN32
//   #include <unistd.h>
//   #include <cstdlib>
//   #include <cstring>
//   #include <netdb.h>
//#else
//   #include <windows.h>
//   #include <winsock2.h>
//   #include <ws2tcpip.h>
//    //#ifdef LEGACY_WIN32
//     //   #include <wspiapi.h>
//    //#endif
//#endif

//#include <QDateTime>

namespace HEHUI {


const UDTSOCKET UDTProtocolBase::INVALID_UDT_SOCK = UDT::INVALID_SOCK;

UDTProtocolBase::UDTProtocolBase(bool stream, const SocketOptions *options, QObject *parent)
    :QObject(parent), m_stream(stream)
{

    // use this function to initialize the UDT library
    UDT::startup();


    epollID = 0;
    serverSocket = UDT::INVALID_SOCK;

    m_listening = false;
    m_serverAddress = "";
    m_serverPort = 0;

    if(options){
        m_socketOptions = *options;
    }

    //IMPORTANT For Multi-thread
    if(QThreadPool::globalInstance()->maxThreadCount() < MIN_THREAD_COUNT){
        QThreadPool::globalInstance()->setMaxThreadCount(MIN_THREAD_COUNT);
    }

    m_errorMessage = "";


}

UDTProtocolBase::~UDTProtocolBase() {

    //if(m_listening){
    closeUDTProtocol();
    //}

    //UDT::epoll_release(epollID);

    // use this function to release the UDT library
    //UDT::cleanup();

}

//void AbstractUDTSocket::setSocketOption(UDT::SOCKOPT optname, const char* optval, int optlen){

//}

void UDTProtocolBase::setSocketOptions(const SocketOptions *options){

    this->m_socketOptions = *options;
}

UDTProtocolBase::SocketOptions UDTProtocolBase::getSocketOptions() const{

    return m_socketOptions;

}

UDTSOCKET UDTProtocolBase::listen(quint16 port, const QHostAddress &localAddress){
    qDebug()<<"--UDTProtocolBase::listen(...) "<<localAddress.toString()<<":"<<port;

    m_errorMessage = "";

    if(UDT::getsockstate(serverSocket) == LISTENING){
        m_listening = true;
        m_errorMessage = tr("Server is already listenning on %1:%2!").arg(m_serverAddress).arg(m_serverPort);
        qWarning()<<m_errorMessage;
        return UDT::INVALID_SOCK;
    }

    addrinfo hints;
    addrinfo* localAddressInfo;
    memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_INET;
    hints.ai_socktype = m_stream?SOCK_STREAM:SOCK_DGRAM;

    if (0 != getaddrinfo(localAddress.toString().toStdString().c_str(), QString::number(port).toStdString().c_str(), &hints, &localAddressInfo))
        //if (0 != getaddrinfo(NULL, QString::number(port).toStdString().c_str(), &hints, &res))
    {
        m_errorMessage = tr("Failed to start listening! Illegal port number or port is busy!");
        qDebug()<<m_errorMessage;
        //cout << "illegal port number or port is busy.\n" << endl;
        freeaddrinfo(localAddressInfo);
        return UDT::INVALID_SOCK;
    }

    serverSocket = UDT::socket(localAddressInfo->ai_family, localAddressInfo->ai_socktype, localAddressInfo->ai_protocol);

    // UDT Options
    //TODO: setup UDT_MSS
    //UDT::setsockopt(serverSocket, 0, UDT_MSS, &(m_socketOptions.UDT_MSS), sizeof(int));
    UDT::setsockopt(serverSocket, 0, UDT_SNDSYN, &(m_socketOptions.UDT_SNDSYN), sizeof(bool));
    UDT::setsockopt(serverSocket, 0, UDT_RCVSYN, &(m_socketOptions.UDT_RCVSYN), sizeof(bool));
    if(m_socketOptions.UDT_CC){
        //UDT::setsockopt(serv, 0, UDT_CC, new CCCFactory<CUDPBlast>, sizeof(CCCFactory<CUDPBlast>));
        //UDT::setsockopt(serv, 0, UDT_CC, &(m_socketOptions.UDT_CC), sizeof(CCC));
    }
    UDT::setsockopt(serverSocket, 0, UDT_FC, &(m_socketOptions.UDT_FC), sizeof(int));
    UDT::setsockopt(serverSocket, 0, UDT_SNDBUF, &(m_socketOptions.UDT_SNDBUF), sizeof(int));
    UDT::setsockopt(serverSocket, 0, UDT_RCVBUF, &(m_socketOptions.UDT_RCVBUF), sizeof(int));
    UDT::setsockopt(serverSocket, 0, UDP_SNDBUF, &(m_socketOptions.UDP_SNDBUF), sizeof(int));
    UDT::setsockopt(serverSocket, 0, UDP_RCVBUF, &(m_socketOptions.UDP_RCVBUF), sizeof(int));
    UDT::setsockopt(serverSocket, 0, UDT_LINGER, &(m_socketOptions.UDT_LINGER), sizeof(linger));
    UDT::setsockopt(serverSocket, 0, UDT_RENDEZVOUS, &(m_socketOptions.UDT_RENDEZVOUS), sizeof(bool));
    UDT::setsockopt(serverSocket, 0, UDT_SNDTIMEO, &(m_socketOptions.UDT_SNDTIMEO), sizeof(int));
    UDT::setsockopt(serverSocket, 0, UDT_RCVTIMEO, &(m_socketOptions.UDT_RCVTIMEO), sizeof(int));
    UDT::setsockopt(serverSocket, 0, UDT_REUSEADDR, &(m_socketOptions.UDT_REUSEADDR), sizeof(bool));
    UDT::setsockopt(serverSocket, 0, UDT_MAXBW, &(m_socketOptions.UDT_MAXBW), sizeof(int64_t));


//    qDebug()<<"m_socketOptions.UDT_MSS:"<<m_socketOptions.UDT_MSS;
//    qDebug()<<"m_socketOptions.UDT_REUSEADDR:"<<m_socketOptions.UDT_REUSEADDR;
//    qDebug()<<"m_socketOptions.UDT_SNDSYN:"<<m_socketOptions.UDT_SNDSYN;
//    qDebug()<<"m_socketOptions.UDT_RCVSYN:"<<m_socketOptions.UDT_RCVSYN;
//    qDebug()<<"m_socketOptions.UDT_FC:"<<m_socketOptions.UDT_FC;
//    qDebug()<<"m_socketOptions.UDT_SNDBUF:"<<m_socketOptions.UDT_SNDBUF;
//    qDebug()<<"m_socketOptions.UDT_RCVBUF:"<<m_socketOptions.UDT_RCVBUF;
//    qDebug()<<"m_socketOptions.UDP_SNDBUF:"<<m_socketOptions.UDP_SNDBUF;
//    qDebug()<<"m_socketOptions.UDP_RCVBUF:"<<m_socketOptions.UDP_RCVBUF;
//    qDebug()<<"m_socketOptions.UDT_LINGER:"<<m_socketOptions.UDT_LINGER.l_linger;
//    qDebug()<<"m_socketOptions.UDT_RENDEZVOUS:"<<m_socketOptions.UDT_RENDEZVOUS;
//    qDebug()<<"m_socketOptions.UDT_SNDTIMEO:"<<m_socketOptions.UDT_SNDTIMEO;
//    qDebug()<<"m_socketOptions.UDT_RCVTIMEO:"<<m_socketOptions.UDT_RCVTIMEO;
//    qDebug()<<"m_socketOptions.UDT_REUSEADDR:"<<m_socketOptions.UDT_REUSEADDR;
//    qDebug()<<"m_socketOptions.UDT_MAXBW:"<<m_socketOptions.UDT_MAXBW;

//    bool sync = false;
//    UDT::setsockopt(serverSocket, 0, UDT_SNDSYN, &sync, sizeof(bool));
//    UDT::setsockopt(serverSocket, 0, UDT_RCVSYN, &sync, sizeof(bool));


    if (UDT::ERROR == UDT::bind(serverSocket, localAddressInfo->ai_addr, localAddressInfo->ai_addrlen))
    {
        m_errorMessage = tr("Failed to start listening! Bind error! ") + UDT::getlasterror().getErrorMessage();
        qDebug()<<m_errorMessage;
        //cout << "bind: " << UDT::getlasterror().getErrorMessage() << endl;
        freeaddrinfo(localAddressInfo);

        //TODO:Close the socket
        //UDT::close(serverSocket);
        serverSocket = UDT::INVALID_SOCK;
        return UDT::INVALID_SOCK;
    }
    freeaddrinfo(localAddressInfo);


    if (UDT::ERROR == UDT::listen(serverSocket, 10))
    {
        m_errorMessage = tr("Failed to start listening! ") + UDT::getlasterror().getErrorMessage();
        qDebug()<<m_errorMessage;
        //cout << "listen: " << UDT::getlasterror().getErrorMessage() << endl;

        //TODO:Close the socket
        //UDT::close(serverSocket);
        serverSocket = UDT::INVALID_SOCK;
        return UDT::INVALID_SOCK;
    }

    epollID = UDT::epoll_create();
    UDT::epoll_add_usock(epollID, serverSocket);


    //    QtConcurrent::run(this, &UDTProtocolBase::waitForNewConnection, 0);
    //    QtConcurrent::run(this, &UDTProtocolBase::waitForIO, msecWaitForIOTimeout);
    //    QtConcurrent::run(this, &UDTProtocolBase::waitForReading, msecWaitForIOTimeout);
    //    QtConcurrent::run(this, &UDTProtocolBase::waitForWriting, msecWaitForIOTimeout);


    m_listening = true;

    getAddressInfoFromSocket(serverSocket, &m_serverAddress, &m_serverPort, false);
    qDebug()<<QString("Server is ready on %1:%2").arg(m_serverAddress).arg(m_serverPort);


    //Call startWaitingForIO() to wait for IO


    return serverSocket;


}

//
void UDTProtocolBase::startWaitingForIO(int msecWaitForIOTimeout){
    qDebug()<<"--UDTProtocolBase::startWaitingForIO(...) "<<"msecWaitForIOTimeout:"<<msecWaitForIOTimeout;


//    QtConcurrent::run(this, &UDTProtocolBase::waitForIO, msecWaitForIOTimeout);

    QtConcurrent::run(this, &UDTProtocolBase::waitForReading, msecWaitForIOTimeout);
    QtConcurrent::run(this, &UDTProtocolBase::waitForWriting, msecWaitForIOTimeout);


}

void UDTProtocolBase::closeUDTProtocol(){
    qDebug()<<"--UDTProtocolBase::closeUDTProtocol()";


    m_listening = false;
    m_serverAddress = "";
    m_serverPort = 0;

    UDT::epoll_release(epollID);
    epollID = 0;

    qDebug()<<"-------------2";
    //UDT::close(serverSocket);

    // use this function to release the UDT library
    UDT::cleanup();
    qDebug()<<"-------------3";

    foreach (QByteArray *info, m_cachedDataInfoHash) {
        recycleCachedData(info);
    }
    m_cachedDataInfoHash.clear();

    foreach (QByteArray *info, m_unusedCached) {
        delete info;
    }
    m_unusedCached.clear();

    qDebug()<<"-------------4";




}

UDTSOCKET UDTProtocolBase::connectToHost(const QHostAddress &address, quint16 port, bool sync){
    qDebug()<<"--UDTProtocolBase::connectToHost(...)" <<address.toString()<<":"<<port<<" sync:"<<sync;

    m_errorMessage = "";

    if(address.isNull() || address == QHostAddress::Any){
        m_errorMessage = tr("Can not connect to host! Invalid peer address!");
        qDebug()<<m_errorMessage;

        return UDT::INVALID_SOCK;
    }
    if(!port){
        m_errorMessage = tr("Can not connect to host! Invalid peer port!");
        qDebug()<<m_errorMessage;

        return UDT::INVALID_SOCK;
    }

    if(!epollID){
        if(UDT::INVALID_SOCK == listen()){
            return UDT::INVALID_SOCK;
        }
    }
    Q_ASSERT_X(epollID, "epollID", "ERROR! EPOLL Not Initialized!");


    // use this function to initialize the UDT library
    //UDT::startup();

    struct addrinfo hints, *local, *peer;
    memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_INET;
    hints.ai_socktype = m_stream?SOCK_STREAM:SOCK_DGRAM;

    if (0 != getaddrinfo(address.toString().toStdString().c_str(), QString::number(port).toStdString().c_str(), &hints, &peer))
    {
        m_errorMessage = tr("Can not connect to host! Invalid peer address or port!");
        qDebug()<<m_errorMessage;

        freeaddrinfo(peer);
        return UDT::INVALID_SOCK;
    }


    if (0 != getaddrinfo(m_serverAddress.toStdString().c_str(), QString::number(m_serverPort).toStdString().c_str(), &hints, &local))
    {
        m_errorMessage = tr("Can not connect to host! Invalid local address or port!!");
        qDebug()<<m_errorMessage;

        freeaddrinfo(local);
        freeaddrinfo(peer);

        return UDT::INVALID_SOCK;
    }


    UDTSOCKET client = UDT::socket(local->ai_family, local->ai_socktype, local->ai_protocol);


    //non-blocking sending
    //UDT::setsockopt(client, 0, UDT_SNDSYN, new bool(false), sizeof(bool));
    //UDT::setsockopt(client, 0, UDT_SNDBUF, new int(10240000), sizeof(int));
    //UDT::setsockopt(client, 0, UDT_SNDBUF, new int(1024000000), sizeof(int));
//    if(!sync){
//        //non-blocking sending
//        UDT::setsockopt(client, 0, UDT_SNDSYN, &sync, sizeof(bool));
//        //non-blocking receiving
//        UDT::setsockopt(client, 0, UDT_RCVSYN, &sync, sizeof(bool));
//    }

    // for rendezvous connection, enable the code below
    //UDT::setsockopt(client, 0, UDT_RENDEZVOUS, new bool(true), sizeof(bool));
    //UDT::setsockopt(client, 0, UDT_REUSEADDR, new bool(true), sizeof(bool));


    if (UDT::ERROR == UDT::bind(client, local->ai_addr, local->ai_addrlen))
    {
        m_errorMessage = tr("Can not connect to host! Failed to bind!") + UDT::getlasterror().getErrorMessage();
        qDebug()<<m_errorMessage;

        freeaddrinfo(local);
        freeaddrinfo(peer);

        //TODO:Close the socket
        //UDT::close(client);
        return UDT::INVALID_SOCK;
    }
    freeaddrinfo(local);


    UDT::epoll_add_usock(epollID, client);


    if (UDT::ERROR == UDT::connect(client, peer->ai_addr, peer->ai_addrlen))
    {
        m_errorMessage = tr("Can not connect to host! Failed to connect!") + UDT::getlasterror().getErrorMessage();
        qDebug()<<m_errorMessage;

        freeaddrinfo(peer);

        //TODO:Close the socket
        UDT::close(client);
        return UDT::INVALID_SOCK;
    }
    freeaddrinfo(peer);


    //UDT::close(client);
    // use this function to release the UDT library
    //UDT::cleanup();

    return client;

}


void UDTProtocolBase::closeSocket(UDTSOCKET socket){
    qDebug()<<"--UDTProtocolBase::closeSocket(...) "<<"socket:"<<socket;

    UDT::close(socket);
    UDT::epoll_remove_usock(epollID, socket);

    QByteArray *data = m_cachedDataInfoHash.take(socket);
    recycleCachedData(data);

}

bool UDTProtocolBase::sendUDTStreamData(UDTSOCKET socket, const QByteArray *byteArray){
    qDebug()<<"--UDTProtocolBase::sendUDTStreamData(...) " <<"socket:"<<socket;

    m_errorMessage = "";

    Q_ASSERT_X(epollID, "epollID", "ERROR! EPOLL Not Initialized!");
    if(!epollID){
        m_errorMessage = tr("EPOLL Not Initialized!");
        qDebug()<<m_errorMessage;

        return false;
    }

    if(byteArray->size() > MAX_DATA_BLOCK_SIZE){
        m_errorMessage = tr("Data is too large! Max allowed size:%1, current data size:%2").arg(MAX_DATA_BLOCK_SIZE).arg(byteArray->size());
        qDebug()<<m_errorMessage;

        return false;
    }


    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_7);
    out << *byteArray ;

    int size = block.size();
    const char* data = block.constData();

    //    int size = byteArray.size();
    //    const char* data = byteArray.constData();

    int ssize = 0;
    int ss;
    while (ssize < size)
    {
        if (UDT::ERROR == (ss = UDT::send(socket, data + ssize, size - ssize, 0)))
        {
            m_errorMessage = tr("Data sent failed! Data size:%1, Sent size:%2! %3").arg(size).arg(ssize).arg(UDT::getlasterror().getErrorMessage());
            qDebug()<<m_errorMessage;

            int errorCode = UDT::getlasterror().getErrorCode();
            if(errorCode == UDT::ERRORINFO::EASYNCSND){
                continue;
            }
            //break;
            return false;
        }

        ssize += ss;
        //QCoreApplication::processEvents();
    }

//    if (ssize < size){
//        m_errorMessage = tr("Data sent failed! Data size:%1, Sent size:%2! %3").arg(size).arg(ssize).arg(UDT::getlasterror().getErrorMessage());
//        qDebug()<<m_errorMessage;

//        return false;
//    }


    return true;

}

bool UDTProtocolBase::sendUDTMessageData(UDTSOCKET socket, const QByteArray *byteArray, int ttl, bool inorder){
    qDebug()<<"--UDTProtocolBase::sendUDTMessageData(...) " <<" socket:"<<socket;


    Q_ASSERT_X(epollID, "epollID", "ERROR! EPOLL Not Initialized!");
    if(!epollID){
        m_errorMessage = tr("EPOLL Not Initialized!");
        qDebug()<<m_errorMessage;
        return false;
    }


    int size = byteArray->size();
    if(size > MAX_DATA_BLOCK_SIZE){
        m_errorMessage = tr("Data is too large! Max allowed size:%1, current data size:%2").arg(MAX_DATA_BLOCK_SIZE).arg(size);
        qDebug()<<m_errorMessage;
        return false;
    }
    const char* data = byteArray->constData();

    int ss = UDT::sendmsg(socket, data, size, ttl, inorder);

    if (UDT::ERROR == ss || 0 == ss)
    {
        m_errorMessage = tr("Failed to send message data!") + UDT::getlasterror().getErrorMessage();
        qDebug()<<m_errorMessage;

        return false;
    }

    return true;

}

void UDTProtocolBase::waitForIO(int msecTimeout){
    qDebug()<<"--UDTProtocolBase::waitForIO(...) "<<" msecTimeout:"<<msecTimeout<<" Thread Id:"<<QThread::currentThreadId();

    set<UDTSOCKET> readfds, writefds;
    int count = 0;

    while(m_listening){

        //acceptNewConnection();

        count = UDT::epoll_wait(epollID, &readfds, &writefds, msecTimeout);
        if(count > 0){
            //printf("epoll returned %d sockets ready to IO | %d in read set, %d in write set\n", count, readfds.size(), writefds.size());

            for( std::set<UDTSOCKET>::const_iterator it = readfds.begin(); it != readfds.end(); ++it){
                //TODO:Process
                //QtConcurrent::run(this, &UDTProtocolBase::readDataFromSocket, *it);
                readDataFromSocket(*it);
            }
            readfds.clear();

            for( std::set<UDTSOCKET>::const_iterator it = writefds.begin(); it != writefds.end(); ++it){
                //TODO:Process
                //QtConcurrent::run(this, &UDTProtocolBase::writeDataToSocket, *it);
                writeDataToSocket(*it);
            }
            writefds.clear();
        }
        //qDebug()<<"----------------------------------------1"<<QDateTime::currentDateTime().toString("mm:ss:zzz");
        //QCoreApplication::processEvents();

    }





}

void UDTProtocolBase::waitForReading(int msecTimeout){
    qDebug()<<"--UDTProtocolBase::waitForReading(...)";

    set<UDTSOCKET> readfds;
    int count = 0;

    while(m_listening){

        count = UDT::epoll_wait(epollID, &readfds, NULL, msecTimeout);
        if(count > 0){
            printf("epoll returned %d sockets ready to IO | %d in read set\n", count, readfds.size());

            for( std::set<UDTSOCKET>::const_iterator it = readfds.begin(); it != readfds.end(); ++it){
                //TODO:Process
                //QtConcurrent::run(this, &UDTProtocolBase::readDataFromSocket, *it);
                readDataFromSocket(*it);
            }
            //readfds.clear();

        }

        //acceptNewConnection();
    }

}

void UDTProtocolBase::waitForWriting(int msecTimeout){
    qDebug()<<"--UDTProtocolBase::waitForWriting(...)";

    set<UDTSOCKET> writefds;
    int count = 0;

    while(m_listening){

        //acceptNewConnection();

        count = UDT::epoll_wait(epollID, NULL, &writefds, msecTimeout);
        if(count > 0){
            //printf("epoll returned %d sockets ready to IO | %d in write set\n", count, writefds.size());

            for( std::set<UDTSOCKET>::const_iterator it = writefds.begin(); it != writefds.end(); ++it){
                //TODO:Process
                //QtConcurrent::run(this, &UDTProtocolBase::writeDataToSocket, *it);
                writeDataToSocket(*it);
            }
            //writefds.clear();
        }

    }

}

UDTSOCKET UDTProtocolBase::acceptNewConnection(){
    qDebug()<<"--UDTProtocolBase::acceptNewConnection()";

    sockaddr_storage clientaddr;
    int addrlen = sizeof(clientaddr);
    UDTSOCKET peer;

    if (UDT::INVALID_SOCK == (peer = UDT::accept(serverSocket, (sockaddr*)&clientaddr, &addrlen)))
    {
        //qDebug()<<"ERROR! Invalid UDTSOCKET! "<< UDT::getlasterror().getErrorMessage();
        //cout << "accept: " << UDT::getlasterror().getErrorMessage() << endl;

//        int errorCode = UDT::getlasterror().getErrorCode();
//        QString errorMessage = QString(UDT::getlasterror().getErrorMessage());
//        switch(errorCode){
//        case UDT::ERRORINFO::EINVSOCK:
//            //serverSocket is an invalid UDT socket.
//            qDebug()<<"ERROR! "<<errorMessage;
//            break;
//        case UDT::ERRORINFO::ENOLISTEN:
//            //serverSocket is not in the listening state.
//            qDebug()<<"ERROR! "<<errorMessage;
//            break;
//        case UDT::ERRORINFO::ERDVNOSERV:
//            //serverSocket is set up to support rendezvous connection.
//            qDebug()<<errorMessage;
//            break;
//        case UDT::ERRORINFO::EASYNCRCV:
//            //serverSocket is non-blocking (UDT_RCVSYN = false) but there is no connection available.
//            qDebug()<<errorMessage;
//            break;
//        default:
//            qDebug()<<errorMessage;
//            break;
//        }

        return UDT::INVALID_SOCK;

    }

    char peerAddress[NI_MAXHOST];
    char peerPort[NI_MAXSERV];
    getnameinfo((sockaddr *)&clientaddr, addrlen, peerAddress, sizeof(peerAddress), peerPort, sizeof(peerPort), NI_NUMERICHOST|NI_NUMERICSERV);
    cout << "New Connection: " << peerAddress << ":" << peerPort << " UDTSOCKET:"<< peer << endl;


    if(UDT::epoll_add_usock(epollID, peer, NULL) < 0){
        qWarning()<<"ERROR! epoll_add_usock Failed! "<< UDT::getlasterror().getErrorMessage();
        //fprintf(stderr, "epoll_add_usock error\n");
        UDT::close(peer);
        return UDT::INVALID_SOCK;
    }

    emit connected(peer);

    return peer;

}

void UDTProtocolBase::readDataFromSocket(UDTSOCKET socket){
    qDebug()<<"--UDTProtocolBase::readDataFromSocket(..) "<<"socket:"<<socket;

    if(socket == serverSocket){
        acceptNewConnection();
        return;
    }


    int size = MAX_DATA_BLOCK_SIZE + 4;

    QByteArray byteArray;
    byteArray.resize(size);
    //TODO:Size
    char* data = byteArray.data();
    //data = new char[size];


    int receivedSize = 0;

    if(m_stream){
        //while (true)
        //{
        int totalReceivedSize = 0;
        while (totalReceivedSize < size)
        {
            if (UDT::ERROR == (receivedSize = UDT::recv(socket, data + totalReceivedSize, size - totalReceivedSize, 0)))
            {
                qDebug()<<"ERROR! Failed to receive data! "<< UDT::getlasterror().getErrorMessage();
                //cout << "recv:" << UDT::getlasterror().getErrorMessage() << endl;
                break;
            }

            totalReceivedSize += receivedSize;
        }

        if (0 == totalReceivedSize){
            return;
        }

        byteArray.resize(totalReceivedSize);
        processStreamDataAfterReceived(socket, &byteArray);

    }else{
        if (UDT::ERROR == (receivedSize = UDT::recvmsg(socket, data, size)))
        {
            qDebug()<<"ERROR! Failed to receive message! "<<UDT::getlasterror().getErrorMessage();
            //cout << "recvmsg:" << UDT::getlasterror().getErrorMessage() << endl;
            return;
        }

        byteArray.resize(receivedSize);

        messageDataReceived(socket, &byteArray);

    }



    //    delete [] data;



}

void UDTProtocolBase::writeDataToSocket(UDTSOCKET socket){
    //qDebug()<<"--UDTProtocolBase::writeDataToSocket() "<<"socket:"<<socket;

    //return;

    UDTSTATUS status = UDT::getsockstate(socket);

    //qDebug()<<"socket:"<<socket<<" status:"<<status;

    switch(status){
    case INIT: //1
    {
        //qDebug()<<"INIT";
    }
        break;
    case OPENED: //2
    {
        //qDebug()<<"OPENED";
    }
        break;
    case LISTENING: //3
    {
        //qDebug()<<"LISTENING";

    }
        break;
    case CONNECTING: //4
    {
        //qDebug()<<"CONNECTING";
    }
        break;
    case CONNECTED: //5
    {
        //qDebug()<<"CONNECTED";

    }
        break;
    case BROKEN: //6
    {
        //UDT::close(socket);
        UDT::epoll_remove_usock(epollID, socket);

        emit disconnected(socket);
        qDebug()<<"BROKEN";
    }
        break;
    case CLOSING: //7
    {
        //qDebug()<<"CLOSING";
    }
        break;
    case CLOSED: //8
    {

        //UDT::close(socket);
        UDT::epoll_remove_usock(epollID, socket);
        emit disconnected(socket);
        qDebug()<<"CLOSED";
    }
        break;
    case NONEXIST: //9
    {
        //UDT::close(socket);

        UDT::epoll_remove_usock(epollID, socket);
        qDebug()<<"NONEXIST";
    }
        break;
    default:
        break;


    }

    //qDebug();


}

void UDTProtocolBase::processStreamDataAfterReceived(UDTSOCKET socket, QByteArray *byteArray){
    qDebug()<<"--UDTProtocolBase::processStreamDataAfterReceived(...) "<<"socket:"<<socket<<" size:"<<byteArray->size();


    QByteArray *cachedData = m_cachedDataInfoHash.value(socket);
    if(cachedData){
        byteArray->prepend(*cachedData);
        cachedData->clear();
    }


    QDataStream in(byteArray, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_4_7);
    QIODevice *dev = in.device();

    QByteArray temp;
    qint64 offset = 0;

    forever{
        in >> temp;
        if(temp.isEmpty()){
            if(!cachedData){
                cachedData = new QByteArray();
                m_cachedDataInfoHash.insert(socket, cachedData);
            }

            dev->seek(offset);
            cachedData->append(dev->readAll());
            break;

        }else{
            streamDataReceived(socket, &temp);
            offset  = dev->pos();
        }

    }


}

void UDTProtocolBase::recycleCachedData(QByteArray* data){
    if(!data){
        return;
    }
    data->clear();

}

QByteArray * UDTProtocolBase::getCachedData(){

    if(m_unusedCached.isEmpty()){
        return new QByteArray();
    }
    return m_unusedCached.takeFirst();

}

bool UDTProtocolBase::getAddressInfoFromSocket(UDTSOCKET socket, QString *address, quint16 *port, bool getPeerInfo){

    m_errorMessage = "";

    sockaddr clientaddr;
    int addrlen = sizeof(clientaddr);
    int error;
    if(getPeerInfo){
        error = UDT::getpeername(socket, &clientaddr, &addrlen);
    }else{
        error = UDT::getsockname(socket, &clientaddr, &addrlen);
    }
    if(UDT::ERROR == error){
        m_errorMessage = UDT::getlasterror().getErrorMessage();
        qDebug()<<QString("ERROR! Failed to get %1 address info! %2").arg(getPeerInfo?"peer":"local").arg(m_errorMessage);
        return false;
    }

    char clienthost[NI_MAXHOST];
    char clientservice[NI_MAXSERV];
    getnameinfo(&clientaddr, addrlen, clienthost, sizeof(clienthost), clientservice, sizeof(clientservice), NI_NUMERICHOST|NI_NUMERICSERV);
    //cout << "Address Info: " << clienthost << ":" << clientservice << endl;

    if(address){
        *address = QString::fromLocal8Bit(clienthost);
    }
    if(port){
        *port = QString::fromLocal8Bit(clientservice).toUInt();
    }

    return true;

}

UDTSocketStatus UDTProtocolBase::getUDTSocketStatus(UDTSOCKET socket){

    return UDT::getsockstate(socket);

}

bool UDTProtocolBase::isSocketListening(UDTSOCKET socket){
    return UDT::getsockstate(socket) == LISTENING;
}

bool UDTProtocolBase::isSocketConnected(UDTSOCKET socket){
    return UDT::getsockstate(socket) == CONNECTED;
}

bool UDTProtocolBase::isSocketBroken(UDTSOCKET socket){
    return (UDT::getsockstate(socket) == BROKEN) || (UDT::getsockstate(socket) == NONEXIST);
}

QString UDTProtocolBase::getUDTListeningAddress(){
    return m_serverAddress;
}

quint16 UDTProtocolBase::getUDTListeningPort(){
    return m_serverPort;
}

QString UDTProtocolBase::getLastErrorMessage() const{

    return m_errorMessage;

}















}



