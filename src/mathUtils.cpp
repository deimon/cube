#include <mathUtils.h>
#include <osg/Vec3>

using namespace cube;

CubInfo::CubeSide MathUtils::CubIntersection(cube::Region* reg, int xreg, int yreg, int zreg, osg::Vec3d X, osg::Vec3d Y)
{
  osg::Vec3d vertexOffset = reg->GetPosition() + osg::Vec3d(xreg, yreg, zreg);
  X -= vertexOffset;
  Y -= vertexOffset;

  CubInfo::CubeSide curSide = CubInfo::FirstSide;
  float minLength = 100000.0;

  for(int i = CubInfo::FirstSide; i <= CubInfo::EndSide; i++)
  {
    CubInfo::CubeSide side = (CubInfo::CubeSide)i;

    osg::Vec3d N = CubInfo::Instance().GetNormal(side);

    float D = 0.0;
    if(side == CubInfo::X_FACE || side == CubInfo::Y_FACE || side == CubInfo::Z_FACE)
      D = -1.0;

    osg::Vec3d XY = Y - X;
    float k = (-(N * X + D)) / (N * XY);

    osg::Vec3d P0 = XY * k + X;

    osg::Vec3d XP0 = P0 - X;
    float length2 = XP0.length2();


    if(side == CubInfo::X_FACE || side == CubInfo::X_BACK)
    {
      if(P0.y() >= 0.0 && P0.y() <= 1.0 && P0.z() >= 0.0 && P0.z() <= 1.0 && length2 < minLength)
      {
        minLength = length2;
        curSide = side;
      }
    }
    else if(side == CubInfo::Y_FACE || side == CubInfo::Y_BACK)
    {
      if(P0.x() >= 0.0 && P0.x() <= 1.0 && P0.z() >= 0.0 && P0.z() <= 1.0 && length2 < minLength)
      {
        minLength = length2;
        curSide = side;
      }
    }
    else if(side == CubInfo::Z_FACE || side == CubInfo::Z_BACK)
    {
      if(P0.x() >= 0.0 && P0.x() <= 1.0 && P0.y() >= 0.0 && P0.y() <= 1.0 && length2 < minLength)
      {
        minLength = length2;
        curSide = side;
      }
    }
  }

  return curSide;
}