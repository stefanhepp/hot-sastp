
set(SASTP_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_CXX_FLAGS "-std=c++0x")

include_directories(${SASTP_INCLUDE_DIR})

add_subdirectory(Framework)
add_subdirectory(Support)
add_subdirectory(Solver)
add_subdirectory(Ants)

add_executable(sastpsolver sastpsolver.cpp)
add_executable(sastpvis    sastpvis.cpp)

target_link_libraries(sastpsolver Ants Solver Support Framework)
target_link_libraries(sastpvis Support Framework)
