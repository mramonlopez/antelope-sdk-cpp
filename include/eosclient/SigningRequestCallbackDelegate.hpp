//
//  SigningRequestCallbackDelegate.hpp
//  Antelope SDK
//
//  Created by Manuel Ramon Lopez Torres on 5/6/23.
//

#ifndef SigningRequestCallbackDelegate_hpp
#define SigningRequestCallbackDelegate_hpp

#include <eosclient/SigningRequestCallback.hpp>

namespace onikami {
namespace eosclient {

class SigningRequestCallbackDelegate {
public:
    virtual void onCallback(const SigningRequestCallback signingRequestCallback) = 0;
};

}}

#endif /* SigningRequestCallbackDelegate_hpp */