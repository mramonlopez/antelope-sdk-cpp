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

Buffer toBuffer(std::string str) {
    return Buffer(str.begin(), str.end());
}

std::string toString(Buffer buffer) {
    return std::string(buffer.begin(), buffer.end());
}

std::string toHexString(Buffer buffer) {
    const std::string charset("0123456789abcdef");
    std::string hex("");
    
    for(auto byte : buffer) {
        hex += charset.at(byte >> 4);
        hex += charset.at(byte & 0x0F);
    }
    
    return hex;
}

} // eosclient
} // onikami


#endif /* types_h */
