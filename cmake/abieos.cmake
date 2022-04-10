include(FindGit)
find_package(Git)

if (NOT Git_FOUND)
	message(FATAL_ERROR "Git not found!")
endif ()

include(FetchContent)

set(BUILD_TESTING 0)

FetchContent_Declare(abieos
	GIT_REPOSITORY https://github.com/EOSIO/abieos.git
	GIT_TAG master)

FetchContent_GetProperties(abieos)
if(NOT abieos_POPULATED)
	FetchContent_Populate(abieos)
	add_subdirectory(${abieos_SOURCE_DIR} ${abieos_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()