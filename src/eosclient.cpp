//
//  eosclient.cpp
//  eosclient
//
//  Created by Manuel Ramon Lopez Torres on 18/4/22.
//

#include <eosclient/eosclient.hpp>
#include <eosclient/eosclient_lib.h>
#include <eosclient/base58.hpp>
#include <iostream>
#include <iomanip>

// Private Key format: https://learnmeabitcoin.com/technical/wif
EOSClient::EOSClient(std::string api_url, std::string priv_key, std::string account) :
        api_url_(api_url),
        account_(account) {
            auto buffer = Base58Decode(priv_key);
            auto key = convertBytesToHexStr(buffer);
            auto key_string = std::string(key.begin(), key.end());
            
            // Private Key format: https://learnmeabitcoin.com/technical/wif
            this->priv_key_ = key_string.substr(2, key_string.size() - 10); // prefix (2) + checksum (8)
            std::cout << this->priv_key_.c_str() << std::endl;
        }
    

std::string EOSClient::action(std::string contract_name, std::string action, nlohmann::json data) {
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    std::chrono::steady_clock::time_point end;

    json tnx_json; // final json of the tnx that will be sended
    
    unsigned char priv_key_bytes[32];
    SECP256K1_API::secp256k1_context *ctx;
    init_transaction(priv_key_, priv_key_bytes, tnx_json, ctx);
    
    // Set the initial tnx values:
    tnx_json["actions"][0]["account"] = contract_name;
    tnx_json["actions"][0]["name"] = action;
    tnx_json["actions"][0]["authorization"][0]["actor"] = account_;

    std::string smart_contract_abi;
    abieos_context *context = check(abieos_create());
    char *packed_tnx; //buffer of 10000
    int packed_tnx_size;
    uint64_t transaction_contract;
    std::string last_irreversible_block_num;
    std::string chain_id;
    unsigned char chain_id_bytes[HASH_SHA256_SIZE];

    end = std::chrono::steady_clock::now();
    get_init_data(api_url_, tnx_json, chain_id, chain_id_bytes, smart_contract_abi);
    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count()
              << "[µs]" << std::endl;

    // Print current tx variables before build tnx:
    std::cout << "**** Transaction variables ****" << std::endl;
    std::cout << std::setw(20) << std::left << "* ref_block_num: " << std::setw(30) << tnx_json["ref_block_num"]
              << std::endl;
    std::cout << std::setw(20) << std::left << "* ref_block_prefix: " << std::setw(30)
              << tnx_json["ref_block_prefix"] << std::endl;
    std::cout << std::setw(20) << std::left << "* expiration: " << std::setw(30) << tnx_json["expiration"]
              << std::endl;
    std::cout << std::setw(20) << std::left << "* chain_id: " << std::setw(30) << chain_id << std::endl;
    std::cout << std::setw(20) << std::left << "* private key: " << std::setw(30) << priv_key_ << std::endl;

    begin = std::chrono::steady_clock::now();

    build_transaction(context, tnx_json, smart_contract_abi, transaction_contract, packed_tnx, packed_tnx_size, ctx,
                      priv_key_bytes, chain_id_bytes, data);
    
    
    end = std::chrono::steady_clock::now();
    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count()
              << "[µs]" << std::endl;
    
    
    begin = std::chrono::steady_clock::now();
    auto response = send_transaction(api_url_, tnx_json, context, transaction_contract,
                     tnx_json["signatures"][0].get<std::string>());
    end = std::chrono::steady_clock::now();
    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count()
              << "[µs]" << std::endl;
    
    clear_program(context, ctx);
    
    json response_json;
    CHECK(parseJSON(response, response_json) == 1);
    
    std::string transaction_id = response_json["transaction_id"].is_null() ? "" : response_json["transaction_id"].dump();
    uint64_t block = 0;
    
    if (!response_json["processed"].is_null()) {
        if (!response_json["processed"]["block_num"].is_null()) {
            block = response_json["processed"]["block_num"];
        } else if (!response_json["processed"]["action_traces"].is_null()) {
            // TODO: get block_nun of last trace?
        }
        
    }
    
//    if (transaction_id == "") {
        std::cout << "response: " << response_json.dump(1) << std::endl;
//    }
    
    json result;
    
    result["transaction_id"] = transaction_id;
    result["block"] = block;
    
    return result.dump();
}


std::string EOSClient::getTransactionState(std::string transactionId, uint64_t blockNumHint) {
    json tnx_json;
    
    return history_get_transaction(this->api_url_, tnx_json, transactionId, blockNumHint);
}


