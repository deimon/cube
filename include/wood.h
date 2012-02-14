#ifndef __WOOD_H__
#define __WOOD_H__

#include <iostream>
#include <world.h>
#include <mathUtils.h>

namespace cube
{
  class Wood
  {
  public:
    static void Generate(cube::World* world, cube::Region* rg, int rgx, int rgy)
    {
      int rgz = rg->GetHeight(rgx, rgy);

      int height = 1;
      float limit = 0.0f;

      while(cube::MathUtils::random() > limit)
      {
        cube::Cub& cub = rg->GetCub(rgx, rgy, rgz + height);

        if(cub._type == cube::Cub::Air)
        {
          cub._type = cube::Cub::TruncWood;
          cub._rendered = true;
          rg->_renderedCubCount[(rgz + height) / GEOM_SIZE]++;
        }

        height++;
        if(height > 4)
          limit += 0.2f;
      }

      osg::Vec3d pos = rg->GetPosition() + osg::Vec3d(rgx + 0.5f, rgy + 0.5f, rgz + height - 2);
      GenLeafRecursive(world, pos.x(), pos.y(), pos.z(), 2);
    }
  protected:
    static void GenLeafRecursive(cube::World* world, float x, float y, float z, int r)
    {
      if(r == 6)
        return;

      cube::Region* rg = world->ContainsRegion(Region::ToRegionIndex(x), Region::ToRegionIndex(y));

      if(rg)
      {
        float cx = x - rg->GetPosition().x();
        float cy = y - rg->GetPosition().y();
        cube::Cub& cub = rg->GetCub(cx, cy, z);

        if(cub._type == cube::Cub::Air || cub._type == cube::Cub::TruncWood || cub._type == cube::Cub::LeavesWood)
        {
          if(cub._type == cube::Cub::Air)
          {
            cub._type = cube::Cub::LeavesWood;
            cub._rendered = true;
            rg->_renderedCubCount[(int)z / GEOM_SIZE]++;
          }

          if(cube::MathUtils::random() > 0.05f * r)
            GenLeafRecursive(world, x, y, z + 1, r + 1);

          float k = 0.1f * r;

          if(cube::MathUtils::random() > k)
            GenLeafRecursive(world, x + 1, y, z, r + 1);
          if(cube::MathUtils::random() > k)
            GenLeafRecursive(world, x - 1, y, z, r + 1);
          if(cube::MathUtils::random() > k)
            GenLeafRecursive(world, x, y + 1, z, r + 1);
          if(cube::MathUtils::random() > k)
            GenLeafRecursive(world, x, y - 1, z, r + 1);
        }
      }
    }
  };
}

#endif