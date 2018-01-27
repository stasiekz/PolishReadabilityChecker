project(cclmwe)

set(cclmwe_SOURCES
        cclmwereader.cpp
	mwemanager.cpp
)

find_package(Corpus2 1.0.9 REQUIRED)
include_directories(${Corpus2_INCLUDE_DIR})
link_directories(${Corpus2_LIBRARY_DIR})
set(LIBS ${LIBS} ${Corpus2_LIBRARY})

find_package(Corpus2Whole REQUIRED)
include_directories(${Corpus2Whole_INCLUDE_DIR})
link_directories(${Corpus2Whole_LIBRARY_DIR})
set(LIBS ${LIBS} ${Corpus2Whole_LIBRARY})

find_package(MWEReader 0.1 REQUIRED)
include_directories(${MWEReader_INCLUDE_DIR})
link_directories(${MWEReader_LIBRARY_DIR})
set(LIBS ${LIBS} ${MWEReader_LIBRARY})

find_package(Glibmm)
find_package(LibXML++)
find_package(Glib)
find_package(ICU REQUIRED)

find_path(corpus2mwe_src_data_dir mwe.xml ${CMAKE_SOURCE_DIR}/data)
mark_as_advanced(corpus2mwe_src_data_dir)
set(corpus2mwe_install_data_dir share/corpus2mwe)
set(CORPUS2MWE_DATA_DIR 
	"${CMAKE_INSTALL_PREFIX}/${corpus2mwe_install_data_dir}"
)

if(NOT corpus2mwe_src_data_dir)
	message(fatal_error " No data for cclmwe")
endif(NOT corpus2mwe_src_data_dir)

add_definitions(-DCORPUS2MWE_DATA_DIR="${CORPUS2MWE_DATA_DIR}")

include_directories(${ICU_INCLUDE_DIR})
link_directories(${ICU_LIBRARY_DIRS})
set(LIBS ${LIBS} icuuc icuio)

set(LIBS ${LIBS} ${LibXML++_LIBRARIES})

include_directories(${LibXML++_INCLUDE_DIR})
link_directories(${LibXML++_LIBRARY_DIR})

include_directories(${LibXML++Config_INCLUDE_DIR})

include_directories(${Glibmm_INCLUDE_DIR})
link_directories(${Glibmm_LIBRARY})

include_directories(${GlibmmConfig_INCLUDE_DIR})

include_directories(${Glib_INCLUDE_DIR})
link_directories(${Glib_LIBRARY})

include_directories(${GlibConfig_INCLUDE_DIR})

add_library(cclmwe SHARED ${cclmwe_SOURCES})
target_link_libraries(cclmwe ${LIBS})

add_subdirectory(tests)

install(TARGETS cclmwe
	RUNTIME DESTINATION bin
	ARCHIVE DESTINATION lib
	LIBRARY DESTINATION lib)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	DESTINATION include
	FILES_MATCHING PATTERN "*.h"
	PATTERN "bin" EXCLUDE
	PATTERN "build" EXCLUDE
	PATTERN "tests" EXCLUDE
	PATTERN "unittests" EXCLUDE
	PATTERN "CMake*" EXCLUDE)

install(DIRECTORY ${corpus2mwe_src_data_dir}/
	DESTINATION ${corpus2mwe_install_data_dir}
	FILES_MATCHING PATTERN "*.xml"
	PATTERN "local/*" EXCLUDE)
