PROJECT( test )

include_directories( ${CMAKE_SOURCE_DIR} )

add_definitions(-DLIBMACA_TEST_DATA_DIR="${PROJECT_SOURCE_DIR}/")

add_executable(tests
	main.cpp
	compareconv.cpp
	conv.cpp
	morph_basic.cpp
)

find_package(Toki 1.0.0 REQUIRED)
set(LIBS ${LIBS} ${Toki_LIBRARY})

find_package(Corpus2 1.0.9 REQUIRED)
set(LIBS ${LIBS} ${Corpus2_LIBRARIES})

target_link_libraries ( tests maca ${Boost_LIBRARIES} ${LIBS})

include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})

#configure_file(test-sanity.sh ${CMAKE_CURRENT_BINARY_DIR})
#add_custom_target(test tests COMMAND ./test-sanity.sh small)
add_custom_target(test tests)
add_custom_target(test-verbose ./tests --log_level=message)
#add_custom_target(test-large ./test-sanity.sh large)
