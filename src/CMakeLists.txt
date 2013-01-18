
set(SASTP_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS}")

include_directories(${SASTP_INCLUDE_DIR})

add_subdirectory(Framework)
add_subdirectory(Support)
add_subdirectory(Solver)
add_subdirectory(Ants)

add_executable(sastpsolver sastpsolver.cpp)
add_executable(sastpvis    sastpvis.cpp)

target_link_libraries(sastpsolver Solver Ants Support Framework)
target_link_libraries(sastpvis Support Framework)
