include(FindGit)
find_package(Git)

if (NOT Git_FOUND)
	message(FATAL_ERROR "Git not found!")
endif ()

include(FetchContent)

FetchContent_Declare(eosclient
	GIT_REPOSITORY https://github.com/mramonlopez/antelope-sdk-cpp.git
	GIT_TAG axmol
)

FetchContent_MakeAvailable(eosclient)