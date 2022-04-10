include(FindGit)
find_package(Git)

if (NOT Git_FOUND)
	message(FATAL_ERROR "Git not found!")
endif ()

include(ExternalProject)
find_program(MAKE_EXE NAMES gmake nmake make)

set (CRYPTOPP "cryptopp")
ExternalProject_Add(
	${CRYPTOPP}
	PREFIX            ${CRYPTOPP}
	GIT_REPOSITORY https://github.com/weidai11/cryptopp.git

	CONFIGURE_COMMAND ""
	UPDATE_COMMAND    ""
	INSTALL_COMMAND   ""

	BUILD_IN_SOURCE   true
	BUILD_ALWAYS      OFF
	INSTALL_DIR       ${CMAKE_CURRENT_BINARY_DIR}/ext/${CRYPTOPP}

	BUILD_COMMAND     sh -c "source TestScripts/setenv-ios.sh IOS_SDK=iPhone IOS_CPU=arm64 && ${MAKE_EXE}"
	INSTALL_COMMAND   ${MAKE_EXE} DESTDIR=${CMAKE_CURRENT_BINARY_DIR}/ext/${CRYPTOPP} PREFIX=/ install
)