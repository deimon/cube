project(cube)

include_directories (./include)

SET(SOURCES
    ./src/main.cpp
    ./src/personManipulator.cpp
    ./src/region.cpp
    ./src/world.cpp
)

SET(INCLUDE
    ./include/cube.h
    ./include/generator.h
    ./include/geoMaker.h
    ./include/personManipulator.h
    ./include/region.h
    ./include/singleton.h
    ./include/world.h
)

add_executable(cube ${SOURCES} ${INCLUDE})

TARGET_LINK_LIBRARIES(cube libosgViewer.so)

