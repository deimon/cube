#ifndef __REGION_H__
#define __REGION_H__

#define REGION_WIDTH 16
#define REGION_HEIGHT 128
#define GEOM_SIZE REGION_WIDTH
#define GEOM_COUNT REGION_HEIGHT / GEOM_SIZE
#define NUM_OCTAVES 1
#define PERSON_HEIGHT 1.8
#define CUBS_IN_GEOM REGION_WIDTH * REGION_WIDTH * REGION_WIDTH

#include <cube.h>
#include <osg/Vec3d>
#include <osg/Geometry>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

namespace cube
{
  class World;
  class CubRegion;

  class Region
  {
  public:

    long GetId() { return id; }

    static cube::Region* Generation(int xreg, int yreg);
    void CubFilling(int rnd);
    void Save();
    void LightFilling();
    void RenderFilling();

    static int ToRegionIndex(float worldPos);

    cube::CubRegion GetCub(int x, int y, int z);
    void SetPosition(int xreg, int yreg){ _position.set(REGION_WIDTH * xreg, REGION_WIDTH * yreg, 0.0); }
    const osg::Vec3d& GetPosition(){ return _position; }

    osg::Geometry* GetGeometry(int k, bool blend = false){ if(blend) return _geom[1][k]; else return _geom[0][k]; }
    void SetGeometry(int k, osg::Geometry* geom, bool blend = false){ if(blend) _geom[1][k] = geom; else _geom[0][k] = geom; }

    void SetHeight(int i, int j, int value) { _height[i + 1][j + 1] = value; }
    int GetHeight(int i, int j) { return _height[i + 1][j + 1]; }

    int GetX() { return _xReg; }
    int GetY() { return _yReg; }

    int _renderedCubCount[2][GEOM_COUNT];

    int _airCubCount[2][GEOM_COUNT];

    bool _geometryCreated;

    bool IsCubFilled() { return _cubFilled; }
    bool IsLightFilled() { return _lightFilled; }
    bool IsRenderFilled() { return _renderFilled; }

    bool InVisibleZone() { return _inVisibleZone; }
    void SetVisibleZone(bool inVisibleZone) { _inVisibleZone = inVisibleZone; }

    bool InScene() { return _inScene; }
    void SetInScene(bool inScene) { _inScene = inScene; }

    bool IsOffside() { return _offside; }

  protected:

    Region()
    {
      id = countRegion;
      countRegion++;

      _geometryCreated = false;
      _cubFilled = false;
      _lightFilled = false;
      _renderFilled = false;
      _inVisibleZone = false;
      _inScene = false;
      _offside = false;
    }

    bool _cubFilled;
    bool _lightFilled;
    bool _renderFilled;
    bool _inVisibleZone;
    bool _inScene;
    bool _offside;

    static int countRegion;
    long id;

    cube::Cub _m[REGION_WIDTH][REGION_WIDTH][REGION_HEIGHT];
    int _height[REGION_WIDTH+2][REGION_WIDTH+2];

    int _xReg, _yReg;
    osg::Vec3d _position;

    osg::Geometry* _geom[2][GEOM_COUNT];
  };

  class CubRegion
  {
  public:
    CubRegion(Region* region, Cub& cub, int geomIndex) : _cub(cub) { _region = region; _geomIndex = geomIndex; }

    const Cub* GetCub() { return &_cub; }

    Region* GetRegion() { return _region; }
    int GetGeomIndex() { return _geomIndex; }

    float& GetCubLight() { return _cub._light; }
    float& GetCubLocLight() { return _cub._locLight; }

    const Cub::CubeType& GetCubType() { return _cub._type; }
    void SetCubType(Cub::CubeType type);

    void SetCubRendered(bool rendered);
    bool GetCubRendered() { return _cub._rendered; }

    void SetCubBlend(bool blend);
    bool GetCubBlend() { return _cub._blend; }

  protected:
    Region* _region;
    Cub& _cub;
    int _geomIndex;
  };
}

#endif 