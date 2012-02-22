#ifndef __WORLD_H__
#define __WORLD_H__

#include "region.h"
#include <osg/NodeCallback>
#include <osg/Group>
#include <osg/Geometry>
#include <osg/Geode>
#include <osg/Texture2D>
#include "singleton.h"
#include <generator.h>

class CreateGeomThread;

namespace cube
{
  class World;

  class WorldCallback
    : public osg::NodeCallback
  {
  public:
    WorldCallback(cube::World *world): _world(world) {};
    virtual void operator()(osg::Node* node, osg::NodeVisitor* nv);

  private:
    cube::World *_world;
  };

  class World : public utils::Singleton<World>
  {
  public:
    typedef std::map<int, cube::Region*> YRegionsContainer;
    typedef std::map<int, YRegionsContainer> RegionsContainer;

    struct DataUpdate
    {
      DataUpdate()
      {
        _geom = NULL;
        _reg = NULL;
        _zCubOff = 0;
        _blend = false;
      }

      DataUpdate(osg::Geometry* geom, cube::Region* reg, int geomNumder, bool blend = false)
      {
        _geom = geom;
        _reg = reg;
        _zCubOff = GEOM_SIZE * geomNumder;
        _blend = blend;
      }

      osg::Geometry* _geom;
      cube::Region* _reg;
      int _zCubOff;
      bool _blend;
    };

    World();
    ~World(){}

    osg::Group* GetGeometry();
    void update();

    cube::Region* World::GetRegion(int i, int j) { return _regions[i][j]; }
    cube::Region* ContainsRegion(int xreg, int yreg);
    bool ContainsRegionSafe(int xreg, int yreg);
    cube::Cub* GetCub(float x, float y, float z);

    void del(cube::Cub& cub, cube::Region* reg, int geomIndex, osg::Vec3d wcpos);
    void add(cube::Cub& cub, cube::Region* reg, int geomIndex, osg::Vec3d wcpos, bool recalcLight = false);

    void RemoveCub(osg::Vec3d vec);
    void AddCub(osg::Vec3d vec);
    void UpdateRegionGeoms(cube::Region* rg, bool addToScene = true);

    void ProcessAddRegions();

    std::vector<DataUpdate> _dataUpdate;

    osg::Vec3d _you;

    mutable OpenThreads::Mutex  _mutex;

  protected:
    void updateGeom(osg::Geometry* geom, cube::Region* reg, int zOffset, bool blend = false, bool updateScene = false);
    void clearRegionGeoms(cube::Region* rg);

    float _rnd;
    int _prevRegX, _prevRegY;

    std::list<cube::Region*> _addRegions;
    std::list<std::pair<cube::Region*, Areas::v2>> _delRegions;
    std::list<cube::Region*> _addToSceneRegions;

    osg::Geode* createGeometry();

    osg::Group* _group;
    osg::Geode* _geode[2];

    TextureInfo* _texInfo;

    CreateGeomThread* _cgThread;

    int _frame;

  public:
    RegionsContainer _regions;
    std::map<int, std::map<int, bool>> _regionsCreated;

    friend CreateGeomThread;
  };
}
#endif