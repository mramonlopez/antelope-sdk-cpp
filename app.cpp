/**
 * @file app.cpp
 * @date 10/2020
 * @author Luc Gerrits <luc.gerrits@univ-cotedazur.fr>
 * @brief Client for helloworld smart contract.
 * @version 0.1
 */

// I followed some instructions from: https://github.com/EOSIO/eos/issues/5341

// #include "gperftools/profiler.h"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <chrono>

#include "abieos/include/eosio/abi.hpp"
#include "abieos/include/eosio/crypto.hpp"
#include "abieos/include/eosio/name.hpp"
#include "abieos/include/eosio/ship_protocol.hpp"
#include "abieos/include/eosio/stream.hpp"
#include "abieos/include/eosio/time.hpp"
#include "abieos/include/eosio/to_json.hpp"
#include "abieos/include/eosio/varint.hpp"
#include "abieos/include/eosio/abieos.h"
#include "eos_client.h"

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

void showParseError(char *argv[])
{
	std::cerr << "EOS.IO cpp client using hello world smart contract.\n"
			  << "Usage: " << argv[0] << " <option(s)>\n"
			  << "Options:\n"
			  << "\t-h,--help\t\tShow this help message\n"
			  << "\t--push   \t\tBuild transaction AND send it\n"
			  << "\t--dry-run\t\tBuild transaction BUT don't send it" << std::endl;
}
int parseArgs(int argc, char *argv[], std::string &command)
{
	if (argc < 2)
	{
		showParseError(argv);
		return 1;
	}
	else
	{
		if (strcmp(argv[1], "--push") == 0)
		{
			command = "push";
		}
		else if (strcmp(argv[1], "--dry-run") == 0)
		{
			command = "dry-run";
		}
		else
		{
			showParseError(argv);
			return 1;
		}
		return 0;
	}
}
/**
 * This is the bitcoin base58check
 */
std::string base58check(std::string data)
{
	// Verifiable with "hello world" -> "3vQB7B6MrGQZaxCuFg4oh"
	std::string data_sha256 = sha256Data(data);
	std::string data_checksum = sha256Data(data_sha256); // checksum is sha256Data(sha256Data(data))[0:4]
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

void init_transaction(std::string priv_key, unsigned char *priv_key_bytes, json &tnx_json,
					  SECP256K1_API::secp256k1_context *&ctx)
{
	//init some variables:

	// std::string priv_key = "5KQwrPbwdL6PhXujxW37FSSQZ1JiwsST4cqQzDeyXtP79zkvFD3"; //WTF: wrong format -> is in WIF format
	//Use: https://learnmeabitcoin.com/technical/wif
	HexStrToUchar(priv_key_bytes, priv_key.c_str(), HASH_SHA256_SIZE);
	ctx = SECP256K1_API::secp256k1_context_create(SECP256K1_CONTEXT_SIGN);
	// init final tnx with example of json:
	CHECK(parseJSON(R"(
		{
			"actions": [
				{
					"account":"helloworld",
					"authorization":[
						{
							"actor":"helloworld",
							"permission":"active"
						}
					],
					"data":"00408A97721AA36A06486579797979",
					"name":"hi"
				}
			],
			"context_free_actions":[],
			"context_free_data":[],
			"delay_sec":0,
			"expiration":"2020-06-02T20:24:36",
			"max_cpu_usage_ms":0,
			"max_net_usage_words":0,
			"ref_block_num":14207,
			"ref_block_prefix":1438248607,
			"transaction_extensions":[]
		}
	)",
					tnx_json) == 1);

	// Set the initial tnx values:
	tnx_json["actions"][0]["account"] = EOS_SMART_CONTRACT_ACCOUNT_NAME;
	tnx_json["actions"][0]["authorization"][0]["actor"] = EOS_SMART_CONTRACT_ACCOUNT_NAME;
	tnx_json["actions"][0]["name"] = EOS_SMART_CONTRACT_ACTION;
}
void clear_program(abieos_context *context, SECP256K1_API::secp256k1_context *ctx)
{
	abieos_destroy(context);
	SECP256K1_API::secp256k1_context_destroy(ctx);
}
void get_node_info(json &tnx_json, std::string &chain_id, unsigned char *chain_id_bytes)
{
	// first get EOS node info
	std::string response;
	json response_json;
	if (sendData("", getEOSChainEndpoint() + "/get_info", response, CURL_IS_VERBOSE) == 1)
	{
		CHECK(parseJSON(response, response_json) == 1);
		if (!response_json["error"].is_null()) // if this exist then there is an error
		{
			std::cerr << "ERROR: Cannot access endpoint : " << getEOSChainEndpoint() << std::endl;
			std::cout << response_json.dump(1) << std::endl;
			exit(1);
		}
		std::cout << "Endpoint OK : " << getEOSChainEndpoint() << std::endl;
		// std::cout << response_json.dump(1) << std::endl;
	}
	else
	{
		std::cerr << "ERROR: Cannot access endpoint : " << getEOSChainEndpoint() << std::endl;
		exit(1);
	}
	chain_id = response_json["chain_id"]; // not used for the moment
	HexStrToUchar(chain_id_bytes, chain_id.c_str(), 32);

	std::uint64_t ref_block_num = response_json["last_irreversible_block_num"].get<std::uint64_t>();
	tnx_json["ref_block_num"] = (std::uint16_t)(ref_block_num & ((std::uint16_t)0xFFFFFFFF));
}
void get_last_block_info(json &tnx_json)
{
	// next: get the last block info to retrieve : chain_id, block_num, block_prefix, expiration
	std::string response;
	json response_json;
	CHECK(sendData("{\"block_num_or_id\":\"" + std::to_string(tnx_json["ref_block_num"].get<std::uint64_t>()) +
					   "\"}",
				   getEOSChainEndpoint() + "/get_block", response, CURL_IS_VERBOSE) == 1);
	CHECK(parseJSON(response, response_json) == 1);
	// std::cout << response_json.dump(1) << std::endl;

	std::uint32_t ref_block_prefix = response_json["ref_block_prefix"].get<std::uint32_t>();
	tnx_json["ref_block_prefix"] = ref_block_prefix;
	tnx_json["expiration"] = response_json["timestamp"].get<std::string>();
	// increase expiration by one minute:
	std::string tmp_exp = tnx_json["expiration"];
	int new_minute_1 = (int)tmp_exp[15] + 2;
	// TODO: Test minute integer > 9 -> else increase hours
	if (new_minute_1 > (int)'9')
	{
		new_minute_1 = (int)'0';
		int new_minute_2 = (int)tmp_exp[14] + 2;
		tmp_exp[14] = (char)new_minute_2;
	}
	tmp_exp[15] = (char)new_minute_1;
	tnx_json["expiration"] = tmp_exp;
}
void get_transaction_smart_contract_abi(std::string &smart_contract_abi)
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
			CHECK(sendData("{\"account_name\":\"" + std::string(EOS_SMART_CONTRACT_ACCOUNT_NAME) + "\"}",
						   getEOSChainEndpoint() + "/get_abi", response, CURL_IS_VERBOSE) == 1);
			CHECK(parseJSON(response, response_json) == 1);
			smart_contract_abi = response_json["abi"].dump();
			std::ofstream out(CACHE_ABI_SMART_CONTRACT_FILENAME);
			out << smart_contract_abi;
			out.close();
		}
	}
}
void get_init_data(json &tnx_json, std::string &chain_id, unsigned char *chain_id_bytes,
				   std::string &smart_contract_abi)
{
	DEBUG("Retrieving node data");
	get_node_info(tnx_json, chain_id, chain_id_bytes);
	get_last_block_info(tnx_json);
	get_transaction_smart_contract_abi(smart_contract_abi);
	DEBUG("Done");
}
void build_transaction_action_binary(abieos_context *context, json &tnx_json, std::string smart_contract_abi)
{
	DEBUG("Building action data binary");
	uint64_t helloworld = check_context(context, __LINE__, __FILE__, abieos_string_to_name(context, "helloworld"));
	check_context(context, __LINE__, __FILE__, abieos_set_abi(context, helloworld, smart_contract_abi.c_str()));
	check_context(context, __LINE__, __FILE__,
				  abieos_json_to_bin_reorderable(context, helloworld, "hi",
												 "{\"message\":\"Heyyyy\",\"from\":\"helloworld\"}"));
	tnx_json["actions"][0]["data"] = check_context(context, __LINE__, __FILE__, abieos_get_bin_hex(context));
	DEBUG("Done");
}
void build_packed_transaction(abieos_context *context, json tnx_json, uint64_t &transaction_contract, char *&packed_tnx,
							  int &packed_tnx_size)
{
	// load tnx into the abi json->bin:
	DEBUG("Building packed transaction");
	std::ifstream infile(
		"transaction.abi.json"); //abieos uses the transaction contract to build the packed transaction
	//Note:
	//A (smart) contract will have to same steps when building the contract action binary (it is based on the contract abi json)
	std::string transactionAbi_str =
		std::string((std::istreambuf_iterator<char>(infile)), std::istreambuf_iterator<char>());
	transaction_contract =
		check_context(context, __LINE__, __FILE__, abieos_string_to_name(context, "transaction"));
	check_context(context, __LINE__, __FILE__,
				  abieos_set_abi(context, transaction_contract, transactionAbi_str.c_str()));
	check_context(context, __LINE__, __FILE__,
				  abieos_json_to_bin_reorderable(context, transaction_contract, "transaction",
												 tnx_json.dump().c_str()));

	packed_tnx = (char *)check_context(context, __LINE__, __FILE__,
									   abieos_get_bin_data(context)); // use abieos_get_bin_hex for hex
	packed_tnx_size = abieos_get_bin_size(context);
	DEBUG("Done");
}
void build_signature(SECP256K1_API::secp256k1_context *ctx, json &tnx_json, unsigned char *priv_key_bytes,
					 unsigned char *chain_id_bytes, char *packed_tnx, int packed_tnx_size)
{
	// Good signature (recoverable & canonical secp256k1 sig):
	DEBUG("Building signature");
	//build data that need to be signed:
	std::vector<unsigned char> signature_feed;
	for (int i = 0; i < HASH_SHA256_SIZE; i++)		 //always 32 bytes because sha256
		signature_feed.push_back(chain_id_bytes[i]); //add chain id
	for (int i = 0; i < packed_tnx_size; i++)
		signature_feed.push_back(packed_tnx[i]); //add packed_tnx
	for (int i = 0; i < HASH_SHA256_SIZE; i++)	 //always 32 bytes because sha256
		signature_feed.push_back(0);			 //add contextFreeData

	std::string signature_feed_sha256 = sha256Data(signature_feed, true);
	unsigned char signature_feed_sha256_bytes[HASH_SHA256_SIZE];
	HexStrToUchar(signature_feed_sha256_bytes, signature_feed_sha256.c_str(), HASH_SHA256_SIZE);

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
															  signature_feed_sha256_bytes, priv_key_bytes, NULL,
															  &loops) == 1);
		CHECK(SECP256K1_API::secp256k1_ecdsa_recoverable_signature_serialize_compact(
				  ctx, recoverable_signature_serilized, &recover_id, &recoverable_signature) == 1);
		// std::cout << "Test canonical " << loops << " rec id: " << recover_id << std::endl;
		loops++;
	} while (is_canonical(recoverable_signature_serilized) == false);

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
	std::string checksum_data_str(checksum_data.begin(), checksum_data.end());
	std::string checksum = RIPEMD160Data(checksum_data_str);
	char const *checksum_char = checksum.c_str();
	// build the final signature that will be passed in base58
	std::vector<unsigned char> base58_data;
	base58_data.push_back(recover_id_2); // add recovery id
	for (int i = 0; i < SIGNATURE_SERILIZED_SIZE; i++)
	{
		base58_data.push_back(recoverable_signature_serilized[i]); // add signature (r & s)
	}
	// add the 4 bytes checksum
	base58_data.push_back(checksum_char[0]);
	base58_data.push_back(checksum_char[1]);
	base58_data.push_back(checksum_char[2]);
	base58_data.push_back(checksum_char[3]);

	std::string base58_data_str(base58_data.begin(),
								base58_data.end()); // to string
	//69 = 1 + 64 + 4 bytes
	std::string eos_signature_str = "SIG_K1_" + eosio::to_base58((const char *)base58_data_str.c_str(),
																 69); // convert to base58
	// DEBUG("EOS sig : ");
	// DEBUG(eos_signature_str.c_str());
	//set the signature
	tnx_json["signatures"].push_back(eos_signature_str);
	DEBUG("Done");
}
void build_transaction(abieos_context *context, json &tnx_json, std::string smart_contract_abi,
					   uint64_t &transaction_contract, char *&packed_tnx, int &packed_tnx_size,
					   SECP256K1_API::secp256k1_context *ctx, unsigned char *priv_key_bytes,
					   unsigned char *chain_id_bytes)
{
	build_transaction_action_binary(context, tnx_json, smart_contract_abi);
	build_packed_transaction(context, tnx_json, transaction_contract, packed_tnx, packed_tnx_size);
	build_signature(ctx, tnx_json, priv_key_bytes, chain_id_bytes, packed_tnx, packed_tnx_size);
}
void send_transaction(std::string command, json &tnx_json, abieos_context *context, uint64_t transaction_contract,
					  std::string eos_signature_str)
{
	// send the transaction
	DEBUG("Sending transaction");
	if (command == "push")
	{
		std::string response;
		json response_json;
		// pack  transaction with signature before push
		check_context(context, __LINE__, __FILE__,
					  abieos_json_to_bin_reorderable(context, transaction_contract, "transaction",
													 tnx_json.dump().c_str()));
		const char *signed_packed_tnx =
			check_context(context, __LINE__, __FILE__,
						  abieos_get_bin_data(context)); // use abieos_get_bin_hex for hex
		int signed_packed_tnx_size = abieos_get_bin_size(context);
		std::vector<unsigned char> packed_tnx_vec_2(signed_packed_tnx,
													signed_packed_tnx + signed_packed_tnx_size);
		std::vector<unsigned char> packed_tnx_vec_2_hex = convertBytesToHexStr(packed_tnx_vec_2);
		std::string packed_tnx_2_hex(packed_tnx_vec_2_hex.begin(), packed_tnx_vec_2_hex.end());

		CHECK(sendData("{\"signatures\":[\"" + eos_signature_str +
						   "\"], \"compression\":none,\"packed_context_free_data\":\"\",\"packed_trx\":\"" +
						   packed_tnx_2_hex + "\"}",
					   getEOSChainEndpoint() + "/push_transaction", response, CURL_IS_VERBOSE) == 1);
		CHECK(parseJSON(response, response_json) == 1);
		DEBUG("Done");
		if (response_json["transaction_id"].is_null())
		{
			std::cout << "transaction_id: " << response_json.dump(1) << std::endl;
		}
		else
		{
			std::cout << "transaction_id: " << response_json["transaction_id"].dump() << std::endl;
		}
	}
	else
	{
		std::cout << "Mode: " << command << std::endl;
	}
}
int main(int argc, char *argv[])
{
	// ProfilerStart("profile.txt");
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	std::chrono::steady_clock::time_point end;
	std::string command = "";
	if (int exit = parseArgs(argc, argv, command))
		return exit;

	// global variables:
	json tnx_json; // final json of the tnx that will be sended
	std::string priv_key = "d2653ff7cbb2d8ff129ac27ef5781ce68b2558c41a74af1f2ddca635cbeef07d";
	unsigned char priv_key_bytes[32];
	SECP256K1_API::secp256k1_context *ctx;
	init_transaction(priv_key, priv_key_bytes, tnx_json, ctx);

	// get raw code and smart contact abi
	// CHECK(sendData("{\"code\":\"" + std::string(EOS_SMART_CONTRACT_ACCOUNT_NAME) + "\",
	// \"action\":\"" + std::string(EOS_SMART_CONTRACT_ACTION) + "\",
	// \"args\":{\"message\":\"Heyyyy\",\"from\":\"helloworld\"}}", getEOSChainEndpoint() +
	// "/abi_json_to_bin", response, CURL_IS_VERBOSE) == 1); CHECK(parseJSON(response, response_json)
	// == 1); tnx_json["actions"][0]["data"] = response_json["binargs"]; std::cout << "binargs:" <<
	// tnx_json["actions"][0]["data"] << std::endl;

	// get required keys needed to sign a transaction.
	// CHECK(sendData("{\"transaction\":" + tnx_json.dump() +", \"available_keys\":[\"" + pub_key +
	// "\"]}", getEOSChainEndpoint() + "/get_required_keys", response, CURL_IS_VERBOSE) == 1);
	// CHECK(parseJSON(response, response_json) == 1);
	// std::cout << "get required keys:" << response_json.dump(1) << std::endl;

	std::string smart_contract_abi;
	abieos_context *context = check(abieos_create());
	char *packed_tnx; //buffer of 10000
	int packed_tnx_size;
	uint64_t transaction_contract;
	std::string last_irreversible_block_num;
	std::string chain_id;
	unsigned char chain_id_bytes[HASH_SHA256_SIZE];

	end = std::chrono::steady_clock::now();
	get_init_data(tnx_json, chain_id, chain_id_bytes, smart_contract_abi);
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
	std::cout << std::setw(20) << std::left << "* private key: " << std::setw(30) << priv_key << std::endl;

	begin = std::chrono::steady_clock::now();
	//Sept 3:
	build_transaction(context, tnx_json, smart_contract_abi, transaction_contract, packed_tnx, packed_tnx_size, ctx,
					  priv_key_bytes, chain_id_bytes);
	end = std::chrono::steady_clock::now();
	std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count()
			  << "[µs]" << std::endl;
	begin = std::chrono::steady_clock::now();
	send_transaction(command, tnx_json, context, transaction_contract,
					 tnx_json["signatures"][0].get<std::string>());
	end = std::chrono::steady_clock::now();
	std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count()
			  << "[µs]" << std::endl;
	clear_program(context, ctx);
	// ProfilerStop();
	return 0;
}