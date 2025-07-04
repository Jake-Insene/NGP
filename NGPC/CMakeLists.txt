project("NGPC")

file(GLOB NGPAS_SOURCES
    "Backend/CCompiler.cpp"
    "Frontend/CLexer.cpp"
    "Frontend/CPreProcessor.cpp"
    "Frontend/CParser.cpp"
    "Main.cpp"

    "../Utility/StringPool.cpp"
)

add_executable("ngpc"${NGPAS_SOURCES})

target_include_directories("ngpc" PUBLIC "." "../Utility")
target_link_libraries("ngpc")