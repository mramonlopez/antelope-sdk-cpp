//
//  SigningRequestCallback.hpp
//  Antelope SDK
//
//  Created by Manuel Ramon Lopez Torres on 5/6/23.
//

#ifndef SigningRequestCallback_hpp
#define SigningRequestCallback_hpp

#include <string>
#include <vector>

namespace onikami {
namespace eosclient {

class SigningRequestCallback {
public:
    std::string host;
    std::string path;
    std::string query;
     
    std::vector<std::string> sigs; /* Signatures. */
    std::string tx; /* Transaction ID as HEX-encoded string. */
    std::string bn; /* Block number hint (only present if transaction was broadcast). */
    std::string sa; /* Signer authority, aka account name. */
    std::string sp; /* Signer permission, e.g. "active". */
    std::string rbn; /* Reference block num used when resolving request. */
    std::string rid; /* Reference block id used when resolving request. */
    std::string req; /* The originating signing request packed as a uri string. */
    std::string ex; /* Expiration time used when resolving request. */
    std::string cid; /* The resolved chain id.  */
};

}}

#endif /* SigningRequestCallback_hpp */