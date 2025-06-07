
file(GLOB NGPFS_SOURCES
	"Main.cpp"
)

add_executable("ngpfs" ${NGPFS_SOURCES})
target_include_directories("ngpfs" PUBLIC ".")
target_link_libraries("ngpfs" "ngp-core")
