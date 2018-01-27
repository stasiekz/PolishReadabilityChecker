PROJECT(maca-analyse)

include_directories( ${CMAKE_SOURCE_DIR} )


find_package(Toki 1.0.0 REQUIRED)
set(LIBS ${LIBS} ${Toki_LIBRARY})

find_package(Corpus2 1.0.9 REQUIRED)
set(LIBS ${LIBS} ${Corpus2_LIBRARIES})
include_directories(${Corpus2_INCLUDE_DIR})

find_package(Boost 1.41 REQUIRED COMPONENTS program_options system filesystem regex)

include_directories(${maca_BINARY_DIR}/include)
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})

find_package(ICU REQUIRED)
include_directories(${ICU_INCLUDE_DIR})
link_directories(${ICU_LIBRARY_DIRS})

find_package(OpenMP)
if (OPENMP_FOUND)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
	set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${OpenMP_C_FLAGS}")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${OpenMP_CXX_FLAGS}")
endif(OPENMP_FOUND)

add_executable( maca-analyse main.cpp )
add_executable( maca-analyse-batch batch.cpp )

target_link_libraries ( maca-analyse maca ${Boost_LIBRARIES} ${LIBS})
target_link_libraries ( maca-analyse-batch maca ${Boost_LIBRARIES} ${LIBS})


	install(TARGETS maca-analyse
		RUNTIME DESTINATION bin)
	install(TARGETS maca-analyse-batch
		RUNTIME DESTINATION bin)
