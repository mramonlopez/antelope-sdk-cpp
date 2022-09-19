//////////////////////////////////////////////////////////////////
//To show errors:
void abort(void) __attribute__((__noreturn__));
#define TEST_FAILURE(msg)                                        \
    do                                                           \
    {                                                            \
        fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, msg); \
        throw std::runtime_error(msg);                           \
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


#define CACHE_ABI_SMART_CONTRACT 1
#define CACHE_ABI_SMART_CONTRACT_FILENAME ".cache.abi"
#define HASH_SHA256_SIZE 32
#define SIGNATURE_SERILIZED_SIZE 64
#define CURL_IS_VERBOSE true

#ifndef EOS_CLIENT
#define EOS_CLIENT

#include <vector>
#include <nlohmann/json.hpp>
using json = nlohmann::json;


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
