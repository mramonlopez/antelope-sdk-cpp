//
//  types.hpp
//  eosclient
//
//  Created by Manuel Ramon Lopez Torres on 22/9/22.
//

#include <eosclient/types.hpp>
#include <vector>
#include <string>

using namespace onikami::eosclient;

Buffer onikami::eosclient::toBuffer(std::string str) {
    return Buffer(str.begin(), str.end());
}

Buffer onikami::eosclient::toBuffer(unsigned char *c, size_t size) {
    return Buffer(c, c + size);
}

std::string onikami::eosclient::toString(Buffer buffer) {
    return std::string(buffer.begin(), buffer.end());
}

std::string onikami::eosclient::toHexString(Buffer buffer) {
    const std::string charset("0123456789abcdef");
    std::string hex("");
    
    for(auto byte : buffer) {
        hex += charset.at(byte >> 4);
        hex += charset.at(byte & 0x0F);
    }
    
    return hex;
}
