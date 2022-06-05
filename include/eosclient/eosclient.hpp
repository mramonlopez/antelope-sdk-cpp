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

class EOSClient {
public:
    EOSClient(std::string api_url, std::string priv_key, std::string account);
    
    void action(std::string contract_name, std::string action, nlohmann::json data);
private:
    
    std::string api_url_;
    std::string priv_key_;
    std::string account_;
};

#endif /* eosclient_hpp */
