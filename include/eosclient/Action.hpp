//
//  Action.hpp
//  eosclient
//
//  Created by Manuel Ramon Lopez Torres on 26/11/22.
//

#ifndef Action_hpp
#define Action_hpp

#include <string>
#include <nlohmann/json.hpp>

namespace onikami {
namespace eosclient {

class Authorization {
public:
    std::string actor;
    std::string permission;
    
    inline Authorization(std::string actor, std::string permission) :
        actor(actor), permission(permission) {}
};

class Action {
public:
    std::string account;
    std::string name;
    nlohmann::json data;
    std::vector<Authorization> authorization;
};

}
}
#endif /* Action_hpp */
