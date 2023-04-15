//
//  types.hpp
//  eosclient
//
//  Created by Manuel Ramon Lopez Torres on 20/9/22.
//

#ifndef eosclient_types_h
#define eosclient_types_h

#include <vector>
#include <string>

namespace onikami
{
namespace eosclient
{

typedef std::vector<unsigned char> Buffer;

Buffer toBuffer(std::string str);
Buffer toBuffer(unsigned char *c, size_t size);
Buffer toBufferFromHex(std::string str);
std::string toString(Buffer buffer);
std::string toHexString(Buffer buffer);
std::string toBase64String(Buffer buffer, const std::string charset = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/", bool with_padding = true);
std::string toBase64UString(Buffer buffer);

} // eosclient
} // onikami


#endif /* eosclient_types_h */
