project(corpus2mwe_Swig)

set(LIBS cclmwe pwrutils)

find_package(Corpus2 1.0.9 REQUIRED)
include_directories(${Corpus2_INCLUDE_DIR})
set(LIBS ${LIBS} ${Corpus2_LIBRARY})

find_package(Corpus2Whole REQUIRED)
include_directories(${Corpus2Whole_INCLUDE_DIR})
set(LIBS ${LIBS} ${Corpus2Whole_LIBRARY})

find_package(MWEReader 0.1 REQUIRED)
include_directories(${MWEReader_INCLUDE_DIR})
set(LIBS ${LIBS} ${MWEReader_LIBRARY})

find_package(ICU REQUIRED QUIET)
include_directories(${ICU_INCLUDE_DIR})
link_directories(${ICU_LIBRARY_DIRS})
set(LIBS ${LIBS} icuuc icuio)

find_package(LibXML++ REQUIRED QUIET)
include_directories(${LibXML++_INCLUDE_DIRS})

include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

find_package(SWIG REQUIRED)
include(${SWIG_USE_FILE})

find_package(PythonLibs 2.7)
find_package(PythonInterp)

execute_process(
	COMMAND
	${PYTHON_EXECUTABLE} -c "from distutils import sysconfig; print sysconfig.get_python_lib(1,0,prefix='${CMAKE_INSTALL_EXEC_PREFIX}')"
	OUTPUT_VARIABLE PYTHON_INSTDIR
	OUTPUT_STRIP_TRAILING_WHITESPACE
)

include_directories(${PYTHON_INCLUDE_PATH})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB_RECURSE I_FILES "*.i")

set(CMAKE_SWIG_FLAGS "")
set(SWIG_SRC_DIR ${SWIG_DIR}/corpus2mwe)

set_source_files_properties(corpus2mwe.i PROPERTIES CPLUSPLUS ON)
set_source_files_properties(corpus2mwe.i PROPERTIES SWIG_FLAGS "-includeall")

set(SWIG_MODULE_corpus2mwe_EXTRA_DEPS ${I_FILES})

swig_add_module(corpus2mwe python corpus2mwe.i)
swig_link_libraries(corpus2mwe ${PYTHON_LIBRARIES} ${LIBS})

set_target_properties(
	_corpus2mwe
	PROPERTIES
	INSTALL_NAME_DIR "${PYTHON_INSTDIR}"
)

set(SCRIPT_PERMISSIONS
	OWNER_READ
	OWNER_WRITE
	OWNER_EXECUTE
	GROUP_READ
	GROUP_EXECUTE
	WORLD_READ
	WORLD_EXECUTE
)

install(
	TARGETS _corpus2mwe
	LIBRARY
	DESTINATION ${PYTHON_INSTDIR}
	PERMISSIONS ${SCRIPT_PERMISSIONS}
)

install(
	FILES ${CMAKE_CURRENT_BINARY_DIR}/corpus2mwe.py
	DESTINATION ${PYTHON_INSTDIR}
	PERMISSIONS ${SCRIPT_PERMISSIONS}
)

install(
	FILES ${I_FILES}
	DESTINATION ${SWIG_SRC_DIR}
	PERMISSIONS ${SCRIPT_PERMISSIONS}
)
