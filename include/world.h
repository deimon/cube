#ifndef __WORLD_H__
#define __WORLD_H__

#include "region.h"
#include "singleton.h"

#define DUBUGMODE
#ifdef DUBUGMODE
#include "windows.h"
#include <iostream>
#endif

class CreateGeomThread;

namespace cube
{
  class RenderGroup;

  class World : public utils::Singleton<World>
  {
  public:

    typedef std::list<cube::Region*> RegionsList;

    World();
    ~World(){}

    osg::Group* GetGeometry();
    void CreateMap(int rnd);
    void update();

    void del(cube::CubRegion& cubReg, osg::Vec3d wcpos);
    void add(cube::CubRegion& cubReg, osg::Vec3d wcpos, bool recalcLight = false);

    void RemoveCub(osg::Vec3d vec);
    void AddCub(osg::Vec3d vec, Cub::CubeType cubeType);
    void UpdateRegionGeoms(cube::Region* rg, bool addToScene = true);

    osg::Vec3d _you;

    mutable OpenThreads::Mutex  _mutex;

    int _rnd;
    int _newRnd;
    int _worldRadius;

    bool IsMapCreated() { return _mapCreated; }
    bool IsHudMode() { return _hudMode; }
    void SetHudMode(bool hudMode) { _hudMode = hudMode; }

  protected:
    void createMap();
    void destroyMap();

    int _prevRegX, _prevRegY;

    std::list<RegionsList*> _addRegionsForCubFilling;
    std::list<RegionsList*> _addRegionsForLightFilling;
    std::list<RegionsList*> _addRegionsForRenderFilling;
    RegionsList _delRegionsForVisual;
    RegionsList _addToSceneRegions;

    CreateGeomThread* _cgThread;

    RenderGroup* _renderGroup;

    int _frame;
    int _radius;

    bool _mapCreated;
    bool _newMap;
    bool _hudMode;

  public:
    friend class CreateGeomThread;
  };
}
#endif