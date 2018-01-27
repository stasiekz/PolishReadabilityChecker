########## corpus2 wrappers -- SWIG ###############

PROJECT(corpus2SwigWrap)

set(LIBS "corpus2" "corpus2_whole" "pwrutils")

if(CORPUS2_BUILD_POLIQARP)
	set(LIBS ${LIBS} "corpus2_poliqarpreader" )
endif(CORPUS2_BUILD_POLIQARP)

find_package(ICU REQUIRED QUIET)
include_directories(${ICU_INCLUDE_DIR})
link_directories(${ICU_LIBRARY_DIRS})
set(LIBS ${LIBS} icuuc icuio)

find_package(Loki REQUIRED QUIET)


include_directories (${corpus2_SOURCE_DIR})
include_directories (${pwrutils_SOURCE_DIR})

link_directories(${Libcorpus2_BINARY_DIR})

FIND_PACKAGE(SWIG REQUIRED)
INCLUDE(${SWIG_USE_FILE})

FIND_PACKAGE(LibXML++ REQUIRED QUIET)
include_directories(${LibXML++_INCLUDE_DIRS})
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})


find_package(PythonLibs 2)
find_package(PythonInterp)
# idea taken from pyplot build system 
execute_process(
	COMMAND
	${PYTHON_EXECUTABLE} -c "from distutils import sysconfig; print sysconfig.get_python_lib(1,0,prefix='${CMAKE_INSTALL_EXEC_PREFIX}')"
	OUTPUT_VARIABLE PYTHON_INSTDIR
	OUTPUT_STRIP_TRAILING_WHITESPACE
)
message(STATUS "INFO: " "python lib: ${PYTHON_INSTDIR}" )

INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_PATH})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

if(CMAKE_COMPILER_IS_GNUCXX)
	set(CMAKE_CXX_FLAGS "-ansi $ENV{CXXFLAGS}")
	set(CMAKE_CXX_FLAGS_DEBUG "-O0 -DDEBUG -ggdb3 -ansi $ENV{CXXFLAGS}")
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g -fno-omit-frame-pointer")
endif(CMAKE_COMPILER_IS_GNUCXX)

file(GLOB_RECURSE I_FILES "*.i")

# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------

SET(CMAKE_SWIG_FLAGS "")
SET(SWIG_SRC_DIR ${SWIG_DIR}/libcorpus2)

# -----------------------------------------------------------------------------
# corous2
SET_SOURCE_FILES_PROPERTIES(corpus2.i PROPERTIES CPLUSPLUS ON)
SET_SOURCE_FILES_PROPERTIES(corpus2.i PROPERTIES SWIG_FLAGS "-includeall" )
set(SWIG_MODULE_corpus2_EXTRA_DEPS ${I_FILES})
SWIG_ADD_MODULE(corpus2 python corpus2.i )
SWIG_LINK_LIBRARIES(corpus2 ${PYTHON_LIBRARIES} ${LIBS})
# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------

set_target_properties(
	_corpus2
	PROPERTIES
	INSTALL_NAME_DIR "${PYTHON_INSTDIR}"
)
# Newer libxml++ requires compilation with c++11
if(NOT ${LibXML++_PKGCONF_VERSION} VERSION_LESS 2.40)
	set_target_properties(_corpus2 PROPERTIES CXX_STANDARD 11)
endif(NOT ${LibXML++_PKGCONF_VERSION} VERSION_LESS 2.40)

set(PERM_SCRIPTS
	OWNER_READ
	OWNER_WRITE
	OWNER_EXECUTE
	GROUP_READ
	GROUP_EXECUTE
	WORLD_READ
	WORLD_EXECUTE
)

# -----------------------------------------------------------------------------
# Install python modules
# -----------------------------------------------------------------------------

install(
	TARGETS _corpus2 
	LIBRARY
	DESTINATION ${PYTHON_INSTDIR}
	PERMISSIONS ${PERM_SCRIPTS}
)

install(
	FILES ${CMAKE_CURRENT_BINARY_DIR}/corpus2.py
	DESTINATION ${PYTHON_INSTDIR}
	PERMISSIONS ${PERM_SCRIPTS}
)

# -----------------------------------------------------------------------------
# Install SWIG files
# -----------------------------------------------------------------------------

install(
	FILES ${I_FILES}
        DESTINATION include/libcorpus2
	PERMISSIONS ${PERM_SCRIPTS}
)
