#ifndef __REGION_H__
#define __REGION_H__

#define REGION_SIZE 64
#define REGION_WIDTH REGION_SIZE * CUBE_SIZE
#define GEOM_DEVIDER_SIZE 8
#define GEOM_SIZE REGION_SIZE / GEOM_DEVIDER_SIZE
#define NUM_OCTAVES 1
#define PERSON_HEIGHT 1.8

#include "cube.h"
#include <osg/Vec3d>
#include <osg/Geometry>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

namespace cube
{
  class World;

  class Region
  {
  public:

    static void Generation(cube::World* world, int xreg, int yreg, float rnd);
    cube::Cub& GetCub(int x, int y, int z){ return _m[x][y][z];}
    const osg::Vec3d& GetPosition(){ return _position;}

    osg::Geometry* GetGeometry(int i, int j, int k){ return _geom[i][j][k]; }
    void SetGeometry(int i, int j, int k, osg::Geometry* geom){ _geom[i][j][k] = geom; }

    void SetHeight(int i, int j, int value) { _height[i + 1][j + 1] = value; }
    int GetHeight(int i, int j) { return _height[i + 1][j + 1]; }

    int GetX() { return _xReg; }
    int GetY() { return _yReg; }

  protected:

    cube::Cub _m[REGION_SIZE][REGION_SIZE][REGION_SIZE];
    int _height[REGION_SIZE+2][REGION_SIZE+2];

    int _xReg, _yReg;
    osg::Vec3d _position;

    osg::Geometry* _geom[GEOM_SIZE][GEOM_SIZE][GEOM_SIZE];
  };
}

#endif 