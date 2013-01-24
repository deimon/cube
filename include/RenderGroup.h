#ifndef __RENDER_GROUP_H__
#define __RENDER_GROUP_H__

#include <osg/Group>

#include <definition.h>

namespace osg
{
  class Geometry;
}

namespace cube
{
  class Region;
  class World;
  class TextureInfo;

  class RenderGroup: public osg::Group
  {
  public:

    RenderGroup(cube::World *world);

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

    typedef std::map<osg::Geometry*, RenderGroup::DataUpdate> DataUpdateContainer;

    void Update();
    void PushToUpdate(DataUpdateContainer* updateGeomMap);
    void ClearDataUpdate() { _dataUpdate.clear(); }

    void UpdateRegionGeoms(cube::Region* rg, bool addToScene);
    void FillRegionGeoms(cube::Region* rg);
    void ClearRegionGeoms(cube::Region* rg);

    DataUpdateContainer _dataUpdate;

  protected:
    void updateGeom(osg::Geometry* geom, cube::Region* reg, int zOffset, bool blend = false, bool updateScene = false);

    osg::Geode* _geode[2];

    TextureInfo* _texInfo;

    osg::Uniform* _worldLight;
  };
}

#endif