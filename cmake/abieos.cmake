include(FindGit)
find_package(Git)

if (NOT Git_FOUND)
	message(FATAL_ERROR "Git not found!")
endif ()

include(FetchContent)

FetchContent_Declare(abieos
	GIT_REPOSITORY https://github.com/mramonlopez/abieos.git
	GIT_TAG mondev
)

FetchContent_GetProperties(abieos)

if(NOT abieos_POPULATED)
	FetchContent_Populate(abieos)

	add_subdirectory(${abieos_SOURCE_DIR} ${abieos_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()