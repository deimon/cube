#ifndef __LIGHT_H__
#define __LIGHT_H__

#include <cube.h>
#include <world.h>

namespace cube
{
  class Light
  {
  public:

    typedef std::map<const cube::Cub*, std::pair<osg::Vec3d, float*>> MapCubPos;

    static void RecalcAndFillingLight(cube::CubRegion& cubReg, osg::Vec3d wcpos, 
                                      std::map<osg::Geometry*, World::DataUpdate>* updateGeomMap = NULL);

    static void fillingLight(cube::CubRegion& cubReg, osg::Vec3d wcpos, CubInfo::CubeSide side, float prevLight,
                             std::map<osg::Geometry*, World::DataUpdate>* updateGeomMap = NULL);

    static void FindLightSourceAndFillingLight(cube::CubRegion& cubReg, osg::Vec3d wcpos, std::map<osg::Geometry*,
                                               World::DataUpdate>* updateGeomMap = NULL);

    static void findLightSource(cube::CubRegion& cubReg, osg::Vec3d wcpos, MapCubPos& listCubPos,
                                std::map<osg::Geometry*, World::DataUpdate>* updateGeomMap = NULL);

    static void fillingLocLight(cube::CubRegion& cubReg, osg::Vec3d wcpos, float prevLight,
      std::map<osg::Geometry*, World::DataUpdate>* updateGeomMap = NULL);
  };
}

#endif