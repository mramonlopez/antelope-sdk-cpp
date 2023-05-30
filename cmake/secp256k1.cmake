include(FindGit)
find_package(Git)

if (NOT Git_FOUND)
	message(FATAL_ERROR "Git not found!")
endif ()

include(FetchContent)

set(BUILD_TESTING 0)

FetchContent_Declare(secp256k1
	GIT_REPOSITORY https://github.com/bitcoin-core/secp256k1
	GIT_TAG v0.2.0)

FetchContent_GetProperties(secp256k1)
if(NOT secp256k1_POPULATED)
	FetchContent_Populate(secp256k1)
	
	file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/cmake/secp256k1/CMakeLists.txt DESTINATION ${secp256k1_SOURCE_DIR})
	
	add_subdirectory(${secp256k1_SOURCE_DIR} ${secp256k1_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

set_target_properties(secp256k1
                      PROPERTIES
                      ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
                      LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
                      FOLDER "Internal"
                      )