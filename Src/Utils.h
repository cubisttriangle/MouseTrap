#pragma once

#include <string>

namespace Util
{

int charToInt( char input );
void hexToBin( const char* src, char* target );
void printHex( const std::string& ctxMsg, const uint8_t* hex, size_t len );

}
