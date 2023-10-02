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
#include <eosclient/PermissionLevel.hpp>

namespace onikami {
namespace eosclient {

//class Authorization {
//public:
//    std::string actor;
//    std::string permission;
//    
//    inline Authorization(std::string actor, std::string permission) :
//        actor(actor), permission(permission) {}
//};

class Action {
public:
    std::string account = "";
    std::string name = "";
    nlohmann::json data;
    std::vector<PermissionLevel> authorization;
    std::string abi = "";
    
private:
    const char* serialize();
};

void to_json(nlohmann::json& j, const Action& a);
void from_json(const nlohmann::json& j, Action& a);

}
}

#endif /* Action_hpp */
