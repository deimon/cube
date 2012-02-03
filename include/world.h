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

    cube::Region* GetRegion(int x, int y);
    cube::Region* ContainsReion(int xreg, int yreg);
    const cube::Cub& GetCub(float x, float y, float z);

    void RemoveCub(osg::Vec3d vec);

    struct DataUpdate
    {
      DataUpdate(osg::Geometry* geom, cube::Region* reg, osg::Vec3d& vec)
      {
        _geom = geom;
        _reg = reg;
        _xCubOff = GEOM_SIZE * (int)vec.x();
        _yCubOff = GEOM_SIZE * (int)vec.y();
        _zCubOff = GEOM_SIZE * (int)vec.z();
      }

      osg::Geometry* _geom;
      cube::Region* _reg;
      int _xCubOff, _yCubOff, _zCubOff;
    };

    std::vector<DataUpdate> _dataUpdate;

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