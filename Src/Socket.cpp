#include "Socket.h"
#include "Utils.h"

#include <exception>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>

void toss( const std::string& ctx, int err )
{
    std::stringstream ss;
    ss << ctx << ": " << strerror( err );
    throw std::runtime_error( ss.str().c_str() );
}

Socket::Socket( const char* destIp, int destPort, int sockType )
  : _fd( -1 )
  , _sockType( sockType )
{
    Create();
    Config();
    PopulateAddr( "0.0.0.0", 0, _localAddr );
    PopulateAddr( destIp, destPort, _destAddr );
    Bind( _localAddr );
}

Socket::~Socket()
{
    ::close( _fd );
}

void Socket::Create()
{
    if ( -1 == ( _fd = ::socket( AF_INET, _sockType, 0 ) ) )
        toss( "Socket creation failed", errno );
}

void Socket::Config()
{
    if ( SOCK_STREAM == _sockType )
    {
        int synRetries = 1; // Send a total of 3 SYN packets => Timeout ~7s
        if ( -1 == setsockopt( _fd, IPPROTO_TCP, TCP_SYNCNT,
                               &synRetries, sizeof(synRetries) ) )
            toss( "Setting timeout sockopt failed", errno );
    }
}

void Socket::Bind( const sockaddr_in& addr )
{
    if ( -1 == ::bind( _fd, (const sockaddr*) &addr, sizeof(addr) ) )
        toss( "Socket bind failed", errno );
}

void Socket::Connect()
{
    Connect( _destAddr );
}

void Socket::Connect( const sockaddr_in& addr )
{
    if ( -1 == ::connect( _fd, (const sockaddr*) &addr, sizeof(addr) ) )
        toss( "Socket connect failed", errno );
}

int Socket::Send( const char* byteBuf, int bufSize )
{
    int bytesSent = 0;

    if ( -1 == ( bytesSent = ::send( _fd, byteBuf, bufSize, 0 ) ) )
        std::cerr << "Socket send failed" << strerror( errno ) << std::endl;

    return bytesSent;
}

int Socket::SendTo( const char* byteBuf, int bufSize, const sockaddr_in& addr )
{
    int bytesSent = 0;

    if ( -1 == ( bytesSent = ::sendto( _fd, byteBuf, bufSize, 0,
                                       (sockaddr*) &addr, sizeof(addr) ) ) )
        std::cerr << "Socket sendto failed" << strerror( errno ) << std::endl;

    return bytesSent;
}

int Socket::SendMsg( const std::string& hexStr )
{
    int len = hexStr.size();

    if ( len <= 0 || len % 2 != 0 )
        toss( "Hex string is shit", EINVAL );

    int hexLen = len / 2;
    std::vector<char> hexBuf( hexLen );

    Util::hexToBin( hexStr.c_str(), &hexBuf[0] );

    return Send( &hexBuf[0], hexLen );
}

int Socket::Recv( char* byteBuf, int recvMax )
{
    int bytesRead = 0;

    if ( -1 == ( bytesRead = ::recv( _fd, byteBuf, recvMax, 0 ) ) )
        std::cerr << "Socket recv failed: " << strerror( errno ) << std::endl;
    else
        Util::printHex( "Server response", (const uint8_t*) byteBuf, bytesRead );

    return bytesRead;
}

int Socket::RecvFrom( char* byteBuf, int recvMax, const sockaddr_in& addr )
{
    int bytesRead = 0;

    socklen_t len = sizeof(addr);
    if ( -1 == ( bytesRead = ::recvfrom( _fd, byteBuf, recvMax, 0,
                                         (sockaddr*) &addr, &len ) ) )
        std::cerr << "Socket recv failed: " << strerror( errno ) << std::endl;
    else
        Util::printHex( "Server response", (const uint8_t*) byteBuf, bytesRead );

    return bytesRead;
}

void Socket::PopulateAddr( const char* ipStr, int port, sockaddr_in& addr )
{
    memset( &addr, 0, sizeof(addr) );
    addr.sin_family = AF_INET;
    addr.sin_port = htons( port );
    if ( 0 ==  inet_aton( ipStr, &addr.sin_addr ) )
        toss( "Socket ip conversion failed", errno );
}

