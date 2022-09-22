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
