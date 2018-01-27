PROJECT(wcrft)

set(WCRFT_INSTALL_DATA_DIR share/wcrft)

set(libwcrft_VER_MAJOR 2)
set(libwcrft_VER_MINOR 0)
set(libwcrft_VER_PATCH 0)
set(libwcrft_VERSION "${libwcrft_VER_MAJOR}.${libwcrft_VER_MINOR}.${libwcrft_VER_PATCH}")

set(WCRFT_PATH_SEPARATOR ";")
set(WCRFT_DATA_DIR ".;./config;./model;")
set(WCRFT_DATA_DIR ${WCRFT_DATA_DIR} "${CMAKE_INSTALL_PREFIX}/${WCRFT_INSTALL_DATA_DIR};")
set(WCRFT_DATA_DIR ${WCRFT_DATA_DIR} "${CMAKE_INSTALL_PREFIX}/${WCRFT_INSTALL_DATA_DIR}/config;")
set(WCRFT_DATA_DIR ${WCRFT_DATA_DIR} "${CMAKE_INSTALL_PREFIX}/${WCRFT_INSTALL_DATA_DIR}/model")

configure_file(version.in include/libwcrft/version.h)
configure_file(settings.in include/libwcrft/settings.h)

include_directories(${CMAKE_CURRENT_BINARY_DIR}/include)

# source code files
set(libwcrft_STAT_SRC
	pathsearcher.cpp
	config.cpp
	corpusio.cpp
	classify.cpp
	exception.cpp
	layers.cpp
	tagger.cpp
)

file(GLOB_RECURSE INCS "*.h")

add_library(wcrft SHARED ${libwcrft_STAT_SRC} ${INCS})
target_link_libraries(wcrft ${LIBS})
set_target_properties(wcrft PROPERTIES
	VERSION "${libwcrft_VER_MAJOR}.${libwcrft_VER_MINOR}"
	SOVERSION ${libwcrft_VER_MAJOR})

# installing
install(TARGETS wcrft
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
				GROUP_READ GROUP_EXECUTE
				WORLD_READ WORLD_EXECUTE)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	DESTINATION include
	FILES_MATCHING PATTERN "*.h"
	PATTERN "config" EXCLUDE
	PATTERN "model" EXCLUDE
	PATTERN ".svn" EXCLUDE
	PATTERN "bin" EXCLUDE
	PATTERN "build" EXCLUDE
	PATTERN "CMake*" EXCLUDE)

install(
	DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include/
	DESTINATION include
	FILES_MATCHING PATTERN "version.h"
				   PATTERN "settings.h")

# install config and model dirs
set(libwcrft_INSTALL_DATA_DIR share/wcrft)

find_path(libwcrft_SRC_CONFIG_DIR
	config
	${CMAKE_CURRENT_SOURCE_DIR}
)
mark_as_advanced(libwcrft_SRC_CONFIG_DIR)
message(STATUS "Config directory is in ${libwcrft_SRC_CONFIG_DIR}")

install(DIRECTORY ${libwcrft_SRC_CONFIG_DIR}/
		DESTINATION ${libwcrft_INSTALL_DATA_DIR}
		FILES_MATCHING PATTERN config
					   PATTERN config/*)

find_path(libwcrft_SRC_MODEL_DIR
	model
	${CMAKE_CURRENT_SOURCE_DIR}
)
mark_as_advanced(libwcrft_SRC_MODEL_DIR)
message(STATUS "Model directory is in ${libwcrft_SRC_MODEL_DIR}")

install(DIRECTORY ${libwcrft_SRC_MODEL_DIR}/
		DESTINATION ${libwcrft_INSTALL_DATA_DIR}
		FILES_MATCHING PATTERN model
					   PATTERN model/*
					   PATTERN model/*/*)
