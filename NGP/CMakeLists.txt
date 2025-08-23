
get_property(NGPSRC GLOBAL PROPERTY NGP_SOURCES)
get_property(NGPLIBS GLOBAL PROPERTY NGP_LIBRARIES)

add_executable("ngpv1" "Main.cpp" ${NGPSRC})
target_link_libraries("ngpv1" ${NGPLIBS})
