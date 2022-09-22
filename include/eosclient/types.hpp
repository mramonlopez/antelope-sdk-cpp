//
//  types.hpp
//  waxtestapp
//
//  Created by Manuel Ramon Lopez Torres on 20/9/22.
//

#ifndef types_h
#define types_h

#include <vector>
#include <string>

namespace onikami
{
namespace eosclient
{

typedef std::vector<unsigned char> Buffer;

Buffer toBuffer(std::string str);
std::string toString(Buffer buffer);
std::string toHexString(Buffer buffer);

} // eosclient
} // onikami


#endif /* types_h */
