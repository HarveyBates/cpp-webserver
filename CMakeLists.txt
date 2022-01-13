cmake_minimum_required(VERSION "3.19.2")

# For automatic setup of CPM.cmake
set(CPM_DOWNLOAD_VERSION 0.28.4)
if(CPM_SOURCE_CACHE)
	set(CPM_DOWNLOAD_LOCATION "${CPM_SOURCE_CACHE}/cpm/CPM_${CPM_DOWNLOAD_VERSION}.cmake")
elseif(DEFINED ENV{CPM_SOURCE_CACHE})
	set(CPM_DOWNLOAD_LOCATION "$ENV{CPM_SOURCE_CACHE}/cpm/CPM_${CPM_DOWNLOAD_VERSION}.cmake")
else()
	set(CPM_DOWNLOAD_LOCATION "${CMAKE_BINARY_DIR}/cmake/CPM_${CPM_DOWNLOAD_VERSION}.cmake")
endif()
if(NOT (EXISTS ${CPM_DOWNLOAD_LOCATION}))
	message(STATUS "Downloading CPM.cmake to ${CPM_DOWNLOAD_LOCATION}")
	file(DOWNLOAD
		https://github.com/TheLartians/CPM.cmake/releases/download/v${CPM_DOWNLOAD_VERSION}/CPM.cmake
		${CPM_DOWNLOAD_LOCATION}
	)
endif()
include(${CPM_DOWNLOAD_LOCATION})

project("cpp-webserver")

set(NAME_SRC
	src/main.cpp
	src/server.cpp
	src/connection.cpp)

set(NAME_HEADERS
	include/main.hpp
	include/server.hpp
	include/connection.hpp
	include/crypto.hpp)

set(EXE_OUTPUT_PATH
	${CMAKE_BINARY_DIR}/bin)

# Threads
find_package(Threads REQUIRED)

# OpenSSL setup
# If OPENSSL not found use:
# brew upgrade openssl
# brew link --force openssl
find_package(OpenSSL REQUIRED)
if(OpenSSL)
	target_link_libraries(${PROJECT_NAME} ${OPENSSL_LIBRARIES})
	target_include_directories(${PROJECT_NAME} PRIVATE
		${OPENSSL_INCLUDE_DIR})
endif() # OPENSSL

# ASIO setup using CPM.cmake
include(cmake/CPM_0.28.4.cmake)
CPMAddPackage(
	NAME asio
	VERSION asio-1-20-0
	URL https://github.com/chriskohlhoff/asio/archive/refs/tags/asio-1-18-1.zip
)
add_library(asio INTERFACE)
target_include_directories(asio SYSTEM INTERFACE ${asio_SOURCE_DIR}/asio/include)
target_compile_definitions(asio INTERFACE ASIO_STANDALONE ASIO_NO_DEPRECATED)
target_link_libraries(asio INTERFACE Threads::Threads)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(${PROJECT_NAME} ${NAME_SRC} ${NAME_HEADERS})

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

target_link_libraries(${PROJECT_NAME} Threads::Threads)
target_link_libraries(${PROJECT_NAME} OpenSSL::SSL)
target_link_libraries(${PROJECT_NAME} OpenSSL::Crypto)
target_link_libraries(${PROJECT_NAME} asio)
