cmake_minimum_required(VERSION 3.12)
project("NGP")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Release>:>")

include_directories("NGP-Core")

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


file(GLOB NGP_CORE_SOURCES
    "NGP-Core/CPU/CPUCore.cpp"
    "NGP-Core/IO/IO.cpp"
    "NGP-Core/IO/Debug/Debug.cpp"
    "NGP-Core/IO/DMA/DMA.cpp"
    "NGP-Core/IO/GU/GU.cpp"
    "NGP-Core/IO/IRQ/IRQ.cpp"
    "NGP-Core/IO/Pad/Pad.cpp"
    "NGP-Core/Memory/Bus.cpp"
    "NGP-Core/Video/GU.cpp"
    "NGP-Core/Video/VGU/VGU.cpp"
    "NGP-Core/Video/OpenGL/GLGU.cpp"
    "NGP-Core/Emulator.cpp"
)


if(WIN32)
    file(GLOB NGP_CORE_PLATFORM_SOURCE
        "NGP-Core/Platform/Win32/Win32OS.cpp"
        "NGP-Core/Platform/Win32/Win32Thread.cpp"
        "NGP-Core/Platform/Win32/Win32Time.cpp"
        "NGP-Core/Video/Win32/Win32Window.cpp"
        "NGP-Core/Video/D3D11/D3D11GU.cpp"
        "NGP-Core/Video/D3D12/D3D12GU.cpp"
    )

    set(NGP_CORE_PLATFORM_LIBRARIES "d3d12" "d3d11" "dxgi" "d3dcompiler")
else()
    file(GLOB NGP_CORE_PLATFORM_SOURCE
        "NGP-Core/Platform/Linux/LinuxOS.cpp"
        "NGP-Core/Platform/Linux/LinuxThread.cpp"
        "NGP-Core/Platform/Linux/LinuxTime.cpp"
        "NGP-Core/Video/Linux/LinuxWindow.cpp"
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
add_custom_command(TARGET "ngp"
    POST_BUILD COMMAND ${CMAKE_BINARY_DIR}/ngpas ${CMAKE_CURRENT_SOURCE_DIR}/BIOS/BIOS.asm
)
