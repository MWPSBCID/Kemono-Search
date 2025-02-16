cmake_minimum_required (VERSION 2.6)
project (KemonoSearch)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)

include(FetchContent)

# Fetch cURL
FetchContent_Declare(curl
    URL https://curl.se/download/curl-8.5.0.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP true
    OVERRIDE_FIND_PACKAGE # this one
)
FetchContent_MakeAvailable(curl)

# Fetch cURLpp
FetchContent_Declare(
    curlpp
    GIT_REPOSITORY https://github.com/jpbarrette/curlpp.git
    GIT_TAG master
)
FetchContent_MakeAvailable(curlpp)

add_executable(KemonoSearch main.cpp parser.cpp jsonExporter.cpp stringFuncs.cpp globalVariables.cpp userFunctions.cpp)

target_link_libraries(KemonoSearch PRIVATE CURL::libcurl curlpp)
