//
//  EosioSigningRequest.cpp
//  eosclient
//
//  Created by Manuel Ramon Lopez Torres on 30/12/22.
//

#include <eosclient/EosioSigningRequest.hpp>
#include <eosclient/types.hpp>

using namespace onikami::eosclient;

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
    auto data = this->getData();
    auto sigData = this->getSignatureData();
    
    Buffer buffer(data);
    buffer.insert(buffer.end(), sigData.begin(), sigData.end());
    
    auto header = EosioSigningRequest::version;
    
    if (compress) {
        Buffer deflated; // = deflateRaw(buffer);
        
        if (buffer.size() > deflated.size()) {
            header |= 1 << 7;
            buffer = deflated;
        }
    }
    
    Buffer out({0});
    
    out[0] = header;
    out.insert(out.end(), buffer.begin(), buffer.end());
    
    std::string scheme = slashes ? "esr://" : "esr:";
    
    return scheme + toBase64String(out);
}


Buffer EosioSigningRequest::getData() {
    // return Serializer.encode({object: this.data}).array
    
    return toBuffer("Esto es una prueba");
}

Buffer EosioSigningRequest::getSignatureData() {
    return Buffer();
}

