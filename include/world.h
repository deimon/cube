#ifndef __WORLD_H__
#define __WORLD_H__

#include "region.h"
#include <osg/NodeCallback>
#include <osg/Group>
#include <osg/Geometry>
#include <osg/Geode>
#include <osg/Texture2D>
#include <osgDB/ReadFile>
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

    struct DataUpdate
    {
      DataUpdate(osg::Geometry* geom, cube::Region* reg, osg::Vec3d& vec)
      {
        _geom = geom;
        _reg = reg;
        _zCubOff = GEOM_SIZE * (int)vec.z();
      }

      osg::Geometry* _geom;
      cube::Region* _reg;
      int _zCubOff;
    };

    std::vector<DataUpdate> _dataUpdate;

    osg::Vec3d _you;

  protected:
    osg::Geode* createGeometry();

    osg::Group* _group;
    osg::Geode* _geode;

    std::vector<osg::Vec3d> _sides;

  public:
    RegionsContainer _regions;
  };
}
#endif