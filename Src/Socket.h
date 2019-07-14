#include <netinet/in.h>
#include <sys/socket.h>

#include <string>

class Socket
{
public:
    Socket( const char* destIp, int destPort, int sockType = SOCK_STREAM );
    ~Socket();

    void Create();
    void Config();
    void Bind( const sockaddr_in& addr );
    void Connect();
    void Connect( const sockaddr_in& addr );
    int Send( const char* byteBuf, int bufSize );
    int SendTo( const char* byteBuf, int bufSize, const sockaddr_in& addr );
    int SendMsg( const std::string& hexStr );
    int Recv( char* byteBuf, int recvMax );
    int RecvFrom( char* byteBuf, int recvMax, const sockaddr_in& addr );
    static void PopulateAddr( const char* ipStr, int port, sockaddr_in& addr );

private:
    int _fd;
    int _sockType;
    sockaddr_in _localAddr;
    sockaddr_in _destAddr;
};
