//
//  EosioSigningRequest.cpp
//  eosclient
//
//  Created by Manuel Ramon Lopez Torres on 30/12/22.
//

#include <eosclient/EosioSigningRequest.hpp>
#include <eosclient/types.hpp>
#include <iostream>

using namespace onikami::eosclient;
using json = nlohmann::json;

/**
 * Encode this request into an `esr:` uri.
 * @argument compress Whether to compress the request data using zlib,
 *                    defaults to true if omitted and zlib is present;
 *                    otherwise false.
 * @argument slashes Whether add slashes after the protocol scheme, i.e. `esr://`.
 *                   Defaults to true.
 * @returns An esr uri string.
 */
std::string EosioSigningRequest::encode(bool compress, bool slashes) {
    auto header = EosioSigningRequest::version;
    
    Buffer buffer;
    
    auto data = this->getData();
    buffer.insert(buffer.end(), data.begin(), data.end());
    
    auto signature = this->getSignatureData();
    buffer.insert(buffer.end(), signature.begin(), signature.end());
    
    if (compress) {
        Buffer deflated; // = deflateRaw(buffer);
        
        // If compressed data is tiner than uncompressed...
        if (buffer.size() > deflated.size()) {
            // Header bit 7:
            // 10000000 -> compressed
            // 00000000 -> uncompressed
            header |= 1 << 7;
            buffer = deflated;
        }
    }
    
    Buffer out({0});
    
    out[0] = header;
    
    // insert data and signature in out buffer
    out.insert(out.end(), buffer.begin(), buffer.end());
    
    std::string scheme = slashes ? "esr://" : "esr:";
    
    return scheme + toBase64UString(out);
}


Buffer EosioSigningRequest::getData() {
    Buffer out;
    
    auto chain_id = toBufferFromHex(this->data_.chain_id);
    
    out.push_back(0x01); // 0x00 -> chain alias, 0x01 -> chain id
    out.insert(out.end(), chain_id.begin(), chain_id.end());
    
    size_t variant_idx = this->data_.req.index();
    
    Buffer data;
    
    switch (variant_idx) {
        case 2: {
            auto transaction = std::get<Transaction>(this->data_.req);
            data = transaction.getPackedData();
            break;
        }
        default:
            throw std::runtime_error("Data variant not yet implemented");
            break;
    }
    
    out.push_back(variant_idx);
    out.insert(out.end(), data.begin(), data.end());
    
    out.push_back(this->data_.flags); // broadcast
    out.push_back(0x00); // callback
    out.push_back(0x00); // info
    
    return out;
}

Buffer EosioSigningRequest::getSignatureData() {
    return Buffer();
}


void onikami::eosclient::to_json(nlohmann::json& j, const RequestDataV2& r) {
    j = json();
    
    j["chain_id"] = r.chain_id;
    Action a = std::get<Action>(r.req);
    j["req"] = json(a);
    j["flags"] = r.flags;
    j["callback"] = r.callback;
    j["info"] = r.info;
}

void onikami::eosclient::from_json(const nlohmann::json& j, RequestDataV2& r) {
    
}

