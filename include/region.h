#ifndef __REGION_H__
#define __REGION_H__

#define REGION_WIDTH 8
#define REGION_HEIGHT 128
#define GEOM_SIZE REGION_WIDTH
#define GEOM_COUNT 16
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

    osg::Geometry* GetGeometry(int k){ return _geom[k]; }
    void SetGeometry(int k, osg::Geometry* geom){ _geom[k] = geom; }

    void SetHeight(int i, int j, int value) { _height[i + 1][j + 1] = value; }
    int GetHeight(int i, int j) { return _height[i + 1][j + 1]; }

    int GetX() { return _xReg; }
    int GetY() { return _yReg; }

    int _renderedCubCount[GEOM_COUNT];

  protected:

    cube::Cub _m[REGION_WIDTH][REGION_WIDTH][REGION_HEIGHT];
    int _height[REGION_WIDTH+2][REGION_WIDTH+2];

    int _xReg, _yReg;
    osg::Vec3d _position;

    osg::Geometry* _geom[GEOM_COUNT];
  };
}

#endif 