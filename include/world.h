#ifndef __WORLD_H__
#define __WORLD_H__

#include "region.h"
#include <osg/NodeCallback>
#include <osg/Group>
#include <osg/Geometry>
#include <osg/Geode>
#include <osg/Texture2D>
#include "singleton.h"

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

    typedef std::list<cube::Region*> RegionsList;

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

    void del(cube::Cub& cub, cube::Region* reg, int geomIndex, osg::Vec3d wcpos);
    void add(cube::Cub& cub, cube::Region* reg, int geomIndex, osg::Vec3d wcpos, bool recalcLight = false);

    void RemoveCub(osg::Vec3d vec);
    void AddCub(osg::Vec3d vec);
    void UpdateRegionGeoms(cube::Region* rg, bool addToScene = true);

    std::vector<DataUpdate> _dataUpdate;

    osg::Vec3d _you;

    mutable OpenThreads::Mutex  _mutex;

    float _rnd;

  protected:
    void updateGeom(osg::Geometry* geom, cube::Region* reg, int zOffset, bool blend = false, bool updateScene = false);
    void clearRegionGeoms(cube::Region* rg);

    int _prevRegX, _prevRegY;

    std::list<RegionsList*> _addRegionsForVisual;
    std::list<RegionsList*> _addRegionsForCalc;
    RegionsList _delRegionsForVisual;
    RegionsList _addToSceneRegions;

    osg::Geode* createGeometry();

    osg::Group* _group;
    osg::Geode* _geode[2];

    TextureInfo* _texInfo;

    CreateGeomThread* _cgThread;

    int _frame;
    int _radius;

  public:
    friend CreateGeomThread;
  };
}
#endif