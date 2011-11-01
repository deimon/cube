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

    cube::Region* GetRegion(float x, float y);
    const cube::Cub& GetCub(float x, float y, float z);

    std::vector<osg::Vec3d> _cubUpdate;

  protected:
    osg::Geometry* createGeometry();
    osg::Geode* createGeometry2();

    osg::Group* _group;

  public:
    RegionsContainer _regions;
  };
}

#endif 