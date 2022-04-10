include(FindGit)
find_package(Git)

if (NOT Git_FOUND)
	message(FATAL_ERROR "Git not found!")
endif ()

include(FetchContent)

set(BUILD_TESTING 0)

FetchContent_Declare(secp256k1
	GIT_REPOSITORY https://github.com/mramonlopez/secp256k1/
	GIT_TAG master)

FetchContent_GetProperties(secp256k1)
if(NOT secp256k1_POPULATED)
	FetchContent_Populate(secp256k1)
	add_subdirectory(${secp256k1_SOURCE_DIR} ${secp256k1_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()