
include_directories(${SASTP_INCLUDE_DIR})

add_library(Solver STATIC Driver.cpp GreedyTour.cpp)
