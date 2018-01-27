########## libmaca ###############

PROJECT(maca)

if(NOT LIBMACA_SRC_DATA_DIR)
	message(FATAL_ERROR "There is no data for libmaca")
endif(NOT LIBMACA_SRC_DATA_DIR)

set(LIBMACA_PATH_SEPARATOR ";")
set(LIBMACA_INSTALL_DATA_DIR share/maca)
set(LIBMACA_DATA_DIR ".;${CMAKE_INSTALL_PREFIX}/${LIBMACA_INSTALL_DATA_DIR}")
set(LIBMACA_VERSION "${maca_ver_major}.${maca_ver_minor}.${maca_ver_patch}")

configure_file(config_d.in include/libmaca/config_d.h)
configure_file(version.in include/libmaca/version.h)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/include/)
add_definitions(-DHAVE_CONFIG_D_H)
add_definitions(-DHAVE_VERSION_H)

include_directories( ${CMAKE_CURRENT_BINARY_DIR}/include/ )


#include_directories("/usr/include/sfst-1.0")
#set(LIBS ${LIBS} "sfst1")

find_package(Toki 1.0.0 REQUIRED)
set(LIBS ${LIBS} ${Toki_LIBRARY})

find_package(Corpus2 1.0.9 REQUIRED)
set(LIBS ${LIBS} ${Corpus2_LIBRARY})
include_directories(${Corpus2_INCLUDE_DIR})

find_package(PwrUtils REQUIRED)
set(LIBS ${LIBS} ${PwrUtils_LIBRARY})


find_package(ICU REQUIRED QUIET)
include_directories(${ICU_INCLUDE_DIR})
link_directories(${ICU_LIBRARY_DIRS})
if(UNIX)
	set(LIBS ${LIBS} icuuc icuio)
else(UNIX)
	set(LIBS ${LIBS} icuuc icuio icutu icuin)
endif(UNIX)

find_package(Loki REQUIRED QUIET)

find_package(LibXML++ REQUIRED QUIET)
include_directories(${LibXML++_INCLUDE_DIRS})
link_directories(${LibXML++_LIBRARY_DIRS})
set(LIBS ${LIBS} ${LibXML++_LIBRARIES})

include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})
set(LIBS ${LIBS} ${Boost_LIBRARIES})

SET(libmaca_STAT_SRC
	exception.cpp
	main.cpp
	conv/attributecopier.cpp
	conv/fold.cpp
	conv/joinlayer.cpp
	conv/joinrule.cpp
	conv/layer.cpp
	conv/predicate.cpp
	conv/removedupes.cpp
	conv/splitlayer.cpp
	conv/tagconvert.cpp
	conv/tagrule.cpp
	conv/tagrulelayer.cpp
	conv/tagsetconverter.cpp
	io/text.cpp
	io/premorph.cpp
	morph/constanalyser.cpp
	morph/convertinganalyser.cpp
	morph/dispatchanalyser.cpp
	morph/mapanalyser.cpp
	morph/morphanalyser.cpp
	morph/guesser2.cpp
	util/confignode.cpp
	util/debug.cpp
	util/sentenceanalyser.cpp
	util/settings.cpp
	util/symboldictionary.cpp
	util/tokentimer.cpp
)

if (UNIX)
	set(SFST_PLUGIN ON CACHE BOOL "Build SFST analyser as a plugin")
	set(MORFEUSZ_PLUGIN OFF CACHE BOOL "Build Morfeusz analyser as a plugin")
	set(GUESSER_PLUGIN ON CACHE BOOL "Build Guesser analyser as a plugin")
endif(UNIX)

find_package(SFST)
if(BUILD_GPL_PLUGINS)
	if (SFST_FOUND)
		set(SFST_COMPONENT_BUILD True CACHE INTERNAL "SFST MARKER" PARENT_SCOPE)
		if(SFST_PLUGIN)
			message(STATUS "SFST support will be built as a plugin")
			add_library(maca_sfst MODULE morph/sfstanalyser.cpp)
			target_link_libraries(maca_sfst maca ${SFST_LIBRARY} )
			set_target_properties(maca_sfst PROPERTIES
				VERSION "${maca_ver_major}.${maca_ver_minor}"
				SOVERSION ${maca_ver_major})
			set_target_properties(maca_sfst PROPERTIES
				COMPILE_FLAGS "-DSFST_AS_PLUGIN -I${SFST_INCLUDE_DIR} -Wno-deprecated"
			)
			install(TARGETS maca_sfst
				LIBRARY DESTINATION lib)
		else(SFST_PLUGIN)
			message(STATUS "SFST support will be built-in")
			set(LIBS ${LIBS} ${SFST_LIBRARY})
			include_directories(${SFST_INCLUDE_DIR})
			link_directories(${SFST_LIBRARY_DIRS})
			set(HAVE_SFST 1)
			SET(libmaca_STAT_SRC ${libmaca_STAT_SRC} morph/sfstanalyser.cpp)
		endif(SFST_PLUGIN)
	endif (SFST_FOUND)
endif(BUILD_GPL_PLUGINS)

find_package(Morfeusz)
if (MORFEUSZ_FOUND)
	if(MORFEUSZ_PLUGIN)
		message(STATUS "Morfeusz support will be built as a plugin")
		add_library(maca_morfeusz MODULE morph/morfeuszanalyser.cpp)
		#target_link_libraries ( maca_morfeusz maca ${MORFEUSZ_LIBRARY} )
		set_target_properties(maca_morfeusz PROPERTIES
			VERSION "${maca_ver_major}.${maca_ver_minor}"
			SOVERSION ${maca_ver_major})
		set_target_properties(maca_morfeusz PROPERTIES
			COMPILE_FLAGS "-DMORFEUSZ_AS_PLUGIN -I${MORFEUSZ_INCLUDE_DIR}"
		)
		install(TARGETS maca_morfeusz
			LIBRARY DESTINATION lib)
	else(MORFEUSZ_PLUGIN)
		message(STATUS "Morfeusz support will be built-in")
		set(LIBS ${LIBS} ${MORFEUSZ_LIBRARY})
		include_directories(${MORFEUSZ_INCLUDE_DIR})
		link_directories(${MORFEUSZ_LIBRARY_DIRS})
		set(HAVE_MORFEUSZ 1)
		SET(libmaca_STAT_SRC ${libmaca_STAT_SRC} morph/morfeuszanalyser.cpp)
	endif(MORFEUSZ_PLUGIN)
endif (MORFEUSZ_FOUND)

find_package(Morfeusz2)
if (MORFEUSZ2_FOUND)
	set(LIBS ${LIBS} ${MORFEUSZ2_LIBRARY})
	include_directories(${MORFEUSZ2_INCLUDE_DIR})
	link_directories(${MORFEUSZ2_LIBRARY_DIRS})

	set(libmaca_STAT_SRC ${libmaca_STAT_SRC} morph/morfeusz2analyser.cpp)
endif (MORFEUSZ2_FOUND)

find_package(Corpus)
if(BUILD_GPL_PLUGINS)
	if (CORPUS_FOUND)
		set(GUESSER_COMPONENT_BUILD True CACHE INTERNAL "GUESSER MARKER" PARENT_SCOPE)
		if(GUESSER_PLUGIN)
			message(STATUS "Guesser support will be built as a plugin")
			add_library(maca_guesser MODULE morph/guesser.cpp)
			target_link_libraries ( maca_guesser maca ${CORPUS_LIBRARY} )
			set_target_properties(maca_guesser PROPERTIES
				VERSION "${maca_ver_major}.${maca_ver_minor}"
				SOVERSION ${maca_ver_major})
			set_target_properties(maca_guesser PROPERTIES
				COMPILE_FLAGS "-DGUESSER_AS_PLUGIN"
			)
			install(TARGETS maca_guesser
				LIBRARY DESTINATION lib)
		else(GUESSER_PLUGIN)
			set(LIBS ${LIBS} ${CORPUS_LIBRARY})
			set(HAVE_CORPUS 1)
			SET(libmaca_STAT_SRC ${libmaca_STAT_SRC} morph/guesser.cpp)
		endif(GUESSER_PLUGIN)
	endif (CORPUS_FOUND)
endif(BUILD_GPL_PLUGINS)

file(GLOB_RECURSE INCS "*.h")

add_library(maca SHARED ${libmaca_STAT_SRC} ${INCS})
target_link_libraries ( maca ${LIBS} )
set_target_properties(maca PROPERTIES
	VERSION "${maca_ver_major}.${maca_ver_minor}"
	SOVERSION ${maca_ver_major})

if(UNIX)
	set_target_properties(maca PROPERTIES
		LINK_FLAGS -Wl,-e,LIBMACA_entry_)
endif(UNIX)
# Newer LibXML requires C++11
if(NOT ${LibXML++_PKGCONF_VERSION} VERSION_LESS 2.40)
	set_target_properties(maca PROPERTIES
		CXX_STANDARD 11)
endif(NOT ${LibXML++_PKGCONF_VERSION} VERSION_LESS 2.40)


	install(TARGETS maca
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib
		RUNTIME DESTINATION bin
		PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
		GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

	install(
		DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		DESTINATION include
		FILES_MATCHING PATTERN "*.h"
		PATTERN ".svn" EXCLUDE
		PATTERN "bin" EXCLUDE
		PATTERN "build" EXCLUDE
		PATTERN "CMake*" EXCLUDE
	)
	install(
		DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include/
		DESTINATION include
		FILES_MATCHING PATTERN "version.h"
	)
	install(
		DIRECTORY ${LIBMACA_SRC_DATA_DIR}/
		DESTINATION ${LIBMACA_INSTALL_DATA_DIR}
		FILES_MATCHING PATTERN "*.ini"
		PATTERN "*.tagset"
		PATTERN "*.conv"
		PATTERN "*.xml"
		PATTERN "*.txt"
		PATTERN "*.fst"
		PATTERN ".svn" EXCLUDE
		PATTERN "local/*" EXCLUDE
	)
