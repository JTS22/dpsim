configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/include/dpsim/Config.h.in
	${CMAKE_CURRENT_BINARY_DIR}/include/dpsim/Config.h
)

add_subdirectory(src)

if(WITH_PYBIND)
	add_subdirectory(src/pybind)
endif()

if(DPSIM_BUILD_EXAMPLES)
	add_subdirectory(examples)
endif()