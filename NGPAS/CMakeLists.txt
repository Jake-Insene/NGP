project("NGPAS")

file(GLOB NGPAS_SOURCES
    "Backend/Assembler.cpp"
    "Backend/Directive.cpp"
    "Backend/Eval.cpp"
    "Backend/Instruction.cpp"
    "Frontend/AsmLexer.cpp"
    "Frontend/AsmPreProcessor.cpp"
    "Main.cpp"
)

add_executable("ngpas" ${NGPAS_SOURCES})

target_include_directories("ngpas" PUBLIC ".")
target_link_libraries("ngpas" "ngp-core")