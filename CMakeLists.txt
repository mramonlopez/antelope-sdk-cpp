cmake_minimum_required(VERSION 3.18)
set(CMAKE_CXX_STANDARD 17)

project(eosclient VERSION 0.1 LANGUAGES CXX C)
set(LIB_NAME eosclient)

option(EOSCLIENT_USE_EXTERNAL_JSON "Use an external JSON library" OFF)

IF(!EOSCLIENT_CURL_NAME)
	set(EOSCLIENT_CURL_NAME "curl")
ENDIF(!EOSCLIENT_CURL_NAME)

# find_package(curl REQUIRED)

include(cmake/cryptopp.cmake)
include(cmake/abieos.cmake)
include(cmake/secp256k1.cmake)
include(cmake/nlohman_json.cmake)

set(SOURCE_FILES 
	src/eosclient.cpp
	src/eosclient_func.cpp
)

set(HEADER_FILES
    include/eosclient/eosclient.h
	include/eosclient/eosclient_func.h
)

source_group(src FILES ${SOURCE_FILES})
source_group(include FILES ${HEADER_FILES})

add_library(${LIB_NAME} ${SOURCE_FILES} ${HEADER_FILES})
add_dependencies(${LIB_NAME} ${EOSCLIENT_CURL_NAME} secp256k1 cryptopp abieos)

target_link_libraries(${LIB_NAME} PUBLIC 
	nlohmann_json::nlohmann_json
	abieos
	secp256k1
	cryptopp
)

target_include_directories(${LIB_NAME} PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${abieos_SOURCE_DIR}/include
	${cryptopp_SOURCE_DIR}/include
	${secp256k1_SOURCE_DIR}/include
	${CURL_INCLUDE_DIR}
	${json_SOURCE_DIR}/include
)
