//
//  eosclient.hpp
//  eosclient
//
//  Created by Manuel Ramon Lopez Torres on 18/4/22.
//

#ifndef eosclient_hpp
#define eosclient_hpp

#include <nlohmann/json.hpp>

class EOSClient {
public:
    // Private Key format:
    // 1) go to https://learnmeabitcoin.com/technical/wif
    // 2) paste private key in "WIF" text box
    // 3) copy "Private Key text box content
    EOSClient(std::string api_url, std::string priv_key, std::string account) :
        api_url_(api_url),
        priv_key_(priv_key),
        account_(account) {}
    
    void action(std::string contract_name, std::string action, nlohmann::json data);
private:
    
    std::string api_url_;
    std::string priv_key_;
    std::string account_;
};

#endif /* eosclient_hpp */
