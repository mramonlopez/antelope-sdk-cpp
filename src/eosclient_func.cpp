#include <iostream>
#include <iomanip>
#include <sstream>
#include <curl/curl.h>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include <eosclient/eosclient_func.h>

/*Start sendata functions*/
struct WriteThis
{
    const char *readptr;
    size_t sizeleft;
};
static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string *)userp)->append((char *)contents, size * nmemb);
    return size * nmemb;
}
static size_t read_callback(void *dest, size_t size, size_t nmemb, void *userp)
{
    struct WriteThis *wt = (struct WriteThis *)userp;
    size_t buffer_size = size * nmemb;

    if (wt->sizeleft)
    {
        /* copy as much as possible from the source to the destination */
        size_t copy_this_much = wt->sizeleft;
        if (copy_this_much > buffer_size)
            copy_this_much = buffer_size;
        memcpy(dest, wt->readptr, copy_this_much);

        wt->readptr += copy_this_much;
        wt->sizeleft -= copy_this_much;
        return copy_this_much; /* we copied this many bytes */
    }

    return 0; /* no more data left to deliver */
}
/**
 * Send data with post method to an api endpoint.
 *   @return 1 if success, else 0
*/
int sendData(std::string data, std::string api_endpoint, std::string &response, bool isverbose)
{
    CURL *curl;
    CURLcode res;

    struct WriteThis wt;

    wt.readptr = data.c_str();
    wt.sizeleft = data.length();

    //std::cout << "Length data:" << (long)wt.sizeleft << std::endl;

    struct curl_slist *headers = NULL;
    std::string readBuffer;
    curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, api_endpoint.c_str());
        headers = curl_slist_append(headers, "Content-Type: application/octet-stream");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_callback);
        curl_easy_setopt(curl, CURLOPT_READDATA, &wt);

        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE_LARGE, (long)wt.sizeleft);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        // if (isverbose)
        //     curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if (isverbose)
        {
            // std::cout << "CURL STATUS CODE:" << res << std::endl;
            // std::cout << "Response:" << std::endl;
            // std::cout << readBuffer << std::endl;
            std::cout << "CURL: POST " << api_endpoint << " SENT=" << data.length() << " B"
                      << " RECEIVED=" << readBuffer.length() << " B" << std::endl;
        }

        response = readBuffer;
    }
    curl_global_cleanup();
    if (res != CURLE_OK)
    {
        std::cerr << "CURL STATUS CODE:" << res << std::endl;
        std::cerr << curl_easy_strerror(res) << std::endl;
        return 0;
    }
    else
    {
        return 1;
    }
}
/*End sendata functions*/

/*Util functions*/

/**
 * Parse a string into a JSON object
 *   @return 1 if success, else 0
*/
int parseJSON(std::string json_str, json &j)
{
    try
    {
        j = json::parse(json_str);
        return 1;
    }
    catch (json::exception &e)
    {
        std::cerr << "Error parsing JSON: " << std::endl
                  << e.what() << std::endl;
        return 0;
    }
}
/*Start define EOS chain endpoints functions*/
//TODO

/**
 * Convert bytes to hex
 * @author OracleChain
*/
std::vector<unsigned char> convertBytesToHexStr(const std::vector<unsigned char> &src)
{
    std::vector<unsigned char> dst;
    for (int i = 0; i < (int)src.size(); ++i)
    {
        unsigned char hex[3];
        unsigned char c = src.at(i);
        sprintf((char *)&hex[0], "%02x", c);
        dst.push_back(hex[0]);
        dst.push_back(hex[1]);
    }

    return dst;
}
/**
 * Convert hex to bytes
 * @author OracleChain
*/
std::vector<unsigned char> convertHexStrToBytes(const std::vector<unsigned char> &hex)
{
    std::vector<unsigned char> dst;
    for (int i = 0; i < (int)hex.size(); i += 2)
    {
        unsigned char c = (hex.at(i) % 32 + 9) % 25 * 16 + (hex.at(i + 1) % 32 + 9) % 25;
        dst.push_back(c);
    }

    return dst;
}

/**
 * Build sha256 from a vector
*/
std::string sha256Data(std::vector<uint8_t> message_vect, bool convert_hex_str)
{
    /////////////////////////////////////////////
    //Create a SHA-512 data Hash
    std::string digest_hex;
    HexEncoder encoder(new StringSink(digest_hex), false);
    std::string digest;

    SHA256 sha256;
    sha256.Update(message_vect.data(), message_vect.size());
    digest.resize(sha256.DigestSize());
    sha256.Final((unsigned char *)&digest[0]);
    if (convert_hex_str)
    {
        StringSource ss(digest, true, new Redirector(encoder));
        return digest_hex;
    }
    else
    {
        return digest;
    }
}
std::string sha256Data(std::string message, bool convert_hex_str)
{
    /////////////////////////////////////////////
    //Create a SHA-512 data Hash
    std::vector<uint8_t> message_vect(message.begin(), message.end());
    std::string digest_hex;
    HexEncoder encoder(new StringSink(digest_hex), false);
    std::string digest;

    SHA256 sha256;
    sha256.Update(message_vect.data(), message_vect.size());
    digest.resize(sha256.DigestSize());
    sha256.Final((unsigned char *)&digest[0]);
    if (convert_hex_str)
    {
        StringSource ss(digest, true, new Redirector(encoder));
        return digest_hex;
    }
    else
    {
        return digest;
    }
}
/**
 * Build RIPEMD-160 hash from a vector
*/
std::string RIPEMD160Data(std::vector<uint8_t> message_vect, bool convert_hex_str)
{
    /////////////////////////////////////////////
    //Create a RIPEMD-160 data Hash
    std::string digest_hex;
    HexEncoder encoder(new StringSink(digest_hex), false);
    std::string digest;

    RIPEMD160 ripemd;
    ripemd.Update(message_vect.data(), message_vect.size());
    digest.resize(ripemd.DigestSize());
    ripemd.Final((unsigned char *)&digest[0]);
    if (convert_hex_str)
    {
        StringSource ss(digest, true, new Redirector(encoder));
        return digest_hex;
    }
    else
    {
        return digest;
    }
}
/**
 * Build RIPEMD-160 hash from a string
*/
std::string RIPEMD160Data(std::string message, bool convert_hex_str)
{
    /////////////////////////////////////////////
    //Create a RIPEMD-160 data Hash
    std::vector<uint8_t> message_vect(message.begin(), message.end());
    std::string digest_hex;
    HexEncoder encoder(new StringSink(digest_hex), false);
    std::string digest;

    RIPEMD160 ripemd;
    ripemd.Update(message_vect.data(), message_vect.size());
    digest.resize(ripemd.DigestSize());
    ripemd.Final((unsigned char *)&digest[0]);
    if (convert_hex_str)
    {
        StringSource ss(digest, true, new Redirector(encoder));
        return digest_hex;
    }
    else
    {
        return digest;
    }
}
//http://www.cplusplus.com/forum/general/53397/
int chhex(char ch)
{
    if (isdigit(ch))
        return ch - '0';
    if (tolower(ch) >= 'a' && tolower(ch) <= 'f')
        return ch - 'a' + 10;
    return -1;
}
void HexStrToUchar(unsigned char *dest, const char *source, int bytes_n)
{
    for (bytes_n--; bytes_n >= 0; bytes_n--)
        dest[bytes_n] = 16 * chhex(source[bytes_n * 2]) + chhex(source[bytes_n * 2 + 1]);
}

//https://stackoverflow.com/a/14051107/11697589
//https://stackoverflow.com/questions/7639656/getting-a-buffer-into-a-stringstream-in-hex-representation/7639754#7639754
std::string UcharToHexStr(unsigned char *data, int len) //bytes to string
{
    //this was first:
    // std::stringstream ss;
    // for (int i = 0; i < data_length; ++i)
    //     ss << std::hex << (int)data[i];
    // std::string mystr = ss.str();

    //the following is better: IT FILLS WITH 0 !!!!
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (int i = 0; i < len; ++i)
        ss << std::setw(2) << static_cast<unsigned>(data[i]);
    return ss.str();
}
