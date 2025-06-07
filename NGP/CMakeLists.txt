
file(GLOB NGP_SOURCES
    "Main.cpp"
)

add_executable("ngp" ${NGP_SOURCES})

target_link_libraries("ngp" "ngp-core")