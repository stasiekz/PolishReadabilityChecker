########## libtokenizer ###############

PROJECT(toki)

set(ver_major "1")
set(ver_minor "1")
set(ver_patch "0")

set(LIBS "")

include_directories( ${CMAKE_SOURCE_DIR} )

find_package(ICU REQUIRED)
include_directories(${ICU_INCLUDE_DIR})
link_directories(${ICU_LIBRARY_DIRS})
if(UNIX)
	set(LIBS ${LIBS} icuuc icuio icui18n)
else(UNIX)
	set(LIBS ${LIBS} icuuc icuio icutu icuin)
endif(UNIX)

find_package(PwrUtils REQUIRED)
set(LIBS ${LIBS} ${PwrUtils_LIBRARY})
include_directories(${PwrUtils_INCLUDE_DIR})

find_package(Loki REQUIRED)
set(LIBS ${LIBS} loki)

find_package(LibXML++ REQUIRED)
include_directories(${LibXML++_INCLUDE_DIRS})
link_directories(${LibXML++_LIBRARY_DIRS})
set(LIBS ${LIBS} ${LibXML++_LIBRARIES})

include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})
set(LIBS ${LIBS} ${Boost_LIBRARIES})

set(LIBTOKI_INSTALL_DATA_DIR share/toki)
set(LIBTOKI_PATH_SEPARATOR ";")
set(LIBTOKI_DATA_DIR ".;${CMAKE_INSTALL_PREFIX}/${LIBTOKI_INSTALL_DATA_DIR}")
set(LIBTOKI_VERSION "${ver_major}.${ver_minor}.${ver_patch}")

FIND_PATH(LIBTOKI_SRC_DATA_DIR
	config.ini
	${CMAKE_SOURCE_DIR}/config
)
MARK_AS_ADVANCED(LIBTOKI_SRC_DATA_DIR)

if(NOT LIBTOKI_SRC_DATA_DIR)
	message(FATAL_ERROR "There is no data for libtoki")
endif(NOT LIBTOKI_SRC_DATA_DIR)

configure_file(config_d.in include/libtoki/config_d.h)
configure_file(version.in include/libtoki/version.h)
include_directories( ${CMAKE_CURRENT_BINARY_DIR}/include/ )
add_definitions(-DHAVE_CONFIG_D_H)
add_definitions(-DHAVE_VERSION_H)

SET(libtoki_STAT_SRC
	exception.cpp
	main.cpp
	sentencesplitter.cpp
	token.cpp
	srx/document.cpp
	srx/rule.cpp
	srx/segmenter.cpp
	srx/srx.cpp
	layers/affixsplit.cpp
	layers/append.cpp
	layers/basicsplit.cpp
	layers/check.cpp
	layers/combine.cpp
	layers/groupaffixsplit.cpp
	layers/groupsplit.cpp
	layers/init.cpp
	layers/inputbuffer.cpp
	layers/lexiconclassify.cpp
	layers/outputqueue.cpp
	layers/regexpclassify.cpp
	layers/regexmatchsplit.cpp
	layers/tokenlayer.cpp
	tokenizer/layertokenizer.cpp
	tokenizer/tokenizer.cpp
	tokenizer/whitespacetokenizer.cpp
	unicode/icustringwrapper.cpp
	unicode/istreamwrapper.cpp
	unicode/nullsource.cpp
	unicode/sink.cpp
	unicode/source.cpp
	util/confignode.cpp
	util/debug.cpp
	util/settings.cpp
	util/xmlutil.cpp
)

file(GLOB_RECURSE INCS "*.h")

add_library(toki SHARED ${libtoki_STAT_SRC} ${INCS})
target_link_libraries ( toki ${LIBS} )
set_target_properties(toki PROPERTIES
	VERSION "${ver_major}.${ver_minor}"
	SOVERSION ${ver_major})

if(UNIX)
	set_target_properties(toki PROPERTIES
		LINK_FLAGS -Wl,-e,LIBTOKI_entry_)
endif(UNIX)
# Newer LibXML requires C++11
if(NOT ${LibXML++_PKGCONF_VERSION} VERSION_LESS 2.40)
	set_target_properties(toki PROPERTIES
		CXX_STANDARD 11)
endif(NOT ${LibXML++_PKGCONF_VERSION} VERSION_LESS 2.40)


	install(TARGETS toki
		RUNTIME DESTINATION bin
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib
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
		DIRECTORY ${LIBTOKI_SRC_DATA_DIR}/
		DESTINATION ${LIBTOKI_INSTALL_DATA_DIR}
		FILES_MATCHING PATTERN "*.ini"
		PATTERN "*.srx"
		PATTERN "*.lex"
		PATTERN "*.xml"
		PATTERN "*.txt"
		PATTERN ".svn" EXCLUDE
	)

