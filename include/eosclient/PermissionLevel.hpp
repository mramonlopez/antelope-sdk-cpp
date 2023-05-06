//
//  PermissionLevel.hpp
//  Antelope SDK
//
//  Created by Manuel Ramon Lopez Torres on 1/1/23.
//

#ifndef PermissionLevel_hpp
#define PermissionLevel_hpp

#include <string>
#include <nlohmann/json.hpp>

namespace onikami {
namespace eosclient {

class PermissionLevel {
public:
    std::string actor;
    std::string permission;
};


void to_json(nlohmann::json& j, const PermissionLevel& p);
void from_json(const nlohmann::json& j, PermissionLevel& p);

}}

#endif /* PermissionLevel_hpp */
