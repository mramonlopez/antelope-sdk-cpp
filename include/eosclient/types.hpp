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
std::string toString(Buffer buffer);
std::string toHexString(Buffer buffer);

} // eosclient
} // onikami


#endif /* eosclient_types_h */
