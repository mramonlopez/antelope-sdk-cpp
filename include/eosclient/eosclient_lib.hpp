//
//  app.h
//  waxtestapp
//
//  Created by Manuel Ramon Lopez Torres on 11/4/22.
//

#ifndef eosclient_lib_h
#define eosclient_lib_h

#include <eosclient/eosclient_func.hpp>
#include <eosio/abieos.h>
#include <iostream>
 
//Crypto and hashing stuff:
#include <secp256k1.h>
#include <secp256k1_recovery.h>
#include <eosclient/Transaction.hpp>

using namespace onikami::eosclient;

/**
 * @file eosclient_lib.h
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
void init_transaction_json(json &tnx_json);
void clear_program(abieos_context *context, SECP256K1_API::secp256k1_context *ctx);
json get_node_info(std::string api_url, std::string &chain_id, unsigned char *chain_id_bytes);
json get_last_block_info(std::string api_url, uint64_t last_irreversible_block_num);
std::string get_expiration_date_string(std::string date, int minutes_to_expire);
time_t parse_date_string(std::string date);

void get_transaction_smart_contract_abi(std::string api_url, std::string contact_name, std::string &smart_contract_abi);
void get_init_data(std::string api_url, json &tnx_json, std::string &chain_id, unsigned char *chain_id_bytes);

std::string build_signature(unsigned char *priv_key_bytes,
                     unsigned char *chain_id_bytes, const Buffer &packed_tnx);
json build_transaction(abieos_context *context, Transaction *transaction,
                       std::vector<std::string> priv_key_bytes_vector,
                       unsigned char *chain_id_bytes);
std::string send_transaction(std::string api_url, json &tnx_json, abieos_context *context);

void history_get_transaction(std::string api_url, json &tnx_json, std::string transaction_id, uint64_t blockNumHint);

#endif /* eosclient_lib_h */
