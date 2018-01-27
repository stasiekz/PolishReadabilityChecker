PROJECT(toki-app)

include_directories( ${CMAKE_SOURCE_DIR} )



include_directories(${PwrUtils_INCLUDE_DIR})
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})

include_directories(${ICU_INCLUDE_DIR})
link_directories(${ICU_LIBRARY_DIRS})
add_executable( toki-app main.cpp )
target_link_libraries ( toki-app toki ${Boost_LIBRARIES} ${ICU_LIBRARY} )

	install(TARGETS toki-app
		RUNTIME DESTINATION bin)
