//
//  app.h
//  waxtestapp
//
//  Created by Manuel Ramon Lopez Torres on 11/4/22.
//

#ifndef eosclinet_h
#define eosclinet_h

#include <eosclient/eosclient_func.h>
#include <eosio/abieos.h>
/**
 * @file app.cpp
 * @date 10/2020
 * @author Luc Gerrits <luc.gerrits@univ-cotedazur.fr>
 * @brief C++ client for helloworld smart contract.
 * @version 0.1
 */

// abieos test functions
template <typename T>
T check(T value, const char *msg = "")
{
    if (!value)
        throw std::runtime_error(std::string{msg} + " failed");
    return value;
}

template <typename T>
T check_context(abieos_context *context, int line, std::string file, T value)
{
    if (!value)
    {
        std::cout << "Error at: ./" << file << ":" << line << std::endl;
        throw std::runtime_error(abieos_get_error(context));
    }
    return value;
}

std::string base58check(std::string data);
bool is_canonical(unsigned char *signature);
void init_transaction(std::string priv_key, unsigned char *priv_key_bytes, json &tnx_json,
                      SECP256K1_API::secp256k1_context *&ctx);
void clear_program(abieos_context *context, SECP256K1_API::secp256k1_context *ctx);
void get_node_info(json &tnx_json, std::string &chain_id, unsigned char *chain_id_bytes);
void get_last_block_info(json &tnx_json);
void get_transaction_smart_contract_abi(std::string &smart_contract_abi);
void get_init_data(json &tnx_json, std::string &chain_id, unsigned char *chain_id_bytes,
                   std::string &smart_contract_abi);

void build_transaction_action_binary(abieos_context *context, json &tnx_json, std::string smart_contract_abi);
void build_packed_transaction(abieos_context *context, json tnx_json, uint64_t &transaction_contract, char *&packed_tnx,
                              int &packed_tnx_size);
void build_signature(SECP256K1_API::secp256k1_context *ctx, json &tnx_json, unsigned char *priv_key_bytes,
                     unsigned char *chain_id_bytes, char *packed_tnx, int packed_tnx_size);
void build_transaction(abieos_context *context, json &tnx_json, std::string smart_contract_abi,
                       uint64_t &transaction_contract, char *&packed_tnx, int &packed_tnx_size,
                       SECP256K1_API::secp256k1_context *ctx, unsigned char *priv_key_bytes,
                       unsigned char *chain_id_bytes);
void send_transaction(json &tnx_json, abieos_context *context, uint64_t transaction_contract,
                      std::string eos_signature_str);
int test_transaction(std::string priv_key);


#endif /* eosclinet_h */
