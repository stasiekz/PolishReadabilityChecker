project(cclmwe_test)

ADD_DEFINITIONS(-DBOOST_TEST_DYN_LINK)

include(FindPythonInterp)

find_package(Boost COMPONENTS unit_test_framework)
set(LIBS ${LIBS} ${Boost_LIBRARIES})

include_directories(${CMAKE_SOURCE_DIR})

include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})

include_directories(${LibXML++_INCLUDE_DIR})
link_directories(${LibXML++_LIBRARY_DIR})

include_directories(${LibXML++Config_INCLUDE_DIR})

include_directories(${Glibmm_INCLUDE_DIR})
link_directories(${Glibmm_LIBRARY})

include_directories(${GlibmmConfig_INCLUDE_DIR})

include_directories(${Glib_INCLUDE_DIR})
link_directories(${Glib_LIBRARY})

include_directories(${GlibConfig_INCLUDE_DIR})

add_executable(cclmwe_tests
	cclmwe_test.cpp
)
target_link_libraries(cclmwe_tests cclmwe ${LIBS} pwrutils)

add_custom_command(
        TARGET cclmwe_tests
        COMMENT "C++ tests"
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/cclmwe/tests/testdata
        $<TARGET_FILE_DIR:cclmwe_tests>/testdata
)

# add_custom_command(
#         TARGET cclmwe_tests
#         COMMENT "Run Python tests"
#         POST_BUILD 
#         WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/cclmwe/tests/ccl_gz_tests
#         COMMAND ${PYTHON_EXECUTABLE} testMWE.py ${CMAKE_BINARY_DIR}/swig/corpus2mwe
# )
