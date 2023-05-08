//
//  SigningRequestCallbackManager.hpp
//  Antelope SDK
//
//  Created by Manuel Ramon Lopez Torres on 5/6/23.
//

#ifndef SigningRequestCallbackManager_hpp
#define SigningRequestCallbackManager_hpp

#include <string>
#include <eosclient/SigningRequestCallbackDelegate.hpp>

namespace onikami {
namespace eosclient {

class SigningRequestCallbackManager {
public:
    static SigningRequestCallbackManager* getInstance();
    void setDelegate(SigningRequestCallbackDelegate* delegate);
    void onCallback(const std::string url);

private:
    SigningRequestCallbackDelegate* delegate_;

    SigningRequestCallbackManager() {};
};

}}

#endif /* SigningRequestCallbackManager_hpp */