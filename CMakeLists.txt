cmake_minimum_required(VERSION 3.18)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(eosclient VERSION 0.1 LANGUAGES CXX C)
set(LIB_NAME eosclient)

option(EOSCLIENT_USE_EXTERNAL_JSON "Use an external JSON library" OFF)
option(ABIEOS_NO_INT128 "disable use of __int128" ON)

IF(!EOSCLIENT_CURL_NAME)
    set(EOSCLIENT_CURL_NAME "curl")
ENDIF(!EOSCLIENT_CURL_NAME)

IF(!EOSCLIENT_OPENSSL_NAME)
    set(EOSCLIENT_OPENSSL_NAME "ssl")
ENDIF(!EOSCLIENT_OPENSSL_NAME)

IF(!EOSCLIENT_OPENSSL_CRYPTO_NAME)
    set(EOSCLIENT_OPENSSL_CRYPTO_NAME "crypto")
ENDIF(!EOSCLIENT_OPENSSL_CRYPTO_NAME)

# find_package(curl REQUIRED)
include(cmake/abieos.cmake)
include(cmake/secp256k1.cmake)
include(cmake/nlohman_json.cmake)

set(SOURCE_FILES
    src/eosclient.cpp
    src/eosclient_func.cpp
    src/eosclient_lib.cpp
    src/base58.cpp
    src/Hash.cpp
    src/types.cpp
    src/EosioSigningRequest.cpp
    src/SigningRequestCallbackManager.cpp
    src/Action.cpp
    src/Transaction.cpp
    src/PermissionLevel.cpp
    src/EdUrlParser.cpp
)

set(HEADER_FILES
    include/eosclient/eosclient.hpp
    include/eosclient/eosclient_func.hpp
    include/eosclient/eosclient_lib.hpp
    include/eosclient/base58.hpp
    include/eosclient/Hash.hpp
    include/eosclient/types.hpp
    include/eosclient/Transaction.hpp
    include/eosclient/Action.hpp
    include/eosclient/PermissionLevel.hpp
    include/eosclient/EosioSigningRequest.hpp
    include/eosclient/SigningRequestCallbackManager.hpp
    include/eosclient/SigningRequestCallback.hpp
    include/eosclient/SigningRequestCallbackDelegate.hpp
    include/eosclient/EdUrlParser.h
)

IF(ANDROID)
    list(APPEND SOURCE_FILES
            android/jni/com_onikami_antelope_SigningRequestCallbackManager.cpp
    )
    list(APPEND HEADER_FILES
            android/jni/com_onikami_antelope_SigningRequestCallbackManager.h
    )
ENDIF()

set(RES_FILES
    res/ca-bundle.txt
)

# INSTALL(FILES ${RES_FILES} DESTINATION ${CMAKE_INSTALL_PREFIX}/res)
file(COPY ${RES_FILES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

source_group(src FILES ${SOURCE_FILES})
source_group(include FILES ${HEADER_FILES})
source_group(res FILES ${RES_FILES})

add_library(${LIB_NAME} ${SOURCE_FILES} ${HEADER_FILES} ${RES_FILES})
add_dependencies(${LIB_NAME}
    ${EOSCLIENT_OPENSSL_NAME}
    ${EOSCLIENT_OPENSSL_CRYPTO_NAME}
    ${EOSCLIENT_CURL_NAME}
    secp256k1
    abieos
    nlohmann_json::nlohmann_json)

target_link_libraries(${LIB_NAME} PUBLIC
    nlohmann_json::nlohmann_json
    abieos
    secp256k1
    ${EOSCLIENT_OPENSSL_NAME}
    ${EOSCLIENT_OPENSSL_CRYPTO_NAME}
    ${EOSCLIENT_CURL_NAME}
)

target_compile_features(${LIB_NAME} PRIVATE cxx_std_17)

target_include_directories(${LIB_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/android/jni
    ${abieos_SOURCE_DIR}/include
    ${secp256k1_SOURCE_DIR}/include
    ${CURL_INCLUDE_DIR}
    ${json_SOURCE_DIR}/include
)

set_target_properties(${LIB_NAME}
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    FOLDER "Internal"
    MACOSX_BUNDLE TRUE
    RESOURCE "${RES_FILES}"
)

# set(LOCATION_FOR_THE_APP $<TARGET_BUNDLE_DIR:${LIB_NAME}>)

# get_target_property(APPLICATION_BIN ${LIB_NAME} LOCATION) 
# get_dotapp_dir(${APPLICATION_BIN} APPLICATION_APP_LOCATION) 
# add_custom_command(
#     TARGET ${PROJECT_NAME} 
#     POST_BUILD COMMAND cp -r "${CMAKE_SOURCE_DIR}/assets" "${APPLICATION_APP_LOCATION}")

# add_custom_command(
#     OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${LOCATION_FOR_THE_APP} 
#     DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/res" 
#     COMMAND -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/res ${CMAKE_CURRENT_BINARY_DIR}/${LOCATION_FOR_THE_APP})

# add_custom_target(copy_assets
#     COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/res ${CMAKE_CURRENT_BINARY_DIR}/res
# )
# add_dependencies(mytarget copy_assets)
