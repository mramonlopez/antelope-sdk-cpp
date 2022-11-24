//
//  eosclient.cpp
//  eosclient
//
//  Created by Manuel Ramon Lopez Torres on 18/4/22.
//

#include <eosclient/eosclient.hpp>
#include <eosclient/eosclient_lib.h>
#include <eosclient/base58.hpp>
#include <eosclient/types.hpp>
#include <iostream>
#include <iomanip>
#include <eosio/crypto.hpp>
#include <secp256k1_extrakeys.h>

using namespace onikami::eosclient;

// Private Key format: https://learnmeabitcoin.com/technical/wif
EOSClient::EOSClient(std::string api_url, std::vector<Authorizer> authorizers) :
        api_url_(api_url) {
            for (auto a : authorizers) {
                auto buffer = Base58Decode(a.priv_key);
                auto key = convertBytesToHexStr(buffer);
                auto key_string = std::string(key.begin(), key.end());
                
                // Private Key format: https://learnmeabitcoin.com/technical/wif
                auto priv_key = key_string.substr(2, key_string.size() - 10); // prefix (2) + checksum (8)
                std::cout << "priv_key -> " << priv_key << std::endl;
                
                this->authorizers_.push_back(Authorizer(a.account, priv_key));
            }
        }
    

std::string EOSClient::action(std::string contract_name, std::string action, nlohmann::json data) {
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    std::chrono::steady_clock::time_point end;

    json tnx_json; // final json of the tnx that will be sended
    SECP256K1_API::secp256k1_context *ctx = SECP256K1_API::secp256k1_context_create(SECP256K1_CONTEXT_SIGN);
    
    init_transaction_json(tnx_json);
    
    // Set the initial tnx values:
    tnx_json["actions"][0]["account"] = contract_name;
    tnx_json["actions"][0]["name"] = action;
    
    for (auto a : this->authorizers_) {
        json auth;
        auth["actor"] = a.account;
        auth["permission"] = "active";
        tnx_json["actions"][0]["authorization"].push_back(auth);
    }
    

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
    
    begin = std::chrono::steady_clock::now();
    
    std::vector<std::string> priv_key_bytes_vector;
    
    for(auto a : this->authorizers_) {
        auto bytesString = fromHexStr(a.priv_key);
       
        priv_key_bytes_vector.push_back(bytesString);
    }

    build_transaction(context, tnx_json, smart_contract_abi, transaction_contract, packed_tnx, packed_tnx_size, ctx,
                      priv_key_bytes_vector, chain_id_bytes, data);
    
    
    end = std::chrono::steady_clock::now();
    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count()
              << "[µs]" << std::endl;
    
    
    begin = std::chrono::steady_clock::now();
    
    auto response = send_transaction(api_url_, tnx_json, context, transaction_contract);
    end = std::chrono::steady_clock::now();
    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count()
              << "[µs]" << std::endl;
    
    clear_program(context, ctx);
    
    json response_json;
    CHECK(parseJSON(response, response_json) == 1);
    
    std::string transaction_id = response_json["transaction_id"].is_null() ? "" : response_json["transaction_id"];
    uint64_t block = 0;
    
    if (transaction_id == "") {
        std::cout << "response: " << response_json.dump(1) << std::endl;
        if (response_json.contains("error") &&
            response_json["error"].contains("details") &&
            response_json["error"]["details"].is_array()) {
            std::string msg = "";
            
            for(auto d : response_json["error"]["details"]) {
                msg = msg.append(d["message"]).append("\n");
            }
            
            throw std::runtime_error(msg);
        }
    } else if (!response_json["processed"].is_null()) {
        if (!response_json["processed"]["block_num"].is_null()) {
            block = response_json["processed"]["block_num"];
        } else if (!response_json["processed"]["action_traces"].is_null()) {
            // TODO: get block_nun of last trace?
        }
    }
    
    json result;
    
    result["transaction_id"] = transaction_id;
    result["block"] = block;
    
    return result.dump();
}


std::string EOSClient::getTransactionState(std::string transactionId, uint64_t blockNumHint) {
    json trx_json;
    
    history_get_transaction(this->api_url_, trx_json, transactionId, blockNumHint);
    
    return trx_json.dump();
}

std::string EOSClient::getPublicKey() {
    SECP256K1_API::secp256k1_context *ctx =
        SECP256K1_API::secp256k1_context_create(SECP256K1_CONTEXT_SIGN);
    
    secp256k1_pubkey pubkey;
    
    // Private key to WIF format
    // TODO: use function parameter to select authorizer
    auto bytesString = fromHexStr(this->authorizers_.at(0).priv_key);
    unsigned char *priv_key_bytes = (unsigned char *) &bytesString[0];
    
    // Get public key from private key
    auto return_val = secp256k1_ec_pubkey_create(ctx, &pubkey, priv_key_bytes);
    
    // Serialize public key
    unsigned char out[33];
    size_t out_size = 33;
    
    secp256k1_ec_pubkey_serialize(ctx, out, &out_size, &pubkey, SECP256K1_EC_COMPRESSED);
    
    SECP256K1_API::secp256k1_context_destroy(ctx);
    
    // To EOS strign format
    eosio::ecc_public_key a;
    std::copy(out, out + out_size, a.begin());
    
    eosio::public_key b;
    
    b.emplace<0>(a);
    
    auto result = eosio::public_key_to_string(b);
    
    return result;
}

bool EOSClient::createKeyPair(std::string &priv_key, std::string &pub_key) {
    SECP256K1_API::secp256k1_context *ctx =
        SECP256K1_API::secp256k1_context_create(SECP256K1_CONTEXT_SIGN);
    
    secp256k1_keypair keypair;
    unsigned char sk[32];
    
    unsigned char *tmp = sk;
    
    for (int i = 0; i < 8; ++i) {
        uint64_t val = std::rand();
        
        tmp[0] = val;
        tmp[1] = val >> 8;
        tmp[2] = val >> 16;
        tmp[3] = val >> 24;
        tmp += 4;
    }
    
    if (!secp256k1_keypair_create(ctx, &keypair, sk)) {
        return false;
    }
    
    unsigned char prikey[32];
    
    if (!secp256k1_keypair_sec(ctx, prikey, &keypair)) {
        return false;
    }
    
    secp256k1_pubkey pubkey;
    
    if (!secp256k1_keypair_pub(ctx, &pubkey, &keypair)) {
        return false;
    }
    
    // Serialize public key
    unsigned char out[33];
    size_t out_size = 33;
    
    secp256k1_ec_pubkey_serialize(ctx, out, &out_size, &pubkey, SECP256K1_EC_COMPRESSED);
    
    // To EOS string format
    eosio::ecc_public_key p;
    std::copy(out, out + out_size, p.begin());
    
    eosio::public_key ep;
    
    ep.emplace<0>(p);
    
    pub_key = eosio::public_key_to_string(ep);
    
    // Serialize private key
    std::string key_str = std::string({(char) 0x80}) + std::string((char *) prikey, 32);
    
    priv_key = base58check(key_str);
    
    SECP256K1_API::secp256k1_context_destroy(ctx);
    
    return true;
}
