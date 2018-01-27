PROJECT(learn_to_guess)
cmake_minimum_required(VERSION 2.8.0)

FIND_PACKAGE(Boost 1.48 COMPONENTS program_options system filesystem REQUIRED)
link_directories(${Boost_LIBRARY_DIRS})

if (NOT Boost_FOUND)
	MESSAGE(FATAL_ERROR "Boost not found!")
endif ()
set(LIBS ${LIBS} ${Boost_LIBRARIES})


find_package(ICU REQUIRED)
include_directories(${ICU_INCLUDE_DIR})
set(LIBS ${LIBS} ${ICU_LIBRARIES} ${ICU_I18N_LIBRARIES} icuio)


include_directories(${Boost_INCLUDE_DIRS})
add_executable(learn_to_guess main.cpp tree.cpp config.cpp)
target_link_libraries(learn_to_guess corpus2 ${LIBS})
