#include "Socket.h"

#include <arpa/inet.h>
#include <unistd.h>
#include <zeroconf.hpp>

#include <iostream>

#define MICE_PORT 7250
#define RECV_BUF_SZ 8192

void SendSourceReadyMsg( const char* destIp )
{
    // SESSION READY message from Vincents laptop.
    std::string sessionReadyMsg = "003B010100001C560069006E00630065006E0074002D004C006100700074006F0070000200021C440300108E0F87AFF5D8084997DAC9208FEA8475";

    Socket miceSock( destIp, MICE_PORT, SOCK_STREAM );
    miceSock.Connect();
    miceSock.SendMsg( sessionReadyMsg );

    // Have to sleep a bit so the meeple pops. Otherwise we disconnect too fast.
    usleep( 1000000 );
}

void PrintLog(Zeroconf::LogLevel level, const std::string& message)
{
    switch (level)
    {
        case Zeroconf::LogLevel::Error:
            std::cout << "E: " << message << std::endl;
            break;
        case Zeroconf::LogLevel::Warning: 
            std::cout << "W: " << message << std::endl;
            break;
    }
}

void* GetInAddr( sockaddr_storage* sa )
{
    if ( sa->ss_family == AF_INET )
        return &reinterpret_cast<sockaddr_in*>(sa)->sin_addr;

    if ( sa->ss_family == AF_INET6 )
        return &reinterpret_cast<sockaddr_in6*>(sa)->sin6_addr;
    
    return nullptr;
}

bool ResponseMatchesIp( Zeroconf::mdns_responce& item, const char* ip )
{
    in_addr ia = reinterpret_cast<sockaddr_in*>( &item.peer )->sin_addr;
    return strcmp( ip, inet_ntoa( ia ) ) == 0;
}

bool ResponseMatchesHostName( Zeroconf::mdns_responce& item, const char* name )
{
    if (!item.records.empty())
    {
        for (size_t j = 0; j < item.records.size(); j++)
        {
            auto& rr = item.records[j];

            if ( 1 == rr.type || 28 == rr.type )
            {
                if ( 0 == strcmp( name, rr.name.c_str() ) )
                    return true;
            }
        }
    }

    return false;
}

bool ResponseMatchesDisplayName( Zeroconf::mdns_responce& item, const char* name )
{
    if (!item.records.empty())
    {
        for (size_t j = 0; j < item.records.size(); j++)
        {
            auto& rr = item.records[j];

            if ( 16 == rr.type || 33 == rr.type )
            {
                if ( 0 == strcmp( name, rr.name.c_str() ) )
                    return true;
            }
        }
    }

    return false;
}

void PrintResult( Zeroconf::mdns_responce& item )
{
    in_addr ia = reinterpret_cast<sockaddr_in*>( &item.peer )->sin_addr;
    std::cout << "Peer: " << inet_ntoa( ia ) << std::endl;

    if ( !item.records.empty() )
    {
        std::cout << "Answers:" << std::endl;
        for ( size_t j = 0; j < item.records.size(); j++ )
        {
            auto& rr = item.records[j];
            std::cout << " " << j;
            std::cout << ": type ";
            switch( rr.type )
            {
                case  1: std::cout << "A"; break;
                case 12: std::cout << "PTR"; break;
                case 16: std::cout << "TXT"; break;
                case 28: std::cout << "AAAA"; break;
                case 33: std::cout << "SRV"; break;
                default: std::cout << rr.type;
            }
            std::cout << ", size " << rr.len;
            std::cout << ", " << rr.name << std::endl;
        }
    }
}


int main( int argc, char** argv )
{
    if ( argc != 4 )
    {
        std::cerr << "Expecting 3 arguments: (1) IP address (2) Pod name (3) hostname"
                  << std::endl;
        return 1;
    }

    const char* ip = argv[1];          // Ex: 192.168.3.103
    const char* displayName = argv[2]; // Ex: VPod
    const char* hostName = argv[3];    // Ex: android-5d0b32ce6ed947cb

    SendSourceReadyMsg( ip );

    static const std::string MdnsQuery = "_display._tcp.local";

    Zeroconf::SetLogCallback( PrintLog );
    std::vector<Zeroconf::mdns_responce> results;
    bool st = Zeroconf::Resolve( MdnsQuery, /*scanTime*/ 3, &results );

    if ( !st )
    {
        std::cout << "MDNS query failed" << std::endl;
    }
    else if ( results.empty() )
    {
        std::cout << "No replies" << std::endl;
    }
    else
    {
        for ( auto& result : results )
        {
            bool matchesIp = ResponseMatchesIp( result, ip );
            bool matchesDisplayName = ResponseMatchesDisplayName( result, displayName );
            bool matchesHostName = ResponseMatchesHostName( result, hostName );

            if ( matchesIp || matchesDisplayName || matchesHostName )
            {
                std::cout << "----------------------------------"  << std::endl;
                PrintResult( result );
                std::cout << std::endl;
                std::cout << "Matches IP: " << matchesIp << std::endl;
                std::cout << "Matches HostName: " << matchesHostName << std::endl;
                std::cout << "Matches DisplayName: " << matchesDisplayName << std::endl;
                std::cout << "----------------------------------"  << std::endl << std::endl;
            }
        }
    }

    return 0;
}
