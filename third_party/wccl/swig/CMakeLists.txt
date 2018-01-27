########## wccl wrappers -- SWIG ###############

PROJECT(WcclSwigWrap)

find_package(Corpus2 1.0.8 REQUIRED)
set(CORPUS2_LIBS ${Corpus2_LIBRARY})
include_directories(${Corpus2_INCLUDE_DIR})

find_package(PwrUtils 1.0.1 REQUIRED)
set(PWRUTILS_LIBS ${PwrUtils_LIBRARY})

# find_package(Wccl 1.0.1 REQUIRED)
set(WCCL_LIBS wccl)
set(CORPUS2_PWR_WCCL_LIBS ${CORPUS2_LIBS} ${PWRUTILS_LIBS} ${WCCL_LIBS})

find_package(ICU REQUIRED)
include_directories(${ICU_INCLUDE_DIR})
link_directories(${ICU_LIBRARY_DIRS})

link_directories(${Boost_LIBRARY_DIRS})

include_directories(${Boost_INCLUDE_DIR})

include_directories (${Libwccl_SOURCE_DIR} "../libwccl")
link_directories(${Libwccl_BINARY_DIR})

FIND_PACKAGE(SWIG REQUIRED)
INCLUDE(${SWIG_USE_FILE})

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
SET(SWIG_SRC_DIR ${SWIG_DIR}/wccl)

# -----------------------------------------------------------------------------
# wccl
SET_SOURCE_FILES_PROPERTIES(wccl.i PROPERTIES CPLUSPLUS ON)
SET_SOURCE_FILES_PROPERTIES(wccl.i PROPERTIES SWIG_FLAGS "-includeall" )
set(SWIG_MODULE_wccl_EXTRA_DEPS ${I_FILES})
SWIG_ADD_MODULE(wccl python wccl.i )
if(UNIX)
SWIG_LINK_LIBRARIES(wccl ${PYTHON_LIBRARIES} ${CORPUS2_PWR_WCCL_LIBS} antlr-pic)
else(UNIX)
SWIG_LINK_LIBRARIES(wccl ${PYTHON_LIBRARIES} ${CORPUS2_PWR_WCCL_LIBS} antlr)
endif(UNIX)
# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------

set_target_properties(
	_wccl
	PROPERTIES
	INSTALL_NAME_DIR "${PYTHON_INSTDIR}"
)

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
	TARGETS _wccl 
	LIBRARY
	DESTINATION ${PYTHON_INSTDIR}
	PERMISSIONS ${PERM_SCRIPTS}
)

install(
	FILES ${CMAKE_CURRENT_BINARY_DIR}/wccl.py
	DESTINATION ${PYTHON_INSTDIR}
	PERMISSIONS ${PERM_SCRIPTS}
)

# -----------------------------------------------------------------------------
# Install SWIG files
# -----------------------------------------------------------------------------

install(
	FILES ${I_FILES}
	DESTINATION ${SWIG_SRC_DIR}
	PERMISSIONS ${PERM_SCRIPTS}
)
