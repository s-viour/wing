cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake
  CACHE STRING "Vcpkg toolchain file")

project(
    wing
    VERSION 0.1.0
    LANGUAGES CXX
)

find_package(fmt CONFIG REQUIRED)
# find_package(tomlplusplus CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(reproc++ REQUIRED)

add_executable(wing
    src/main.cpp
    src/ninja.cpp
    src/template.cpp
    src/tools.cpp
)
target_include_directories(wing PUBLIC
    include
)
target_link_libraries(wing PRIVATE
    fmt::fmt
#   tomlplusplus::tomlplusplus
    spdlog::spdlog
    reproc++
)
set_target_properties(wing PROPERTIES
    CXX_STANDARD 17
)
