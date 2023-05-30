# Antelope C++ Client Library
![Antelope SDK cpp Logo](/docs/antelope-sdk-cpp.png)

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
The easiest way to incorporate the Antelope network client into a project is by providing the client with the private key used for signing transactions. This eliminates the need to exit the application and request an external wallet to sign the transaction, thus reducing waiting time and improving the overall user experience. Moreover, this approach lowers the entry barrier for users who are less familiar with blockchain technology.

On the other hand, safeguarding a private key imposes greater security requirements on applications and places greater responsibility on the developer.

Here is an example that demonstrates how to send a signed transaction to the blockchain:
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
It is indeed possible to include multiple actions within the same transaction:
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
Furthermore, you have the flexibility to employ multiple signers per action or even utilize different signers for different actions:
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
If you don't want to manage users' private keys, there is the possibility of delegating the responsibility of signing and sending transactions to an external wallet application that supports the ESR protocol.

Currently, the only supported operations of the ESR protocol are transaction sending and login request, which are more than enough for the normal use of an app integrated with Antelope blockchains.

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
In the previous examples, we explored the possibility of instructing a wallet application to execute a callback to a specific URL. Mobile apps can register a custom URL scheme and inform the operating system that they can handle such calls.

Now, let's delve into the configuration process for iOS and Android applications to enable processing these callbacks.

The first step is to define a class that serves as a callback event delegate by implementing the SigningRequestCallbackDelegate interface. This interface includes a single method called onCallback. Within our code, we need to register an instance of this class as the callback delegate. In the example code provided, this registration occurs in the constructor. Note that only one delegate object can be assigned to the callback handler.

```cpp
// MyDelegate.hpp
class MyDelegate : public onikami::eosclient::SigningRequestCallbackDelegate {
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
#### Step 1
For Android, to register a custom scheme, we need to add the following code snippet to the AndroidManifest.xml file in our project:
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
#### Step 2
In the main Activity of the project, we also need to override the onNewIntent method and include code similar to the following to notify the delegate about the arrival of the callback:
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

#### Step 3
Lastly, we need to copy the folder com/onikami/antelope located in android/src/main/java/ to the root folder of your Java classes in your project. Note that in a cocos2d-x game, you can find this folder at <GAME ROOT PATH>/proj.android/app/src/.
### iOS
Now, let's move on to configuring the custom scheme in iOS by following these steps:

#### Step 1
After selecting the root of our project structure, click on the main target (1). Then, select the Info tab (2) and click on the "+" button in the URL Types section (3). Finally, fill in the necessary data.
![Custom scheme in iOS](/docs/custom_schema_ios.png)

#### Step 2
The next step involves adding code to the AppDelegate class in our project that notifies the delegate about the arrival of a callback.
```objective-c
- (BOOL)application:(UIApplication *)application openURL:(NSURL *)url sourceApplication:(NSString *)sourceApplication annotation:(id)annotation {
    onikami::eosclient::SigningRequestCallbackManager::getInstance()->onCallback([url.absoluteString UTF8String]);
}
```

### Parametros callback
The parameters that can be received in the query string of the callback are as follows:

- sig: Transaction ID as HEX-encoded string.
- tx: Block number hint (only present if transaction was broadcast).
- bn: Signer authority, aka account name.
- sa: Signer permission, e.g. "active".
- sp: Reference block num used when resolving request.
- rbn: Reference block id used when resolving request.
- rid: The originating signing request packed as a uri string.
- req: Expiration time used when resolving request.
- ex: The resolved chain id. 
- cid: All signatures 0-indexed as `sig0`, `sig1`, etc.
    
## Tested platforms
The currently supported platforms and SDKs are as follows:

- iOS + Cocos2d-x
- Android + Cocos2d-x

This doesn't mean that Antelope SDK cpp cannot be used with other SDKs and engines that support integration with native code.

## TODOs: 
- HTTPS is not yet supported due to integration issues with the OpenSSL library.
- Enhance the usage of the Atieso library.
- Cache ABIs definitions.
- Improve this documentation.
- Support desktop operating systems: Windows, Linux, and Mac.
- Support for Unreal Engine.
- Support for Godot Engine.
