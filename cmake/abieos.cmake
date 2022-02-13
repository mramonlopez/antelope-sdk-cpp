include(FindGit)
find_package(Git)

if (NOT Git_FOUND)
	message(FATAL_ERROR "Git not found!")
endif ()

find_program(MAKE_EXE NAMES gmake nmake make)

include(ExternalProject)

set (ABIEOS "abieos")
set (ABIEOS_ONLY_LIBRARY on)

ExternalProject_Add(
	${ABIEOS}
	PREFIX          ${ABIEOS}
	GIT_REPOSITORY 	https://github.com/EOSIO/abieos.git
	GIT_SUBMODULES_RECURSE true

	BUILD_IN_SOURCE   true
	BUILD_ALWAYS      OFF
	INSTALL_DIR       ${CMAKE_CURRENT_BINARY_DIR}/ext/${ABIEOS}
	INSTALL_COMMAND   cp -R include external/rapidjson/include libabieos.a libabieos.so ${CMAKE_CURRENT_BINARY_DIR}/ext/${ABIEOS}
)