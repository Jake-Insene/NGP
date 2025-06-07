cmake_minimum_required(VERSION 3.21)
project("NGP")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Release>:>")
endif()

include_directories("NGP-Core")

add_subdirectory("NGP-Core")
add_subdirectory("NGPAS")
add_subdirectory("NGP")
add_subdirectory("NGPFS")

add_custom_command(
    OUTPUT BIOS/BIOS.BIN
    COMMAND ngpas ${CMAKE_SOURCE_DIR}/BIOS/BIOS.asm
    DEPENDS ngpas BIOS/BIOS.asm
    COMMENT "Building BIOS..."
    VERBATIM
)

add_custom_target(BIOS ALL DEPENDS BIOS/BIOS.BIN)

add_dependencies(BIOS "ngpas")
