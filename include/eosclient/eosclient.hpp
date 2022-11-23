//
//  eosclient.hpp
//  eosclient
//
//  Created by Manuel Ramon Lopez Torres on 18/4/22.
//

#ifndef eosclient_hpp
#define eosclient_hpp

#include <nlohmann/json.hpp>
#include <iostream>
#include <string>

class EOSClient {
public:
    EOSClient(std::string api_url, std::string priv_key, std::string account);
    
    std::string action(std::string contract_name, std::string action, nlohmann::json data);
    std::string getTransactionState(std::string transaction_id, uint64_t blockNumHint);
    std::string getPublicKey();
    bool createKeyPair(std::string &priv_key, std::string &pub_key);
    inline std::string getApiURL() const { return api_url_; }
private:
    
    std::string api_url_;
    std::string priv_key_;
    std::string account_;
};

#endif /* eosclient_hpp */
