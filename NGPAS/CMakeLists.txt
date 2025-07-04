project("NGPAS")


file(GLOB NGPAS_SOURCES
    "Backend/Assembler.cpp"
    "Backend/Directive.cpp"
    "Backend/Eval.cpp"
    "Backend/Instruction.cpp"
    "Frontend/AsmLexer.cpp"
    "Frontend/AsmPreProcessor.cpp"
    "Main.cpp"

    "../Utility/StringPool.cpp"
)

add_executable("ngpas" ${NGPAS_SOURCES})
target_include_directories("ngpas" PUBLIC "." "../Utility")
