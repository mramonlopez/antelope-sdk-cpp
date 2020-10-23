//////////////////////////////////////////////////////////////////
//To show errors:
void abort(void) __THROW __attribute__((__noreturn__));
#define TEST_FAILURE(msg)                                        \
    do                                                           \
    {                                                            \
        fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, msg); \
        abort();                                                 \
    } while (0)
#define EXPECT(x, c) __builtin_expect((x), (c))
#define CHECK(cond)                                        \
    do                                                     \
    {                                                      \
        if (EXPECT(!(cond), 0))                            \
        {                                                  \
            TEST_FAILURE("test condition failed: " #cond); \
        }                                                  \
    } while (0)
//////////////////////////////////////////////////////////////////
#define DEBUG(msg)                                               \
    do                                                           \
    {                                                            \
        fprintf(stdout, "%s:%d: %s\n", __FILE__, __LINE__, msg); \
    } while (0)

#define EOS_ENDPOINT "http://localhost"
// #define EOS_ENDPOINT "http://chrisserverfr.ddns.net"
#define EOS_ENDPOINT_CHAIN_PORT 8888
#define EOS_ENDPOINT_CHAIN_VERSION "/v1/chain"
#define EOS_SMART_CONTRACT_ACCOUNT_NAME "helloworld"
#define EOS_SMART_CONTRACT_ACTION "hi"
#define CACHE_ABI_SMART_CONTRACT 1
#define CACHE_ABI_SMART_CONTRACT_FILENAME ".cache.abi"
#define HASH_SHA256_SIZE 32
#define SIGNATURE_SERILIZED_SIZE 64
#define CURL_IS_VERBOSE true

#ifndef EOS_CLIENT
#define EOS_CLIENT

#include <vector>
#include "json/single_include/nlohmann/json.hpp"
using json = nlohmann::json;

//Crypto and hashing stuff:
#include "secp256k1/include/secp256k1.h"
#include "secp256k1/include/secp256k1_recovery.h"
//crypto++
#include "cryptopp/cryptlib.h"
#include "cryptopp/osrng.h"
using CryptoPP::AutoSeededRandomPool;
#include "cryptopp/hex.h"
using CryptoPP::HexEncoder;
#include "cryptopp/sha.h"
using CryptoPP::SHA256;
using CryptoPP::SHA512;
#include "cryptopp/ripemd.h"
using CryptoPP::RIPEMD160;
#include "cryptopp/filters.h"
using CryptoPP::Redirector;
using CryptoPP::StringSink;
using CryptoPP::StringSource;

int sendData(std::string data, std::string api_endpoint, std::string &response, bool isverbose = false);

std::string getEOSChainEndpoint();
int parseJSON(std::string json_str, json &j);

std::vector<unsigned char> convertBytesToHexStr(const std::vector<unsigned char> &src);
std::vector<unsigned char> convertHexStrToBytes(const std::vector<unsigned char> &hex);

std::string sha256Data(std::vector<uint8_t> message_vect, bool convert_hex_str = false);
std::string sha256Data(std::string message, bool convert_hex_str = false);
std::string RIPEMD160Data(std::vector<uint8_t> message_vect, bool convert_hex_str = false);
std::string RIPEMD160Data(std::string message, bool convert_hex_str = false);
void HexStrToUchar(unsigned char *dest, const char *source, int bytes_n);
std::string UcharToHexStr(unsigned char *data, int len);

#endif