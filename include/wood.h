#ifndef __WOOD_H__
#define __WOOD_H__

#include <iostream>
#include <world.h>
#include <mathUtils.h>
#include <regionManager.h>

namespace cube
{
  class Wood
  {
  public:
    static void Generate(cube::RegionManager& regMgr, cube::Region* rg, int rgx, int rgy)
    {
      int rgz = rg->GetHeight(rgx, rgy);

      int height = 1;
      float limit = 0.0f;

      while(cube::MathUtils::random() > limit)
      {
        cube::CubRegion cubReg = rg->GetCub(rgx, rgy, rgz + height);

        if(cubReg.GetCubType() == cube::Cub::Air)
        {
          cubReg.SetCubType(cube::Cub::TruncWood);
          cubReg.SetCubRendered(true);
        }

        height++;
        if(height > 4)
          limit += 0.2f;
      }

      osg::Vec3d pos = rg->GetPosition() + osg::Vec3d(rgx + 0.5f, rgy + 0.5f, rgz + height - 2);
      GenLeafRecursive(regMgr, pos.x(), pos.y(), pos.z(), 2);
    }
  protected:
    static void GenLeafRecursive(cube::RegionManager& regMgr, float x, float y, float z, int r)
    {
      if(r == 6)
        return;

      cube::Region* rg = regMgr.ContainsRegion(Region::ToRegionIndex(x), Region::ToRegionIndex(y));

      if(rg)
      {
        float cx = x - rg->GetPosition().x();
        float cy = y - rg->GetPosition().y();
        cube::CubRegion cubReg = rg->GetCub(cx, cy, z);

        if(  cubReg.GetCubType() == cube::Cub::Air 
          || cubReg.GetCubType() == cube::Cub::TruncWood 
          || cubReg.GetCubType() == cube::Cub::LeavesWood)
        {
          if(cubReg.GetCubType() == cube::Cub::Air)
          {
            cubReg.SetCubType(cube::Cub::LeavesWood);
            cubReg.SetCubRendered(true);
          }

          if(cube::MathUtils::random() > 0.05f * r)
            GenLeafRecursive(regMgr, x, y, z + 1, r + 1);

          float k = 0.1f * r;

          if(cube::MathUtils::random() > k)
            GenLeafRecursive(regMgr, x + 1, y, z, r + 1);
          if(cube::MathUtils::random() > k)
            GenLeafRecursive(regMgr, x - 1, y, z, r + 1);
          if(cube::MathUtils::random() > k)
            GenLeafRecursive(regMgr, x, y + 1, z, r + 1);
          if(cube::MathUtils::random() > k)
            GenLeafRecursive(regMgr, x, y - 1, z, r + 1);
        }
      }
    }
  };
}

#endif