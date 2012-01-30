project(cube)

include_directories (./include)

SET(SOURCES
    ./src/main.cpp
    ./src/personManipulator.cpp
    ./src/region.cpp
    ./src/world.cpp    
)
add_executable(cube ${SOURCES})

TARGET_LINK_LIBRARIES(cube libosgViewer.so)

