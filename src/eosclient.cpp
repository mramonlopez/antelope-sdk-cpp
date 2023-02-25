//
//  eosclient.cpp
//  eosclient
//
//  Created by Manuel Ramon Lopez Torres on 18/4/22.
//

#include <eosclient/eosclient.hpp>
#include <eosclient/eosclient_lib.hpp>
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
                
                this->authorizers_.push_back(Authorizer(a.account, priv_key, a.permission));
            }
        }
    

std::string EOSClient::sendTransaction(Transaction transaction) {
    SECP256K1_API::secp256k1_context *ctx = SECP256K1_API::secp256k1_context_create(SECP256K1_CONTEXT_SIGN);
    
    abieos_context *context = check(abieos_create());
    char *packed_tnx; //buffer of 10000
    int packed_tnx_size;
    uint64_t transaction_contract;
    
    std::string chain_id;
    unsigned char chain_id_bytes[HASH_SHA256_SIZE];

    auto node_info = get_node_info(api_url_, chain_id, chain_id_bytes);
    std::uint64_t last_irreversible_block_num = node_info["last_irreversible_block_num"].get<std::uint64_t>();
    std::string head_block_time = node_info["head_block_time"].get<std::string>();
    
    auto last_block_info = get_last_block_info(api_url_, last_irreversible_block_num);
    
    transaction.ref_block_num = (std::uint16_t)(last_irreversible_block_num & 0xFFFF);
    transaction.expiration = parse_date_string(head_block_time) + 6 * 60;
    transaction.ref_block_prefix = last_block_info["ref_block_prefix"].get<std::uint32_t>();

    std::vector<std::string> priv_key_bytes_vector;
    
    for(auto a : this->authorizers_) {
        auto bytesString = fromHexStr(a.priv_key);
       
        priv_key_bytes_vector.push_back(bytesString);
    }

    json tnx_json = json(transaction);
    
    build_transaction(context, tnx_json, transaction_contract, packed_tnx, packed_tnx_size, ctx,
                      priv_key_bytes_vector, chain_id_bytes);

    auto response = send_transaction(api_url_, tnx_json, context, transaction_contract);
    
    clear_program(context, ctx);
    
    json response_json;
    CHECK(parseJSON(response, response_json) == 1);
    
    std::string transaction_id = response_json["transaction_id"].is_null() ? "" : response_json["transaction_id"];
    uint64_t block = 0;
    
    std::cout << "response: " << response_json.dump(1) << std::endl;
    
    if (transaction_id == "") {

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


Action* EOSClient::setABIAction(Action* action) {
    std::string smart_contract_abi;
    
    get_transaction_smart_contract_abi(this->api_url_, action->account, smart_contract_abi);
    action->abi = smart_contract_abi;
    
    return action;
}
