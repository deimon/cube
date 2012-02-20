#ifndef __LIGHT_H__
#define __LIGHT_H__

#include <cube.h>
#include <world.h>

namespace cube
{
  class Light
  {
  public:

    typedef std::map<cube::Cub*, osg::Vec3d> MapCubPos;

    static void RecalcAndFillingLight(cube::Cub& cub, osg::Vec3d wcpos, std::map<osg::Geometry*, World::DataUpdate>& updateGeomMap);

    static void fillingLight(cube::Cub* cub, osg::Vec3d wcpos, CubInfo::CubeSide side, float prevLight,
                             std::map<osg::Geometry*, World::DataUpdate>& updateGeomMap);

    static void FindLightSourceAndFillingLight(cube::Cub& cub, osg::Vec3d wcpos, std::map<osg::Geometry*,
                                               World::DataUpdate>& updateGeomMap);

    static void findLightSource(cube::Cub& cub, osg::Vec3d wcpos, MapCubPos& listCubPos,
                                std::map<osg::Geometry*, World::DataUpdate>& updateGeomMap);

  };
}

#endif