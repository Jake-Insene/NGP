cmake_minimum_required(VERSION 3.12)
project("NGP")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_COMPILER "/usr/bin/g++-10")

include_directories("NGP-Core")

file(GLOB NGP_CORE_SOURCES
    "NGP-Core/CPU/CPU.cpp"
    "NGP-Core/IO/DMA.cpp"
    "NGP-Core/IO/IO.cpp"
    "NGP-Core/IO/Pad.cpp"
    "NGP-Core/Memory/Bus.cpp"
    "NGP-Core/Video/GPU.cpp"
)

if(WIN32)
else()
    file(GLOB NGP_CORE_PLATFORM_SOURCE
    "NGP-Core/Memory/Bus.cpp"
    "NGP-Core/Platform/Impl/LinuxThread.cpp"
    "NGP-Core/Platform/Impl/LinuxTime.cpp"
    )
endif()

add_library("ngp-core" STATIC ${NGP_CORE_SOURCES})

file(GLOB NGP_SOURCES
    "NGP/Main.cpp"
)

add_executable("ngp" ${NGP_SOURCES})
target_link_libraries("ngp" "ngp-core")

file(GLOB NGPAS_SOURCES
    "NGPAS/Backend/Assembler.cpp"
    "NGPAS/Backend/Directive.cpp"
    "NGPAS/Backend/Eval.cpp"
    "NGPAS/Backend/Instruction.cpp"
    "NGPAS/Frontend/Lexer.cpp"
    "NGPAS/Frontend/PreProcessor.cpp"
    "NGPAS/Main.cpp"
)

add_executable("ngpas" ${NGPAS_SOURCES})
target_include_directories("ngpas" PUBLIC "NGPAS")
target_link_libraries("ngpas" "ngp-core")
