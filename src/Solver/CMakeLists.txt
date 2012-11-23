
include_directories(${SASTP_INCLUDE_DIR})

add_library(Solver STATIC Driver.cpp Neighborhood.cpp AbstractSearch.cpp
                          GreedyTour.cpp LocalSearch.cpp
                          Grasp.cpp GVNS.cpp VND.cpp)
