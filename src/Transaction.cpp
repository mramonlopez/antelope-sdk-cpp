//
//  Transaction.cpp
//  eosclient
//
//  Created by Manuel Ramon Lopez Torres on 31/1/23.
//

#include <eosclient/Transaction.hpp>
#include <nlohmann/json.hpp>
#include <eosclient/eosclient_lib.hpp>

using namespace onikami::eosclient;
using json = nlohmann::json;

Buffer Transaction::getPackedData() {
    //auto data = "59dbd863e586ec6a225200000a00030000000000ea305500409e9a2264b89a01606ef67251c594c300000000a8ed323266606ef67251c594c310c810810c1124c50100000001000294c54b835f3f978b32194942443d2c2584b4d2d014ad11922907e0e708c56dbc01000000010000000100031970d4bc731135d03dbe0d5fa8d50916ef73f1e79881baf5ff0cbe8822ae180c010000000000000000ea305500b0cafe4873bd3e01606ef67251c594c300000000a8ed323214606ef67251c594c310c810810c1124c5b40b00000000000000ea305500003f2a1ba6a24a01606ef67251c594c300000000a8ed323231606ef67251c594c310c810810c1124c500e1f50500000000085741580000000000e1f5050000000008574158000000000000";
    
    abieos_context *context = check(abieos_create());
    
    // Add ABI contrat of "transaction" to context
    // Move to getName function
    auto transaction_contract =
        check_context(context, __LINE__, __FILE__, abieos_string_to_name(context, "transaction"));
    check_context(context, __LINE__, __FILE__,
                  abieos_set_abi(context, transaction_contract, transactionAbi_str.c_str()));
    
    // Get packed unsigned transaction
    json tnx_json;
    to_json(tnx_json, *this);
    
    std::cout << tnx_json.dump().c_str() << std::endl;
    
    check_context(context, __LINE__, __FILE__,
                  abieos_json_to_bin_reorderable(context, transaction_contract, "transaction",
                                                 tnx_json.dump().c_str()));

    //packed_tnx = (char *)check_context(context, __LINE__, __FILE__,
//                                       abieos_get_bin_data(context)); // use abieos_get_bin_hex for hex
    
    
    const char *signed_packed_tnx = check_context(context, __LINE__, __FILE__, abieos_get_bin_data(context));
    int signed_packed_tnx_size = abieos_get_bin_size(context);
    
    Buffer data_buf = toBuffer((unsigned char *) signed_packed_tnx, signed_packed_tnx_size);
    
    abieos_destroy(context);
    
    return data_buf;
    
    //auto data = "10daf963906f18aaef9b00000a00030000000000ea305500409e9a2264b89a01606ef67251c594c300000000a8ed323266606ef67251c594c310c810810c1124c50100000001000294c54b835f3f978b32194942443d2c2584b4d2d014ad11922907e0e708c56dbc01000000010000000100031970d4bc731135d03dbe0d5fa8d50916ef73f1e79881baf5ff0cbe8822ae180c010000000000000000ea305500b0cafe4873bd3e01606ef67251c594c300000000a8ed323214606ef67251c594c310c810810c1124c5b40b00000000000000ea305500003f2a1ba6a24a01606ef67251c594c300000000a8ed323231606ef67251c594c310c810810c1124c500e1f50500000000085741580000000000e1f5050000000008574158000000000000";
    //return toBufferFromHex(data);
}

void  onikami::eosclient::to_json(nlohmann::json& j, const Transaction& t) {
    j = json();
    
    j["actions"] = json(t.actions);
    j["context_free_actions"] = json(t.context_free_actions);
    j["context_free_data"] = t.context_free_data;
    j["delay_sec"] = t.delay_sec;
    
    auto exp = gmtime(&t.expiration);
    char buffer [80];
    strftime(buffer, 80, "%Y-%m-%dT%H:%M:%S.000", exp);
    
    j["expiration"] = std::string(buffer);
    j["max_cpu_usage_ms"] = t.max_cpu_usage_ms;
    j["max_net_usage_words"] = t.max_net_usage_words;
    j["ref_block_num"] = t.ref_block_num;
    j["ref_block_prefix"] = t.ref_block_prefix;
    j["transaction_extensions"] = json(t.transaction_extensions);
}

void  onikami::eosclient::from_json(const nlohmann::json& j, Transaction& t) {
    
}

void  onikami::eosclient::to_json(nlohmann::json& j, const TransactionExtension& e) {
    j = json();
    
    j["type"] = e.type;
    j["data"] = e.data;
}

void  onikami::eosclient::from_json(const nlohmann::json& j, TransactionExtension& e) {
    
}

const std::string Transaction::transactionAbi_str = R"(
{
    "version": "eosio::abi/1.0",
    "types": [
        {
            "new_type_name": "account_name",
            "type": "name"
        },
        {
            "new_type_name": "action_name",
            "type": "name"
        },
        {
            "new_type_name": "permission_name",
            "type": "name"
        }
    ],
    "structs": [
        {
            "name": "permission_level",
            "base": "",
            "fields": [
                {
                    "name": "actor",
                    "type": "account_name"
                },
                {
                    "name": "permission",
                    "type": "permission_name"
                }
            ]
        },
        {
            "name": "action",
            "base": "",
            "fields": [
                {
                    "name": "account",
                    "type": "account_name"
                },
                {
                    "name": "name",
                    "type": "action_name"
                },
                {
                    "name": "authorization",
                    "type": "permission_level[]"
                },
                {
                    "name": "data",
                    "type": "bytes"
                }
            ]
        },
        {
            "name": "extension",
            "base": "",
            "fields": [
                {
                    "name": "type",
                    "type": "uint16"
                },
                {
                    "name": "data",
                    "type": "bytes"
                }
            ]
        },
        {
            "name": "transaction_header",
            "base": "",
            "fields": [
                {
                    "name": "expiration",
                    "type": "time_point_sec"
                },
                {
                    "name": "ref_block_num",
                    "type": "uint16"
                },
                {
                    "name": "ref_block_prefix",
                    "type": "uint32"
                },
                {
                    "name": "max_net_usage_words",
                    "type": "varuint32"
                },
                {
                    "name": "max_cpu_usage_ms",
                    "type": "uint8"
                },
                {
                    "name": "delay_sec",
                    "type": "varuint32"
                }
            ]
        },
        {
            "name": "transaction",
            "base": "transaction_header",
            "fields": [
                {
                    "name": "context_free_actions",
                    "type": "action[]"
                },
                {
                    "name": "actions",
                    "type": "action[]"
                },
                {
                    "name": "transaction_extensions",
                    "type": "extension[]"
                }
            ]
        }
    ]
}
)";
