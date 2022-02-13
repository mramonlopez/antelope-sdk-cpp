cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 17)

project(eosclient VERSION 0.1 LANGUAGES CXX C)
set(APP_NAME eosclient)

find_package(nlohmann_json 3.2.0 REQUIRED)

include(cmake/cryptopp.cmake)
include(cmake/abieos.cmake)
include(cmake/secp256k1.cmake)

set(SOURCE_FILES 
	src/app.cpp
	src/eos_client.cpp
)

add_executable(${APP_NAME} ${SOURCE_FILES})
add_dependencies(${APP_NAME} cryptopp abieos secp256k1)

ExternalProject_Get_Property(abieos INSTALL_DIR)
set(ABIEOS_INSTALL_DIR ${INSTALL_DIR})

ExternalProject_Get_Property(cryptopp INSTALL_DIR)
set(CRYPTOPP_INSTALL_DIR ${INSTALL_DIR})

ExternalProject_Get_Property(secp256k1 INSTALL_DIR)
set(SECP256K1_INSTALL_DIR ${INSTALL_DIR})

target_link_libraries(${APP_NAME} PRIVATE 
	nlohmann_json::nlohmann_json
	-L${CRYPTOPP_INSTALL_DIR}/lib
	-L${SECP256K1_INSTALL_DIR}/lib
)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${ABIEOS_INSTALL_DIR}/include
	${CRYPTOPP_INSTALL_DIR}/include
	${SECP256K1_INSTALL_DIR}/include
)