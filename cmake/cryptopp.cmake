include(FindGit)
find_package(Git)

if (NOT Git_FOUND)
	message(FATAL_ERROR "Git not found!")
endif ()

include(FetchContent)

FetchContent_Declare(cryptopp
	GIT_REPOSITORY https://github.com/weidai11/cryptopp.git
	GIT_TAG master
)

FetchContent_GetProperties(cryptopp)

if(NOT cryptopp_POPULATED)
	FetchContent_Populate(cryptopp)

	# Copy an additional/replacement file into the populated source
	file(REMOVE_RECURSE ${cryptopp_SOURCE_DIR}/include)

	file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/cmake/cryptopp/CMakeLists.txt DESTINATION ${cryptopp_SOURCE_DIR})

	file(
		COPY ${cryptopp_SOURCE_DIR}
		DESTINATION ${cryptopp_SOURCE_DIR}/include
		FILES_MATCHING 
			PATTERN "*.h" 
			PATTERN ".git*" EXCLUDE 
			PATTERN "include" EXCLUDE 		
	)

	file(RENAME ${cryptopp_SOURCE_DIR}/include/cryptopp-src ${cryptopp_SOURCE_DIR}/include/cryptopp)
	
	add_subdirectory(${cryptopp_SOURCE_DIR} ${cryptopp_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()