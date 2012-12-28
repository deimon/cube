project(cube)

include_directories (./include)

SET(SOURCES
    ./src/cube.cpp
    ./src/geoMaker.cpp
    ./src/gridUtils.cpp
    ./src/light.cpp
    ./src/main.cpp
    ./src/mathUtils.cpp
    ./src/perlin.cpp
    ./src/personManipulator.cpp
    ./src/region.cpp
    ./src/regionManager.cpp
    ./src/test.cpp
    ./src/world.cpp
    ./src/debugEventHandler.cpp
)

SET(INCLUDE
    ./include/cube.h
    ./include/geoMaker.h
    ./include/gridUtils.h
    ./include/light.h
    ./include/mathUtils.h
    ./include/perlin.h
    ./include/region.h
    ./include/regionManager.h
    ./include/singleton.h
    ./include/wood.h
    ./include/world.h
    ./include/debugEventHandler.h
)

add_executable(cube ${SOURCES} ${INCLUDE})

TARGET_LINK_LIBRARIES(cube libosgViewer.so)

