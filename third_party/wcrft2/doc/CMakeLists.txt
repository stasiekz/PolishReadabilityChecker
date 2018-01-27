find_package(Doxygen REQUIRED)

add_custom_target(documentation ALL
	COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile > doxygen.log
	COMMAND ${CMAKE_BUILD_TOOL} --directory=./latex > ./latex.log
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	COMMENT "Building documentation for libwcrft and wcrft-app")
