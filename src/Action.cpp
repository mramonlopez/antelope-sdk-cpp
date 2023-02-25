//
//  Action.cpp
//  eosclient
//
//  Created by Manuel Ramon Lopez Torres on 1/1/23.
//

#include <eosclient/Action.hpp>
#include <nlohmann/json.hpp>
#include <eosclient/eosclient_lib.hpp>

using namespace onikami::eosclient;
using json = nlohmann::json;

const char* Action::serialize() {
    abieos_context *context = check(abieos_create());
    
    uint64_t contract = check_context(context, __LINE__, __FILE__, abieos_string_to_name(context,this->account.c_str()));
    check_context(context, __LINE__, __FILE__, abieos_set_abi(context, contract, this->abi.c_str()));
    
    
    check_context(context, __LINE__, __FILE__,
                  abieos_json_to_bin_reorderable(context, contract, this->name.c_str(), data.dump().c_str()));
    
    auto data = check_context(context, __LINE__, __FILE__, abieos_get_bin_data(context));
    
    abieos_destroy(context);
    
    return data;
}

void onikami::eosclient::to_json(nlohmann::json& j, const Action& a) {
    j = json();
    
    j["account"] = a.account;
    j["name"] = a.name;
    j["authorization"] = json(a.authorization);
    
    abieos_context *context = check(abieos_create());
    
    j["data"] = std::string(check_context(context, __LINE__, __FILE__,
                                      build_transaction_action_binary(context, a.account, a.name, a.abi, a.data)));
    
    abieos_destroy(context);
}

void onikami::eosclient::from_json(const nlohmann::json& j, Action& a) {
    
}

