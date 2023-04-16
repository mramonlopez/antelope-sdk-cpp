include(FindGit)
find_package(Git)

if (NOT Git_FOUND)
	message(FATAL_ERROR "Git not found!")
endif ()

include(FetchContent)

FetchContent_Declare(antelope-sdk-cpp
	GIT_REPOSITORY https://github.com/mramonlopez/antelope-sdk-cpp.git
	GIT_TAG cocos-examples
)

FetchContent_MakeAvailable(antelope-sdk-cpp)