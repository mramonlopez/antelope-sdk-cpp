cmake_minimum_required(VERSION 3.18)
set(CMAKE_CXX_STANDARD 17)

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
)

set(HEADER_FILES
    include/eosclient/eosclient.hpp
	include/eosclient/eosclient_func.h
	include/eosclient/eosclient_lib.h
    include/eosclient/base58.hpp
    include/eosclient/Hash.hpp
    include/eosclient/types.hpp
)

source_group(src FILES ${SOURCE_FILES})
source_group(include FILES ${HEADER_FILES})

add_library(${LIB_NAME} ${SOURCE_FILES} ${HEADER_FILES})
add_dependencies(${LIB_NAME}
    ${EOSCLIENT_OPENSSL_NAME}
    ${EOSCLIENT_OPENSSL_CRYPTO_NAME}
    ${EOSCLIENT_CURL_NAME}
    secp256k1
    abieos)

target_link_libraries(${LIB_NAME} PUBLIC 
	nlohmann_json::nlohmann_json
	abieos
	secp256k1
    ${EOSCLIENT_OPENSSL_NAME}
    ${EOSCLIENT_OPENSSL_CRYPTO_NAME}
	${EOSCLIENT_CURL_NAME}
)

target_include_directories(${LIB_NAME} PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${abieos_SOURCE_DIR}/include
	${secp256k1_SOURCE_DIR}/include
	${CURL_INCLUDE_DIR}
	${json_SOURCE_DIR}/include
)
