#ifndef __MATH_UTILS_H__
#define __MATH_UTILS_H__

#include <region.h>
#include <osg/Vec3>

namespace cube
{
  class MathUtils
  {
  public:
    static CubInfo::CubeSide CubIntersection(cube::Region* reg, int xreg, int yreg, int zreg, osg::Vec3d X, osg::Vec3d Y);
  };
}

#endif