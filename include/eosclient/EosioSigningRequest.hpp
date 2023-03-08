//
//  EosioSigningRequest.hpp
//  eosclient
//
//  Created by Manuel Ramon Lopez Torres on 30/12/22.
//

#ifndef EosioSigningRequest_hpp
#define EosioSigningRequest_hpp

#include <eosclient/types.hpp>
#include <eosclient/Action.hpp>
#include <eosclient/Transaction.hpp>
#include <variant>

namespace onikami
{
namespace eosclient
{

class IdentityV2 {
    PermissionLevel permission;
};

typedef std::variant<Action, std::vector<Action>, Transaction, IdentityV2> RequestVariantV2;

class RequestDataV2 {
public:
    static const uint8_t BROADCAST = 1 << 0;
    static const uint8_t BACKGROUND = 1 << 1;
    
    std::string chain_id = "";
    RequestVariantV2 req;
    uint8_t flags = 0;
    std::string callback = "";
    std::map<std::string, Buffer> info;
};

class EosioSigningRequest {
public:
    inline EosioSigningRequest(RequestDataV2 data) : data_(data) {};
    
    std::string encode(bool compress = false, bool slashes = true);
    Buffer getData();
    Buffer getSignatureData();
    
private:
    static const unsigned char version = 2;
    
    RequestDataV2 data_;
};

void to_json(nlohmann::json& j, const RequestDataV2& r);
void from_json(const nlohmann::json& j, RequestDataV2& r);

}
}

#endif /* EosioSigningRequest_hpp */
