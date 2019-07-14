#include "Utils.h"

#include <cstring>
#include <iostream>
#include <string>
#include <vector>

namespace Util
{

int charToInt( char input )
{
    if ( input >= '0' && input <= '9' )
        return input - '0';
    if ( input >= 'A' && input <= 'F' )
        return input - 'A' + 10;
    if ( input >= 'a' && input <= 'f' )
        return input - 'a' + 10;
    throw std::invalid_argument( "Invalid input hex string." );
}

void hexToBin( const char* src, char* target )
{
    while ( *src && src[1] )
    {
        *( target++ ) = charToInt( *src ) * 16 + charToInt( src[1] );
        src += 2;
    }
}

void printHex( const std::string& ctxMsg, const uint8_t* hex, size_t len )
{
    if ( 0 == len )
        return;

    size_t buffLen = 2 * len + 1; // two hex chars per byte, plus null terminator
    std::vector<char> strBuff( buffLen );
    strBuff[buffLen] = '\0';

    for ( size_t i = 0; i < len; ++i )
    {
        sprintf( &strBuff[2 * i], "%02X", hex[i] );
    }
    std::cout << ctxMsg << ", len[" << len << "]: " << &strBuff[0] << std::endl;
}

}
