cmake_minimum_required(VERSION 3.16)
project(sendspind VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# sendspin-cpp: player + controller + metadata roles only (slim the binary)
set(SENDSPIN_ENABLE_COLOR OFF CACHE BOOL "" FORCE)
set(SENDSPIN_ENABLE_ARTWORK OFF CACHE BOOL "" FORCE)
set(SENDSPIN_ENABLE_VISUALIZER OFF CACHE BOOL "" FORCE)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_Declare(sendspin
  GIT_REPOSITORY https://github.com/Sendspin/sendspin-cpp.git
  GIT_TAG bf9e085)
FetchContent_MakeAvailable(sendspin)

find_package(ALSA REQUIRED)
find_package(CURL REQUIRED)
find_package(nlohmann_json REQUIRED)

# dns_sd from libavahi-compat-libdnssd
find_library(DNSSD_LIB NAMES dns_sd)
if(NOT DNSSD_LIB)
  message(FATAL_ERROR "libavahi-compat-libdnssd (dns_sd) not found")
endif()

add_library(sendspind_lib STATIC
  src/options.cpp
  # later tasks append their src/*.cpp here
)
target_include_directories(sendspind_lib PUBLIC src)
target_link_libraries(sendspind_lib PUBLIC
  sendspin ALSA::ALSA CURL::libcurl nlohmann_json::nlohmann_json ${DNSSD_LIB})

add_executable(sendspind src/main.cpp)
target_link_libraries(sendspind PRIVATE sendspind_lib)

enable_testing()
add_subdirectory(tests)
