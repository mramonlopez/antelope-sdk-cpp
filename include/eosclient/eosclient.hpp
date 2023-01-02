//
//  eosclient.hpp
//  eosclient
//
//  Created by Manuel Ramon Lopez Torres on 18/4/22.
//

#ifndef eosclient_hpp
#define eosclient_hpp

#include <eosclient/Action.hpp>
#include <iostream>
#include <string>

namespace onikami {
namespace eosclient {

class Authorizer {
public:
    std::string account;
    std::string priv_key;
    std::string permission;
    
    Authorizer(std::string account, std::string priv_key, std::string permission = "active") :
    account(account),
    priv_key(priv_key),
    permission(permission) {}
};

class EOSClient {
public:
    EOSClient(std::string api_url, std::vector<Authorizer> authorizers);
    
    std::string action(std::vector<Action> actions);
    std::string getTransactionState(std::string transaction_id, uint64_t blockNumHint);
    
    inline std::string getApiURL() const { return api_url_; }
    
    std::string getPublicKey();
    static bool createKeyPair(std::string &priv_key, std::string &pub_key);
    
private:
    std::string api_url_;
    std::vector<Authorizer> authorizers_;
};

} // namespace eosclient
} // namespace onikami

#endif /* eosclient_hpp */
