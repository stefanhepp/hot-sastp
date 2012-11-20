
include_directories(${SASTP_INCLUDE_DIR})

add_library(Support STATIC Environment.cpp Instance.cpp Lookup.cpp)

