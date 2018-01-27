PROJECT(maca-convert)

include_directories( ${CMAKE_SOURCE_DIR} )


find_package(Toki 1.0.0 REQUIRED)
set(LIBS ${LIBS} ${Toki_LIBRARY})

find_package(Corpus2 1.0.9 REQUIRED)
include_directories(${Corpus2_INCLUDE_DIR})
set(LIBS ${LIBS} ${Corpus2_LIBRARIES})

find_package(ICU REQUIRED)
include_directories(${ICU_INCLUDE_DIR})
link_directories(${ICU_LIBRARY_DIRS})

include_directories(${maca_BINARY_DIR}/include)
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})

add_executable( maca-convert main.cpp )
target_link_libraries ( maca-convert maca ${Boost_LIBRARIES} ${LIBS})

	install(TARGETS maca-convert
		RUNTIME DESTINATION bin)
