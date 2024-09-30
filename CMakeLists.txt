cmake_minimum_required(VERSION 3.12)
project("NGP")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

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
    file(GLOB NGP_CORE_PLATFORM_SOURCE
        "NGP-Core/Platform/Win32/Win32OS.cpp"
        "NGP-Core/Platform/Win32/Win32Thread.cpp"
        "NGP-Core/Platform/Win32/Win32Time.cpp"
        "NGP-Core/Video/Win32/Win32Window.cpp"
        "NGP-Core/Video/D3D12/D3D12GPU.cpp"
    )

    set(NGP_CORE_PLATFORM_LIBRARIES "d3d12")
else()
    file(GLOB NGP_CORE_PLATFORM_SOURCE
        "NGP-Core/Platform/Linux/LinuxOS.cpp"
        "NGP-Core/Platform/Linux/LinuxThread.cpp"
        "NGP-Core/Platform/Linux/LinuxTime.cpp"
    )
    set(NGP_CORE_PLATFORM_LIBRARIES "")
endif()

add_library("ngp-core" STATIC ${NGP_CORE_SOURCES} ${NGP_CORE_PLATFORM_SOURCE})
target_link_libraries("ngp-core" ${NGP_CORE_PLATFORM_LIBRARIES})

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
