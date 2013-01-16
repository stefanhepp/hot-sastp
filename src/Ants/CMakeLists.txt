
include_directories(${SASTP_INCLUDE_DIR})

add_library(Ants STATIC PheromoneMatrix.cpp Ant.cpp ACO.cpp)
