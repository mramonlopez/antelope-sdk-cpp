//
//  EosioSigningRequest.hpp
//  eosclient
//
//  Created by Manuel Ramon Lopez Torres on 30/12/22.
//

#ifndef EosioSigningRequest_hpp
#define EosioSigningRequest_hpp

#include <eosclient/types.hpp>


namespace onikami
{
namespace eosclient
{

class EosioSigningRequest {
public:
    inline EosioSigningRequest() {};
    
    std::string encode(bool compress = false, bool slashes = true);
    Buffer getData();
    Buffer getSignatureData();
    
private:
    static const unsigned char version = 2;
};

}
}

#endif /* EosioSigningRequest_hpp */
