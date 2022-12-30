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

std::string onikami::eosclient::toBase64String(Buffer buffer) {
    const std::string charset("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/");
    
    unsigned int char_count;
    unsigned int bits;
    unsigned int input_idx = 0;
    
    std::string output = "";
    
    char_count = 0;
    bits = 0;
    
    for( input_idx=0; input_idx < buffer.size() ; input_idx++ ) {
        bits |= buffer[ input_idx ];
        
        char_count++;
        if (char_count == 3) {
            output += charset[(bits >> 18) & 0x3f];
            output += charset[(bits >> 12) & 0x3f];
            output += charset[(bits >> 6) & 0x3f];
            output += charset[bits & 0x3f];
            bits = 0;
            char_count = 0;
        } else {
            bits <<= 8;
        }
    }
    
    if (char_count) {
        if (char_count == 1) {
            bits <<= 8;
        }

        output += charset[(bits >> 18) & 0x3f];
        output += charset[(bits >> 12) & 0x3f];
        if (char_count > 1) {
            output += charset[(bits >> 6) & 0x3f];
        } else {
            output += '=';
        }
        output += '=';
    }
    
    return output;
}
