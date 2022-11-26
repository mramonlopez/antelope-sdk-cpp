# EOS C++ Client Library
This project is a fork of https://github.com/lucgerrits/EOS.IO-cpp-client created with the aim of using this EOS.io client as a library using cmake in other projects and on mobile platforms.

The project downloads all the repositories it needs through FetchContent, except for the curl library. This library must be provided externally, indicating its name in the **EOSCLIENT_CURL_NAME** variable. For example, in a cocos2d-x project, which already includes curl precompiles, we can include something like this in the main CMakeLists.txt file:

```cmake
include(${COCOS2DX_ROOT_PATH}/external/cmake/CocosExternalConfig.cmake)

set(EOSCLIENT_CURL_NAME "ext_curl")
set(CURL_INCLUDE_DIR ${COCOS2DX_ROOT_PATH}/external/curl/include/${platform_name})
set(CURL_LIBRARY ${COCOS2DX_ROOT_PATH}/external/curl/${platform_spec_path})

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/EOS.IO-cpp-client EXCLUDE_FROM_ALL)
add_dependencies(${APP_NAME} ${EOSCLIENT})
```

Instructions on how to build the project with make, as well as how to profile it, can be found in the original project.

## How to use
```cpp
std::string priv_key = "3ac4d7455 ... 4534575634ce6"; 
   
auto client = new EOSClient("http://testnet.waxsweden.org/v1/chain", priv_key, "testuser");

nlohmann::json data = json::parse("{\"my_name\":\"John\"}", data);

client->action("helloword", "hi", data);
```

## Tested platforms
- iOS + Cocos2d-x
- Android + Cocos2d-x
