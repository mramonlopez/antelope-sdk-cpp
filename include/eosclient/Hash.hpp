//
//  hash.hpp
//  eosclient
//
//  Created by Manuel Ramon Lopez Torres on 20/9/22.
//

#ifndef eosclient_hash_hpp
#define eosclient_hash_hpp

#include <eosclient/types.hpp>
#include <openssl/evp.h>

namespace onikami
{
namespace eosclient
{

class Hash
{
public:
    static bool SHA256(const Buffer &msg, Buffer &digest);
    static bool SHA512(const Buffer &msg, Buffer &digest);
    static bool RIPEMD160(const Buffer &msg, Buffer &digest);
    static bool hash(const Buffer &msg, Buffer &digest, const EVP_MD* algorithm);
    
    Hash() = delete;
};

} // eosclient
} // onikami

#endif /* eosclient_hash_hpp */
