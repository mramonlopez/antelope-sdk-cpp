cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 17)

project(eosclient VERSION 0.1 LANGUAGES CXX C)
set(APP_NAME eosclient)


find_package(nlohmann_json 3.2.0 REQUIRED)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${CMAKE_CURRENT_SOURCE_DIR}/abieos/include
	${CMAKE_CURRENT_SOURCE_DIR}/abieos/external/rapidjson/include
	${CMAKE_CURRENT_SOURCE_DIR}/cryptopp
	${CMAKE_CURRENT_SOURCE_DIR}/secp256k1/include
)

set(SOURCE_FILES 
	src/app.cpp
	src/eos_client.cpp
)

add_executable(${APP_NAME} ${SOURCE_FILES})

target_link_libraries(${APP_NAME} PRIVATE 
	nlohmann_json::nlohmann_json
)

add_subdirectory(abieos)