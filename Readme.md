# Antelope C++ Client Library
![Tux, the Linux mascot](/docs/antelope-sdk-cpp.png) 

This project was a fork of https://github.com/lucgerrits/EOS.IO-cpp-client created with the aim of using this EOS.io client as a library using cmake in other projects and on mobile platforms.

## Table of content
- [How to include in you project](#How-to-include-in-you-project)
- [Using client with embeded keys](#Using-client-with-embeded-keys)
    - [Multi-action transaction](#Multi-action-transaction)
    - [Multiple signer](#Multiple-signer)
- [Using client with a external wallet app](#Using-client-with-a-external-wallet-app)
    - [Send login request to external wallet](#Send-login-request-to-external-wallet)
    - [Send transaction request to external wallet](#Send-transaction-request-to-external-wallet)
- [Process wallet callbacks](#Process-wallet-callbacks)
    - [Android](#Android)
    - [iOS](#iOS)
- [Tested platforms](#Tested-platforms)
- [TODOs:](#TODOs:)

## How to include in you project 
The project downloads all the repositories it needs through FetchContent, except for the curl library. This library must be provided externally, indicating its name in the **EOSCLIENT_CURL_NAME** variable. For example, in a cocos2d-x project, which already includes curl precompiles, we can include something like this in the main CMakeLists.txt file:

```cmake
include(${COCOS2DX_ROOT_PATH}/external/cmake/CocosExternalConfig.cmake)

set(EOSCLIENT_CURL_NAME "ext_curl")
set(CURL_INCLUDE_DIR ${COCOS2DX_ROOT_PATH}/external/curl/include/${platform_name})
set(CURL_LIBRARY ${COCOS2DX_ROOT_PATH}/external/curl/${platform_spec_path})

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/EOS.IO-cpp-client EXCLUDE_FROM_ALL)
add_dependencies(${APP_NAME} ${EOSCLIENT})
```

## Using client with embeded keys 
```cpp
// Get key from a secure storage
std::string priv_key = "3ac .... ce6"; 
   
// Create client
Authorizer auth = Authorizer("testuser", priv_key, "active");
auto client = new onikami::eosclient::EOSClient("http://testnet.waxsweden.org/v1/chain", {auth});

// Action data
nlohmann::json data = json::parse("{\"my_name\":\"John\"}", data);

// Create action
onikani::eosclient::PermissionLevel permission;
permission.actor = "testuser";
permission.permission = "active";

onikami::eosclient::Action action = {"helloword", "hi", data, {permission}};

// Get action ABI from network
client->setABIAction(&action)

// Create transaction
onikami::eosclient::Transaction transaction;
transaction.actions.push_back(action);

// Send to network
auto result = client->sendTransaction(transaction);

std::cout << result.c_srt() << std::endl;
```

### Multi-action transaction 
```cpp
onikani::eosclient::PermissionLevel permission;
permission.actor = "testuser";
permission.permission = "active";

nlohmann::json data = json::parse("{\"my_name\":\"John\"}", data);

Action action1 = {"helloword", "hi", json::object(), {permission}};
Action action2 = {"helloword", "bye", json::object(), {permission}};
    
Transaction transaction;

client->setABIAction(&action1);
transaction.actions.push_back(action1);

client->setABIAction(&action2);
transaction.actions.push_back(action2);
```

###  Multiple signer 
```cpp
Authorizer auth1 = Authorizer("testuser1", priv_key1, "active");
Authorizer auth2 = Authorizer("testuser2", priv_key2, "active");
    
auto client = new EOSClient(this->getAPIEndpoint(), {auth1, auth2});

onikani::eosclient::PermissionLevel permission;
permission.actor = "testuser1";
permission.permission = "active";

// 
onikami::eosclient::Action action = {"helloword", "hi", data, {permission, {"testuser1", "active"}}};
```

## Using client with a external wallet app 

### Send login request to external wallet 
```cpp
onikami::eosclient::IdentityV2 identity;
identity.permission.actor = "testuser1";
identity.permission.permission = "active";

onikami::eosclient::RequestDataV2 data;

// https://github.com/eosio-eps/EEPs/blob/master/EEPS/eep-7.md#Chain-Aliases
data.chain_id = "1064487b3cd1a897ce03ae5b6a865651747e2e152090f99c1d19d44e01aea5a4"; // WAX
data.callback = "esrtestapp://login/?sa={{sa}}&sp={{sp}}";
data.req = identity;

auto request = new onikami::eosclient::EosioSigningRequest(data);
auto url = request->encode();

// Open URL. In a cocos2d-x app:
Application::getInstance()->openURL(url);
```

### Send transaction request to external wallet 
```cpp
// Action data
nlohmann::json data = json::parse("{\"my_name\":\"John\"}", data);

// Create action
onikani::eosclient::PermissionLevel permission;
permission.actor = "testuser";
permission.permission = "active";

onikami::eosclient::Action action = {"helloword", "hi", data, {permission}};

// Create client without authorizers
auto client = new onikami::eosclient::EOSClient("http://testnet.waxsweden.org/v1/chain", {});

// Get action ABI from network
client->setABIAction(&action)

// Create transaction
onikami::eosclient::Transaction transaction;
transaction.actions.push_back(action);

onikami::eosclient::RequestDataV2 data;

// https://github.com/eosio-eps/EEPs/blob/master/EEPS/eep-7.md#Chain-Aliases
data.chain_id = this->chain_id_ = "4667b205c6838ef70ff7988f6e8257e8be0e1284a2f59699054a018f743b1d11"; // TELOS;
data.req = transaction;
data.callback = "esrtestapp://result?tx={{tx}}";
// Ask the wallet to send the transaction to the network after signing it
data.flags = onikami::eosclient::RequestDataV2::BROADCAST;

auto request = new onikami::eosclient::EosioSigningRequest(data);
auto url = request->encode();

// Open URL. In a cocos2d-x app:
Application::getInstance()->openURL(url);
```

### Process wallet callbacks 
TODO: calback fields

```cpp
// MyDelegate.hpp
class MyDelegate : public cocos2d::Scene, public onikami::eosclient::SigningRequestCallbackDelegate {
public:
    virtual void onCallback(const onikami::eosclient::SigningRequestCallback signingRequestCallback);

    MyDelegate();
}

// MyDelegate.cpp
MyDelegate::MyDelegate() {
    SigningRequestCallbackManager::getInstance()->setDelegate(this);
}

void MyDelegate::onCallback(const SigningRequestCallback signingRequestCallback) {
    if (signingRequestCallback.host == "login") {
        std::cout << "Actor: " << signingRequestCallback.sa.c_srt() << std::endl;
        std::cout << "Permission: " << signingRequestCallback.sp.c_srt() << std::endl;
    }
}
```

### Android 

- In AndroidManifest.xml, add this in activity section:
```XML
<intent-filter android:label="Signing Request Example">
    <action android:name="android.intent.action.VIEW" />

    <category android:name="android.intent.category.DEFAULT" />
    <category android:name="android.intent.category.BROWSABLE" />

    <data
        android:scheme="esrtestapp"
        android:host="login" />
</intent-filter>
```

- In the main Activity class add

```java
import com.onikami.antelope.SigningRequestCallbackManager;

.
.
.

@Override
public void onNewIntent(Intent intent) {
    super.onNewIntent(intent);
    setIntent(intent);

    Uri data = intent.getData();

    if (data != null) {
        SigningRequestCallbackManager m = new SigningRequestCallbackManager();
        String url = data.toString();

        m.onCallback(url);
    }
}
```

*Note: In a cocos2d-x game, you can locate this file in \<GAME ROOT PATH\>/proj.android/app/src/org/cocos2dx/cpp/AppActivity.java*

- Copy the folder *com/onikami/antelope* located in *android/src/main/java/* to your project java classes root folder
*Note: In a cocos2d-x game, you can locate this folder in \<GAME ROOT PATH\>/proj.android/app/src/*

### iOS 


## Tested platforms 
- iOS + Cocos2d-x
- Android + Cocos2d-x

## TODOs: 
- https is not yet supported
- 
