include(FindGit)
find_package(Git)

if (NOT Git_FOUND)
	message(FATAL_ERROR "Git not found!")
endif ()

include(ExternalProject)
find_program(MAKE_EXE NAMES gmake nmake make)

set (SECP256K1 "secp256k1")
ExternalProject_Add(
	${SECP256K1}
	PREFIX            ${SECP256K1}
	GIT_REPOSITORY    https://github.com/bitcoin-core/secp256k1.git

	CONFIGURE_COMMAND sh -c "./autogen.sh && ./configure --enable-module-recovery"
	UPDATE_COMMAND    ""
	INSTALL_COMMAND   ""

	BUILD_IN_SOURCE   true
	BUILD_ALWAYS      OFF
	INSTALL_DIR       ${CMAKE_CURRENT_BINARY_DIR}/ext/${SECP256K1}

	BUILD_COMMAND     ${MAKE_EXE}
	INSTALL_COMMAND   ${MAKE_EXE} DESTDIR=${CMAKE_CURRENT_BINARY_DIR}/ext/${SECP256K1} PREFIX=/ install
)