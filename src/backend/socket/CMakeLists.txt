include_directories(${CMAKEDEMO_SOURCE_DIR}/src/backend/socket)
link_directories(${CMAKEDEMO_BINARY_DIR}/src/backend/socket)

add_library(ksocket ksocket.c)