include(FindGit)
find_package(Git)

if (NOT Git_FOUND)
	message(FATAL_ERROR "Git not found!")
endif ()

include(FetchContent)

FetchContent_Declare(cryptopp
	GIT_REPOSITORY https://github.com/mramonlopez/cryptocpp-lib
	GIT_TAG master
)

FetchContent_GetProperties(cryptopp)

if(NOT cryptopp_POPULATED)
	FetchContent_Populate(cryptopp)
	
	add_subdirectory(${cryptopp_SOURCE_DIR} ${cryptopp_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()