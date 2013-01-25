#ifndef __GRID_UTILS_H__
#define __GRID_UTILS_H__

#include <RenderGroup.h>
#include <baseBlock.h>

namespace osg
{
  class Vec3d;
}

namespace cube
{
  class GridUtils
  {
  public:
    static void RemoveCub(osg::Vec3d vec, RenderGroup::DataUpdateContainer* updateGeomMap);
    static void AddCub(osg::Vec3d vec, Block::BlockType cubeType, RenderGroup::DataUpdateContainer* updateGeomMap);

  private:
    static void del(cube::CubRegion& cubReg, osg::Vec3d wcpos, RenderGroup::DataUpdateContainer* updateGeomMap);
    static void add(cube::CubRegion& cubReg, osg::Vec3d wcpos, bool recalcLight, RenderGroup::DataUpdateContainer* updateGeomMap);
  };
}

#endif