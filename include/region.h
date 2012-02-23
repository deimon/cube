#ifndef __REGION_H__
#define __REGION_H__

#define REGION_WIDTH 8
#define REGION_HEIGHT 128
#define GEOM_SIZE REGION_WIDTH
#define GEOM_COUNT 16
#define NUM_OCTAVES 1
#define PERSON_HEIGHT 1.8

#include <cube.h>
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

    long GetId() { return id; }

    static cube::Region* Generation(int xreg, int yreg);
    void FillRegion(float rnd);

    static int ToRegionIndex(float worldPos);

    cube::Cub& GetCub(int x, int y, int z){ return _m[x][y][z];}
    const osg::Vec3d& GetPosition(){ return _position;}

    osg::Geometry* GetGeometry(int k, bool blend = false){ if(blend) return _geom[1][k]; else return _geom[0][k]; }
    void SetGeometry(int k, osg::Geometry* geom, bool blend = false){ if(blend) _geom[1][k] = geom; else _geom[0][k] = geom; }

    void SetHeight(int i, int j, int value) { _height[i + 1][j + 1] = value; }
    int GetHeight(int i, int j) { return _height[i + 1][j + 1]; }

    int GetX() { return _xReg; }
    int GetY() { return _yReg; }

    int _renderedCubCount[2][GEOM_COUNT];

    bool _geometryCreated;

    bool IsAreaGenerated() { return _areaGenerated; }

    bool InVisibleZone() { return _inVisibleZone; }
    void SetVisibleZone(bool inVisibleZone) { _inVisibleZone = inVisibleZone; }

    bool InScene() { return _inScene; }
    void SetInScene(bool inScene) { _inScene = inScene; }

  protected:

    Region()
    {
      id = countRegion;
      countRegion++;

      _geometryCreated = false;
      _areaGenerated = false;
      _inVisibleZone = false;
      _inScene = false;
    }

    bool _areaGenerated;
    bool _inVisibleZone;
    bool _inScene;

    static int countRegion;
    long id;

    cube::Cub _m[REGION_WIDTH][REGION_WIDTH][REGION_HEIGHT];
    int _height[REGION_WIDTH+2][REGION_WIDTH+2];

    int _xReg, _yReg;
    osg::Vec3d _position;

    osg::Geometry* _geom[2][GEOM_COUNT];
  };
}

#endif 