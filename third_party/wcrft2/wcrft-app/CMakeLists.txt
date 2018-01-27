PROJECT(wcrft-app)

include_directories(${CMAKE_SOURCE_DIR})
include_directories(${wcrft_BINARY_DIR}/include)

add_executable(wcrft-app main.cpp program_options.cpp)

#wcrft-app needs some additional libs linked
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	find_package(ICU REQUIRED)
	include_directories(${ICU_INCLUDE_DIR})
	set(LIBS ${LIBS} ${ICU_LIBRARIES} ${ICU_I18N_LIBRARIES})
endif(CMAKE_BUILD_TYPE STREQUAL "Debug")

set(LIBS ${LIBS} boost_system)

target_link_libraries(wcrft-app wcrft ${LIBS})

install(TARGETS wcrft-app
		RUNTIME DESTINATION bin)
