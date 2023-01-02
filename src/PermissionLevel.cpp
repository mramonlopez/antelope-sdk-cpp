//
//  PermissionLevel.cpp
//  eosclient
//
//  Created by Manuel Ramon Lopez Torres on 1/1/23.
//

#include <eosclient/PermissionLevel.hpp>
#include <nlohmann/json.hpp>

using namespace onikami::eosclient;
using json = nlohmann::json;

void onikami::eosclient::to_json(nlohmann::json& j, const PermissionLevel& p) {
    j = json();
    
    j["actor"] = p.actor;
    j["permission"] = p.permission;
}

void onikami::eosclient::from_json(const nlohmann::json& j, PermissionLevel& p) {
    
}

