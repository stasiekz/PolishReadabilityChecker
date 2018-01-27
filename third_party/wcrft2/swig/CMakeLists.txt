project(WcrftSwigWrap)

set(WCRFT_LIBS wcrft)

set(CORPUS2_LIBS ${Corpus2_LIBRARY})
set(PWRUTILS_LIBS ${PwrUtils_LIBRARY})
set(MACA_LIBS ${Maca_LIBRARY})
set(WCCL_LIBS ${WCCL_LIBRARY})

set(OUR_LIBS ${CORPUS2_LIBS} ${PWRUTILS_LIBS} ${MACA_LIBS} ${WCCL_LIBS} ${WCRFT_LIBS})

find_package(SWIG REQUIRED)
include(${SWIG_USE_FILE})

include_directories(${LIBWCRFT_SOURCE_DIR} "../libwcrft")
link_directories(${LIBWCRFT_BINARY_DIR})

find_package(PythonLibs)
find_package(PythonInterp)

# idea taken from pyplot build system 
execute_process(
	COMMAND
	 python -c "from distutils import sysconfig; print sysconfig.get_python_lib(1,0,prefix='${CMAKE_INSTALL_EXEC_PREFIX}')"
	OUTPUT_VARIABLE PYTHON_INSTDIR
	OUTPUT_STRIP_TRAILING_WHITESPACE
)
message(STATUS "INFO: " "python lib: ${PYTHON_INSTDIR}" )

include_directories(${PYTHON_INCLUDE_PATH})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

link_directories(${Boost_LIBRARY_DIRS})
include_directories(${Boost_INCLUDE_DIR})

if(CMAKE_COMPILER_IS_GNUCXX)
        set(CMAKE_CXX_FLAGS "-ansi $ENV{CXXFLAGS}")
        set(CMAKE_CXX_FLAGS_DEBUG "-O0 -DDEBUG -ggdb3 -ansi $ENV{CXXFLAGS}")
        set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g -fno-omit-frame-pointer")
endif(CMAKE_COMPILER_IS_GNUCXX)

file(GLOB_RECURSE I_FILES "*.i")

# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------

set(CMAKE_SWIG_FLAGS "")
set(SWIG_SRC_DIR ${SWIG_DIR}/wcrft2)

# -----------------------------------------------------------------------------

# -----------------------------------------------------------------------------
# maca
set_source_files_properties(wcrft2.i PROPERTIES CPLUSPLUS ON)
set_source_files_properties(wcrft2.i PROPERTIES SWIG_FLAGS "-includeall" )
set(SWIG_MODULE_WCRFT_EXTRA_DEPS ${I_FILES})
SWIG_ADD_MODULE(wcrft2 python wcrft2.i)
SWIG_LINK_LIBRARIES(wcrft2 ${PYTHON_LIBRARIES} ${OUR_LIBS})

# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------

set_target_properties(
	_wcrft2
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
	TARGETS _wcrft2
	LIBRARY
	DESTINATION ${PYTHON_INSTDIR}
	PERMISSIONS ${PERM_SCRIPTS}
)

install(
	FILES ${CMAKE_CURRENT_BINARY_DIR}/wcrft2.py
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
