cmake_minimum_required(VERSION 3.18)
set(CMAKE_CXX_STANDARD 17)

project(eosclient VERSION 0.1 LANGUAGES CXX C)
set(APP_NAME eosclient)

option(FABRIC_SDK_CPP_USE_EXTERNAL_JSON "Use an external JSON library" OFF)

include(cmake/cryptopp.cmake)
include(cmake/abieos.cmake)
include(cmake/secp256k1.cmake)
include(cmake/nlohman_json.cmake)

set(SOURCE_FILES 
	src/eos_client.cpp
)

add_library(${APP_NAME} ${SOURCE_FILES})
add_dependencies(${APP_NAME} cryptopp abieos secp256k1)

ExternalProject_Get_Property(abieos INSTALL_DIR)
set(ABIEOS_INSTALL_DIR ${INSTALL_DIR})

ExternalProject_Get_Property(cryptopp INSTALL_DIR)
set(CRYPTOPP_INSTALL_DIR ${INSTALL_DIR})

ExternalProject_Get_Property(secp256k1 INSTALL_DIR)
set(SECP256K1_INSTALL_DIR ${INSTALL_DIR})

target_link_libraries(${APP_NAME} PUBLIC 
	nlohmann_json::nlohmann_json
	${CRYPTOPP_INSTALL_DIR}/lib/libcryptopp.a
	${SECP256K1_INSTALL_DIR}/lib/libsecp256k1.a
	${ABIEOS_INSTALL_DIR}/libabieos.a
)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${ABIEOS_INSTALL_DIR}/include
	${CRYPTOPP_INSTALL_DIR}/include
	${SECP256K1_INSTALL_DIR}/include
)