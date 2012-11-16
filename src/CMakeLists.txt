
set(SASTP_INCLUDE_DIR ${CMAKE_SOURCE_DIR})


set(CMAKE_CXX_FLAGS "-std=c++0x")

add_subdirectory(Framework)
add_subdirectory(Solver)
add_subdirectory(Common)

add_executable(solver solver.cpp)

target_link_libraries(solver Framework Solver Common)
