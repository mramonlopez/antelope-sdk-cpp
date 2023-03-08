/**
 * @file app.cpp
 * @date 10/2020
 * @author Luc Gerrits <luc.gerrits@univ-cotedazur.fr>
 * @brief C++ client for helloworld smart contract.
 * @version 0.1
 */

// I followed some instructions from: https://github.com/EOSIO/eos/issues/5341

// #include "gperftools/profiler.h"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <chrono>
#include <stdio.h>
#include <ctime>
#include <sstream>


#include <eosio/abi.hpp>
#include <eosio/crypto.hpp>
#include <eosio/name.hpp>
#include <eosio/stream.hpp>
#include <eosio/time.hpp>
#include <eosio/to_json.hpp>
#include <eosio/varint.hpp>
#include <eosio/abieos.h>

#include <eosclient/eosclient_lib.hpp>
#include <eosclient/eosclient_func.hpp>
#include <eosclient/Hash.hpp>
#include <eosclient/types.hpp>

using namespace onikami::eosclient;

/**
 * This is the bitcoin base58check
 */
std::string base58check(std::string data)
{
	// Verifiable with "hello world" -> "3vQB7B6MrGQZaxCuFg4oh"
    Buffer data_sha256;
    Buffer data_checksum;
    
    Hash::SHA256(toBuffer(data), data_sha256);
    Hash::SHA256(data_sha256, data_checksum); // checksum is SHA256(SHA256(data))[0:4]
    
	std::string data_with_checksum = data;
	// concat 4 bytes
	data_with_checksum += data_checksum[0];
	data_with_checksum += data_checksum[1];
	data_with_checksum += data_checksum[2];
	data_with_checksum += data_checksum[3];
    
	std::string data_base58check_str = eosio::to_base58((const char *)data_with_checksum.c_str(),
														data_with_checksum.size()); // convert to base58
	return data_base58check_str;
}

/**
 * Function to test if signature is canonical
 * @param signature A 64 Bytes signature containing r then s parameter of the signature
 */
bool is_canonical(unsigned char *signature)
{
	// https://github.com/steemit/steem/issues/1944#issuecomment-354205064
	return !(signature[0] & 0x80) && !(signature[0] == 0 && !(signature[1] & 0x80)) && !(signature[32] & 0x80) &&
		   !(signature[32] == 0 && !(signature[33] & 0x80));
}

void init_transaction_json(json &tnx_json)
{
	// init final tnx with example of json:
	parseJSON(R"(
		{
            "actions": [],
            "context_free_actions":[],
            "context_free_data":[],
            "delay_sec":0,
            "expiration":"<REPLACED>",
            "max_cpu_usage_ms":0,
            "max_net_usage_words":0,
            "ref_block_num":0,
            "ref_block_prefix":0,
            "transaction_extensions":[]
		}
	)", tnx_json);
}

json get_node_info(std::string api_url, std::string &chain_id, unsigned char *chain_id_bytes) {
	// first get EOS node info
	std::string response;
	json response_json;
	if (sendData("", api_url + "/chain/get_info", response, CURL_IS_VERBOSE) == 1)
	{
		CHECK(parseJSON(response, response_json) == 1);
		if (!response_json["error"].is_null()) // if this exist then there is an error
		{
            std::string msg = "ERROR: Cannot access endpoint " + api_url + "\n" + response_json.dump(1);
                
			std::cerr << msg << std::endl;
            throw std::runtime_error(msg);
		}
		std::cout << "Endpoint OK : " << api_url << std::endl;
        std::cout << response_json.dump(1) << std::endl;
	}
	else
	{
        std::string msg = "ERROR: Cannot access endpoint " + api_url;
		std::cerr << msg << std::endl;
        throw std::runtime_error(msg);
	}
    
	chain_id = response_json["chain_id"]; // not used for the moment
    auto bytesString = fromHexStr(chain_id);
    strncpy((char *)chain_id_bytes, &bytesString[0], 32);
    
    return response_json;
}

json get_last_block_info(std::string api_url, uint64_t last_irreversible_block_num)
{
	// next: get the last block info to retrieve : chain_id, block_num, block_prefix, expiration
	std::string response;
	json response_json;
	CHECK(sendData("{\"block_num_or_id\":\"" + std::to_string(last_irreversible_block_num) + "\"}",
				   api_url + "/chain/get_block", response, CURL_IS_VERBOSE) == 1);
	CHECK(parseJSON(response, response_json) == 1);
    
    return response_json;
}

std::string get_expiration_date_string(std::string date, int minutes_to_expire) {
    int year, month, day, hour, minute, second, tail = 0;
    
    check(7 == sscanf(date.c_str(), "%d-%d-%dT%d:%d:%d.%d", &year, &month, &day, &hour, &minute, &second, &tail));
    
    tm t;
    
    t.tm_year = year - 1900;
    t.tm_mon = month - 1;
    t.tm_mday = day;
    t.tm_hour = hour;
    t.tm_min = minute + minutes_to_expire;
    t.tm_sec = second;
    t.tm_isdst = -1;
    
    mktime(&t);
    
    char buffer [80];
    strftime(buffer, 80, "%Y-%m-%dT%H:%M:%S.500", &t);

    return std::string(buffer);
}

time_t parse_date_string(std::string date) {
    int year, month, day, hour, minute, second, tail = 0;
    
    check(7 == sscanf(date.c_str(), "%d-%d-%dT%d:%d:%d.%d", &year, &month, &day, &hour, &minute, &second, &tail));
    
    tm t = {};
    
    t.tm_year = year - 1900;
    t.tm_mon = month - 1;
    t.tm_mday = day;
    t.tm_hour = hour;
    t.tm_min = minute;
    t.tm_sec = second;
    t.tm_isdst = 0;
    
#if defined(_WIN32)
    auto timestamp = _mkgmtime(&t)
#else // Assume POSIX
    auto timestamp = timegm(&t);
#endif
    
    return timestamp;
}

void get_transaction_smart_contract_abi(std::string api_url, std::string contact_name, std::string &smart_contract_abi)
{
	std::string response;
	json response_json;
	// get abi of smart contract
	if (CACHE_ABI_SMART_CONTRACT)
	{
		std::ifstream infile(CACHE_ABI_SMART_CONTRACT_FILENAME);
		if (infile.good())
		{
			smart_contract_abi =
				std::string((std::istreambuf_iterator<char>(infile)), std::istreambuf_iterator<char>());
		}
		else
		{
			CHECK(sendData("{\"account_name\":\"" + contact_name + "\"}",
						   api_url + "/chain/get_abi", response, CURL_IS_VERBOSE) == 1);
			CHECK(parseJSON(response, response_json) == 1);
			smart_contract_abi = response_json["abi"].dump();
			std::ofstream out(CACHE_ABI_SMART_CONTRACT_FILENAME);
			out << smart_contract_abi;
			out.close();
		}
	}
}

void get_init_data(std::string api_url, json &tnx_json, std::string &chain_id, unsigned char *chain_id_bytes)
{
	DEBUG("Retrieving node data");
	auto node_info = get_node_info(api_url, chain_id, chain_id_bytes);
    std::uint64_t last_irreversible_block_num = node_info["last_irreversible_block_num"].get<std::uint64_t>();
    std::string head_block_time = node_info["head_block_time"].get<std::string>();
    
	auto last_block_info = get_last_block_info(api_url, last_irreversible_block_num);
    
    tnx_json["ref_block_num"] = (std::uint16_t)(last_irreversible_block_num & 0xFFFF);
    tnx_json["expiration"] = get_expiration_date_string(head_block_time, 6);
    tnx_json["ref_block_prefix"] = last_block_info["ref_block_prefix"].get<std::uint32_t>();
    
	DEBUG("Done");
}

std::string build_signature(unsigned char *priv_key_bytes,
					 unsigned char *chain_id_bytes, const Buffer &packed_tnx)
{
	DEBUG("Building signature");
	
    SECP256K1_API::secp256k1_context *ctx = SECP256K1_API::secp256k1_context_create(SECP256K1_CONTEXT_SIGN);
	
    Buffer signature_feed;
    auto packed_tnx_size = packed_tnx.size();
    
	for (int i = 0; i < HASH_SHA256_SIZE; i++)		 //always 32 bytes because sha256
		signature_feed.push_back(chain_id_bytes[i]); //add chain id
	for (int i = 0; i < packed_tnx_size; i++)
		signature_feed.push_back(packed_tnx[i]); //add packed_tnx
	for (int i = 0; i < HASH_SHA256_SIZE; i++)	 //always 32 bytes because sha256
		signature_feed.push_back(0);			 //add contextFreeData
    
    Buffer signature_feed_sha256_bytes;
    Hash::SHA256(signature_feed, signature_feed_sha256_bytes);

	//build signature object:
	SECP256K1_API::secp256k1_ecdsa_recoverable_signature recoverable_signature;
	unsigned char recoverable_signature_serilized[64];

	//Sign the data following EOS standard:
	int recover_id;
	int loops = 0;
	do
	{
		// make new signature while canonical is false
		CHECK(SECP256K1_API::secp256k1_ecdsa_sign_recoverable(ctx, &recoverable_signature,
															  &signature_feed_sha256_bytes[0], priv_key_bytes, NULL,
															  &loops) == 1);
		CHECK(SECP256K1_API::secp256k1_ecdsa_recoverable_signature_serialize_compact(
				  ctx, recoverable_signature_serilized, &recover_id, &recoverable_signature) == 1);
		// std::cout << "Test canonical " << loops << " rec id: " << recover_id << std::endl;
		loops++;
	} while (is_canonical(recoverable_signature_serilized) == false);
    
    SECP256K1_API::secp256k1_context_destroy(ctx);

	int recover_id_2 = 27 + 4 + recover_id;

	// build the checksum data = recover_id_2 + sig + "K1"
	std::vector<unsigned char> checksum_data;
	checksum_data.push_back(recover_id_2);
	for (int i = 0; i < SIGNATURE_SERILIZED_SIZE; i++)
	{
		checksum_data.push_back(recoverable_signature_serilized[i]);
	}
	checksum_data.push_back('K');
	checksum_data.push_back('1');
    
    Buffer checksum;
    Hash::RIPEMD160(checksum_data, checksum);
    
	// build the final signature that will be passed in base58
	std::vector<unsigned char> base58_data;
	base58_data.push_back(recover_id_2); // add recovery id
	for (int i = 0; i < SIGNATURE_SERILIZED_SIZE; i++)
	{
		base58_data.push_back(recoverable_signature_serilized[i]); // add signature (r & s)
	}
	// add the 4 bytes checksum
	base58_data.push_back(checksum[0]);
	base58_data.push_back(checksum[1]);
	base58_data.push_back(checksum[2]);
	base58_data.push_back(checksum[3]);

	std::string base58_data_str(base58_data.begin(),
								base58_data.end()); // to string
	//69 = 1 + 64 + 4 bytes
	std::string eos_signature_str = "SIG_K1_" + eosio::to_base58((const char *)base58_data_str.c_str(),
																 69); // convert to base58
    return eos_signature_str;
}

json build_transaction(abieos_context *context,
                       Transaction * transaction,
					   std::vector<std::string> priv_key_bytes_vector,
					   unsigned char *chain_id_bytes)
{
    Buffer packed_tnx = transaction->getPackedData();
    int packed_tnx_size;
    
    json tnx_json;
    to_json(tnx_json, *transaction);
    
    // Add singatures of serializaed (packed) transaction
    for(auto bytesString : priv_key_bytes_vector) {
        unsigned char *priv_key_bytes = (unsigned char *) &bytesString[0];
        
        auto signature = build_signature(priv_key_bytes, chain_id_bytes, packed_tnx);
        
        tnx_json["signatures"].push_back(signature);
    }
    
    return tnx_json;
}

std::string send_transaction(std::string api_url,
                      json &tnx_json,
                      abieos_context *context)
{
    std::string response;
    std::string signatures = tnx_json["signatures"].dump();
        
	// send the transaction
	DEBUG("Sending transaction");
    DEBUG(tnx_json.dump().c_str());
    
    // Move to getName function
    auto transaction_contract =
        check_context(context, __LINE__, __FILE__, abieos_string_to_name(context, "transaction"));
    check_context(context, __LINE__, __FILE__,
                  abieos_set_abi(context, transaction_contract, Transaction::transactionAbi_str.c_str()));
    
    // pack  transaction with signature before push
    check_context(context, __LINE__, __FILE__,
                  abieos_json_to_bin_reorderable(context, transaction_contract, "transaction",
                                                 tnx_json.dump().c_str()));
    
    const char *signed_packed_tnx = check_context(context, __LINE__, __FILE__, abieos_get_bin_data(context)); // use abieos_get_bin_hex for hex
    int signed_packed_tnx_size = abieos_get_bin_size(context);
    
    std::cout << std::endl << toBase64UString(
                    toBuffer((unsigned char *) signed_packed_tnx,
                    (size_t)signed_packed_tnx_size)) << std::endl;
    
    std::vector<unsigned char> packed_tnx_vec_2(signed_packed_tnx,
                                                signed_packed_tnx + signed_packed_tnx_size);
    
    std::vector<unsigned char> packed_tnx_vec_2_hex = convertBytesToHexStr(packed_tnx_vec_2);
    std::string packed_tnx_2_hex(packed_tnx_vec_2_hex.begin(), packed_tnx_vec_2_hex.end());

    std::string data = "{\"signatures\":" + signatures +
    ", \"compression\":\"none\",\"packed_context_free_data\":\"\",\"packed_trx\":\"" +
    packed_tnx_2_hex + "\"}";
    
    std::cout << std::endl << data << std::endl << std::endl;
    CHECK(sendData(data, api_url + "/chain/push_transaction", response, CURL_IS_VERBOSE) == 1);
    
    DEBUG("Done");
    
    return response;
}

int test_transaction(std::string priv_key)
{

	// ProfilerStop();
	return 0;
}


void history_get_transaction(std::string api_url, json &tnx_json, std::string transaction_id, uint64_t blockNumHint)
{
    // next: get the last block info to retrieve : chain_id, block_num, block_prefix, expiration
    std::string response;
    std::string data = "{\"id\":\"" + transaction_id + "\", \"block_num_hint\":" + std::to_string(blockNumHint) + "}";
    
    std::cout << data << std::endl;
    
    CHECK(sendData(data, api_url + "/history/get_transaction", response, CURL_IS_VERBOSE) == 1);
    CHECK(parseJSON(response, tnx_json) == 1);
    std::cout << tnx_json.dump(1) << std::endl;
}
    
