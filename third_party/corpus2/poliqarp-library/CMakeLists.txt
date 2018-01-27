project (PoliqarpLibrary)
cmake_minimum_required(VERSION 2.8.0)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMakeScripts)
set(pqlib_ver_major 1)
set(pqlib_ver_minor 0)

FIND_PACKAGE(BISON REQUIRED)
FIND_PACKAGE(FLEX REQUIRED)

INCLUDE (CheckIncludeFiles)
INCLUDE (CheckLibraryExists)
INCLUDE (CheckFunctionExists)
INCLUDE (CheckTypeSize)
INCLUDE (AC_HEADER_STDC)

CHECK_FUNCTION_EXISTS(ngettext HAVE_GETTEXT)
CHECK_FUNCTION_EXISTS(nftw HAVE_NFTW)
CHECK_FUNCTION_EXISTS(snprintf HAVE_SNPRINTF)
CHECK_FUNCTION_EXISTS(_snprintf HAVE__SNPRINTF)

CHECK_INCLUDE_FILES("inttypes.h" HAVE_INTTYPES_H)
CHECK_INCLUDE_FILES("locale.h" HAVE_LOCALE_H)
CHECK_INCLUDE_FILES("memory.h" HAVE_MEMORY_H)
CHECK_INCLUDE_FILES("malloc.h" HAVE_MALLOC_H)
CHECK_INCLUDE_FILES("netinet/in.h" HAVE_NETINET_IN_H)
CHECK_INCLUDE_FILES("pthread.h" HAVE_PTHREAD)
CHECK_INCLUDE_FILES("stdbool.h" HAVE_STDBOOL_H)
CHECK_INCLUDE_FILES("stdint.h" HAVE_STDINT_H)
CHECK_INCLUDE_FILES("stdlib.h" HAVE_STDLIB_H)
CHECK_INCLUDE_FILES("strings.h" HAVE_STRINGS_H)
CHECK_INCLUDE_FILES("string.h" HAVE_STRING_H)
CHECK_INCLUDE_FILES("sys/socket.h" HAVE_SYS_SOCKET_H)
CHECK_INCLUDE_FILES("sys/stat.h" HAVE_SYS_STAT_H)
CHECK_INCLUDE_FILES("sys/types.h" HAVE_SYS_TYPES_H)
CHECK_INCLUDE_FILES("unistd.h" HAVE_UNISTD_H)

CHECK_LIBRARY_EXISTS(expat XML_ParserCreate "" HAVE_LIBEXPAT)

SET(CMAKE_EXTRA_INCLUDE_FILES unistd.h sys/socket.h netinet/in.h)
CHECK_TYPE_SIZE("socklen_t" SOCKLEN_T)
SET(CMAKE_EXTRA_INCLUDE_FILES unistd.h)
CHECK_TYPE_SIZE("ssize_t" SSIZE_T)
SET(CMAKE_EXTRA_INCLUDE_FILES stdbool.h)
CHECK_TYPE_SIZE("_Bool" _BOOL)
SET(CMAKE_EXTRA_INCLUDE_FILES)

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/cmake-config.h.in ${CMAKE_CURRENT_BINARY_DIR}/poliqarp-config.h)
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/commondef.h ${CMAKE_CURRENT_BINARY_DIR}/sakura/commondef.h)

include_directories(${PoliqarpLibrary_SOURCE_DIR} ${PoliqarpLibrary_BINARY_DIR})
# ${PoliqarpLibrary_SOURCE_DIR}/sakura/commons ${PoliqarpLibrary_SOURCE_DIR}/foostring ${PoliqarpLibrary_SOURCE_DIR}/progress ${PoliqarpLibrary_SOURCE_DIR}/sakura ${PoliqarpLibrary_SOURCE_DIR}/unibits)
find_package(Parsers REQUIRED)
find_package(Threads)
set(CFLAGS "$ENV{CFLAGS} -Wno-unused-parameter")
set(CMAKE_C_FLAGS "${CFLAGS}")
set(CMAKE_C_FLAGS_DEBUG "-O0 -DDEBUG -ggdb3 ${CFLAGS}")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "-O2 -g -fno-omit-frame-pointer")

GENERATE_BISON_FLEX_SOURCES(
	"${CMAKE_CURRENT_SOURCE_DIR}/sakura/parser.y" ""
	"${CMAKE_CURRENT_SOURCE_DIR}/sakura/lexer.y" ""
)
add_custom_command(
	OUTPUT ${PoliqarpLibrary_BINARY_DIR}/sakura/parser.h
	COMMAND ${CMAKE_COMMAND}
	ARGS -E copy ${PoliqarpLibrary_BINARY_DIR}/parser.h ${PoliqarpLibrary_BINARY_DIR}/sakura/parser.h
	COMMENT "copy parser.h"
	DEPENDS ${PoliqarpLibrary_BINARY_DIR}/parser.h
)

set(foostring_SRC foostring/foostring.c foostring/strnlen.c)
set(progress_SRC progress/progress.c)
set(unibits_SRC unibits/tclUtils.c unibits/tclUtf.c unibits/strcoll.c)

set(common_SRC 
	sakura/common/args.c
	sakura/common/bitstream.c
	sakura/common/bs.c
	sakura/common/bs-file-backend.c
	sakura/common/entity.c
	sakura/common/file-map.c
	sakura/common/file-reader.c
	sakura/common/getline.c
	sakura/common/graph.c
	sakura/common/hash-table.c
	sakura/common/memory-arena.c
	sakura/common/newdict.c
	sakura/common/set.c
	sakura/common/system-error.c
	sakura/common/tinydb.c
)
set(sakura_SRC
	sakura/backend-base.c
	sakura/backend-config.c
	sakura/backend-corpus.c
	sakura/backend-document.c
	sakura/backend-index.c
	sakura/backend-interp.c
	sakura/backend-meta.c
	sakura/backend-orth.c
	sakura/backend-subdocument.c
	sakura/backend-syntax.c
	sakura/backend-tag.c
	sakura/cdf.c
	sakura/config.c
	sakura/corpus.c
	sakura/dict.c
	sakura/exception.c
	sakura/expression.c
	sakura/meta-value.c
	sakura/poliqarp.c
	sakura/query.c
	sakura/query-rewrite.c
	sakura/random.c
	sakura/regexp.c
	sakura/value-attr.c
	sakura/value-base.c
	sakura/value.c
	sakura/value-interp.c
	sakura/value-orth.c
	sakura/value-pattern.c
	sakura/value-pos.c
	sakura/value-space.c
	sakura/value-tag.c
	sakura/value-type.c
)
set(poliqarpd_SRC
	poliqarpd/async.c
	poliqarpd/configuration.c
	poliqarpd/log.c
	poliqarpd/msgqueue.c
	poliqarpd/od_unix.c
#	poliqarpd/od_win32.c
	poliqarpd/poliqarpd.c
	poliqarpd/protocol.c
	poliqarpd/server.c
	poliqarpd/session.c
	poliqarpd/sessopt.c
	poliqarpd/sockets.c
	poliqarpd/sockstream.c
	poliqarpd/utils.c
)

add_library(poliqarpc2 SHARED ${foostring_SRC} ${progress_SRC} ${unibits_SRC} ${common_SRC} ${sakura_SRC} ${BF_SOURCES} ${PoliqarpLibrary_BINARY_DIR}/sakura/parser.h)
#add_dependencies(poliqarpc2 ${PoliqarpLibrary_BINARY_DIR}/sakura/parser.h)
set_target_properties(poliqarpc2 PROPERTIES
	VERSION "${pqlib_ver_major}.${pqlib_ver_minor}"
	SOVERSION ${pqlib_ver_major})

add_executable(poliqarpc-shared utils/poliqarpc.c)
target_link_libraries(poliqarpc-shared poliqarpc2 pthread)
add_executable(poliqarpd-shared ${poliqarpd_SRC})
target_link_libraries(poliqarpd-shared poliqarpc2 pthread)
if(UNIX)
	install(TARGETS poliqarpc2 LIBRARY DESTINATION lib)
	install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/sakura DESTINATION include
		FILES_MATCHING PATTERN "*.h")
	install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/progress DESTINATION include/sakura
		FILES_MATCHING PATTERN "*.h")
	install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/ DESTINATION include/sakura
		FILES_MATCHING PATTERN "poliqarp-config.h")
	install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/sakura DESTINATION include
		FILES_MATCHING PATTERN "commondef.h")
	install(TARGETS poliqarpc-shared poliqarpd-shared RUNTIME DESTINATION bin)
endif(UNIX)

