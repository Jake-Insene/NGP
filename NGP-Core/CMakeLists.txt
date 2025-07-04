
file(GLOB NGP_CPU_SOURCES
    "CPU/NGPV1/NGPV1.cpp"
)

file(GLOB NGP_CORE_SOURCES
    "CPU/CPUCore.cpp"
    "CPU/JIT/X86/X86JIT.cpp"

    "IO/IO.cpp"
    "IO/Display/Display.cpp"
    "IO/DMA/DMA.cpp"
    "IO/GU/GU.cpp"
    "IO/IRQ/IRQ.cpp"
    "IO/Pad/Pad.cpp"
    "IO/USI/USI.cpp"
    
    "Memory/Bus.cpp"
    
    "Video/GUDevice.cpp"
    "Video/VGU/VGU.cpp"
    "Video/VGU/VGUQueue.cpp"
    "Video/VGU/VRasterizer.cpp"
    "Video/OpenGL/GLGU.cpp"
    
    "Emulator.cpp"
)


if(WIN32)
    file(GLOB NGP_CORE_PLATFORM_SOURCE
        "Platform/Win32/Win32OS.cpp"
        "Platform/Win32/Win32Time.cpp"

        "Video/Win32/Win32Window.cpp"
        
        "Video/D3D11/D3D11GU.cpp"
        "Video/D3D12/D3D12GU.cpp"
    )

    set(NGP_CORE_PLATFORM_LIBRARIES "d3d12" "d3d11" "dxgi" "d3dcompiler")
else()
    file(GLOB NGP_CORE_PLATFORM_SOURCE
        "Platform/Linux/LinuxOS.cpp"

        "Video/Linux/LinuxWindow.cpp"
    )
    set(NGP_CORE_PLATFORM_LIBRARIES "")
endif()

set_property(GLOBAL PROPERTY NGP_SOURCES ${NGP_CORE_SOURCES} ${NGP_CORE_PLATFORM_SOURCE} ${NGP_CPU_SOURCES})
set_property(GLOBAL PROPERTY NGP_LIBRARIES ${NGP_CORE_PLATFORM_LIBRARIES})
