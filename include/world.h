#ifndef __WORLD_H__
#define __WORLD_H__

#include "region.h"
#include <osg/NodeCallback>
#include <osg/Group>
#include <osg/Geometry>
#include <osg/Geode>
#include <osg/Texture2D>
#include "singleton.h"

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

    World();
    ~World(){}

    osg::Group* GetGeometry();
    void update();

    cube::Region* World::GetRegion(int i, int j) { return _regions[i][j]; }
    cube::Region* ContainsReion(int xreg, int yreg);
    const cube::Cub& GetCub(float x, float y, float z);

    void RemoveCub(osg::Vec3d vec);
    void AddCub(osg::Vec3d vec);
    void UpdateRegionGeoms(cube::Region* rg);

    void ProcessAddRegions();

    struct DataUpdate
    {
      DataUpdate(osg::Geometry* geom, cube::Region* reg, int geomNumder)
      {
        _geom = geom;
        _reg = reg;
        _zCubOff = GEOM_SIZE * geomNumder;
      }

      osg::Geometry* _geom;
      cube::Region* _reg;
      int _zCubOff;
    };

    std::vector<DataUpdate> _dataUpdate;

    osg::Vec3d _you;

    mutable OpenThreads::Mutex  _mutex;

  protected:
    void updateGeom(osg::Geometry* geom, cube::Region* reg, int zOffset);
    void clearRegionGeoms(cube::Region* rg);

    float _rnd;
    int _prevRegX, _prevRegY;

    std::map<long, cube::Region*> _addRegions;
    std::map<long, cube::Region*> _delRegions;
    osg::Geode::DrawableList _addDrwList;
    osg::Geode::DrawableList _delDrwList;

    osg::Geode* createGeometry();

    osg::Group* _group;
    osg::Geode* _geode;

    TextureInfo* _texInfo;

  public:
    RegionsContainer _regions;
  };
}
#endif