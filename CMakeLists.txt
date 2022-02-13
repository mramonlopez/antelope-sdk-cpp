cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 17)

project(eosclient VERSION 0.1 LANGUAGES CXX C)
set(APP_NAME eosclient)

find_package(nlohmann_json 3.2.0 REQUIRED)

include(cmake/cryptopp.cmake)
include(cmake/abieos.cmake)

set(SOURCE_FILES 
	src/app.cpp
	src/eos_client.cpp
)

add_executable(${APP_NAME} ${SOURCE_FILES})
add_dependencies(${APP_NAME} cryptopp abieos)

ExternalProject_Get_Property(abieos INSTALL_DIR)
include_directories(
	${INSTALL_DIR}/include
)

ExternalProject_Get_Property(cryptopp INSTALL_DIR)
include_directories(
	${INSTALL_DIR}/include
)

target_link_libraries(${APP_NAME} PRIVATE 
	nlohmann_json::nlohmann_json
	-L${INSTALL_DIR}/lib
)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/include
	# ${CMAKE_CURRENT_SOURCE_DIR}/abieos/external/rapidjson/include
	${CMAKE_CURRENT_SOURCE_DIR}/secp256k1/include
)